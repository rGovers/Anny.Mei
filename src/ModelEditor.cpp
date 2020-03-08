#include "ModelEditor.h"

#include <algorithm>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <string.h>

#include "Camera.h"
#include "ColorTheme.h"
#include "DataStore.h"
#include "FileUtils.h"
#include "imgui/imgui.h"
#include "IntermediateRenderer.h"
#include "Models/Model.h"
#include "Models/MorphPlaneModel.h"
#include "MorphPlane.h"
#include "Name.h"
#include "Namer.h"
#include "PropertyFile.h"
#include "Renderers/ImageDisplay.h"
#include "Renderers/MorphPlaneDisplay.h"
#include "RenderTexture.h"
#include "StaticTransform.h"
#include "WindowControls/ModelEditorWindow.h"
#include "Workspace.h"

const static int IMAGE_SIZE = 2048;

ModelEditor::ModelEditor(Workspace* a_workspace)
{
    m_workspace = a_workspace;

    m_models = new std::list<ModelData*>();

    m_window = new ModelEditorWindow(this);

    m_namer = new Namer();
    m_morphPlaneNamer = new Namer();

    m_renderTexture = new RenderTexture(IMAGE_SIZE, IMAGE_SIZE, GL_RGBA);

    m_intermediateRenderer = new IntermediateRenderer();

    m_imageDisplay = new ImageDisplay();
    m_morphPlaneDisplay = new MorphPlaneDisplay();

    m_selectedModelData = nullptr;
    m_selectedMorphPlane = nullptr;
}  
ModelEditor::~ModelEditor()
{
    delete m_window;

    for (auto iter = m_models->begin(); iter != m_models->end(); ++iter)
    {
        delete[] (*iter)->TextureName;
        delete (*iter)->ModelName;
        delete (*iter)->BaseModel;

        delete *iter;
    }

    delete m_models;

    delete m_renderTexture;

    delete m_imageDisplay;

    delete m_namer;
    delete m_morphPlaneNamer;

    delete m_intermediateRenderer;
}

bool ModelEditor::IsModelSelected() const
{
    return m_selectedModelData != nullptr;
}
bool ModelEditor::IsMorphPlaneSelected() const
{
    return m_selectedMorphPlane != nullptr;
}

void ModelEditor::DrawModelList()
{
    DataStore* store = DataStore::GetInstance();

    ImGui::BeginChild("Layer Model Scroll");

    auto removeIter = m_models->end();

    for (auto iter = m_models->begin(); iter != m_models->end(); ++iter)
    {
        bool is_selected = (*iter == m_selectedModelData);

        if (ImGui::Selectable((*iter)->ModelName->GetName(), &is_selected))
        {
            m_selectedModelData = *iter;
            m_selectedMorphPlane = nullptr;

            m_workspace->SelectWorkspace(this);
        }

        if (is_selected)
        {
            ImGui::SetItemDefaultFocus();
        }

        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Remove Model"))
            {
                removeIter = iter;
            }

            ImGui::EndPopup();
        }
    }

    if (removeIter != m_models->end())
    {
        store->RemoteModel((*removeIter)->ModelName->GetName(), (*removeIter)->BaseModel->GetModelType());

        if (m_selectedModelData == *removeIter)
        {
            m_selectedModelData = nullptr;
        }

        delete[] (*removeIter)->TextureName;
        delete (*removeIter)->ModelName;
        delete (*removeIter)->BaseModel;

        delete *removeIter;

        m_models->erase(removeIter);
    }

    ImGui::EndChild();
}

void ModelEditor::GetModelData(PropertyFileProperty& a_property, mz_zip_archive& a_archive)
{
    DataStore* dataStore = DataStore::GetInstance();

    char* name = nullptr;
    char* trueName = nullptr;
    char* texName = nullptr;
    int vertexCount = -1;
    int indexCount = -1;
    int modelType = (int)e_ModelType::Base;

    const std::list<PropertyFileValue> values = a_property.Values();
    for (auto iter = values.begin(); iter != values.end(); ++iter)
    {
        IFSETTOATTVALCPY("name", iter->Name, name, iter->Value)
        else IFSETTOATTVALCPY("truename", iter->Name, trueName, iter->Value)
        else IFSETTOATTVALCPY("texname", iter->Name, texName, iter->Value)
        else IFSETTOATTVALI("vertices", iter->Name, vertexCount, iter->Value)
        else IFSETTOATTVALI("indices", iter->Name, indexCount, iter->Value)
    }

    if (name != nullptr && trueName != nullptr && vertexCount > 0 && indexCount > 0)
    {
        std::string vertexFileName = "mdl/" + std::string(name) + ".verbin";
        std::string indexFileName = "mdl/" + std::string(name) + ".indbin";

        unsigned int* indicies = (unsigned int*)ExtractFileFromArchive(indexFileName.c_str(), a_archive);
        ModelVertex* vertices = (ModelVertex*)ExtractFileFromArchive(vertexFileName.c_str(), a_archive);

        ModelData* modelData = AddModel(texName, name, trueName, vertices, vertexCount, indicies, indexCount, (e_ModelType)modelType);

        const std::list<PropertyFileProperty*> children = a_property.GetChildren();
        for (auto iter = children.begin(); iter != children.end(); ++iter)
        {
            const char* name = (*iter)->GetName();

            const std::list<PropertyFileValue> mValue = (*iter)->Values();
            if (strcmp(name, "MorphPlane") == 0)
            {
                char* valName = nullptr;
                char* valTrueName = nullptr;
                int valSize = -1;

                for (auto valIter = mValue.begin(); valIter != mValue.end(); ++valIter)
                {
                    IFSETTOATTVALCPY("name", valIter->Name, valName, valIter->Value)
                    else IFSETTOATTVALCPY("truename", valIter->Name, valTrueName, valIter->Value)
                    else IFSETTOATTVALI("size", valIter->Name, valSize, valIter->Value)
                }

                if (valName != nullptr && valTrueName != nullptr && valSize != -1)
                {
                    if (modelData->MorphPlaneModel == nullptr)
                    {
                        modelData->MorphPlaneModel = new MorphPlaneModel();

                        modelData->MorphPlaneSize = valSize;
                        
                        GenerateMorphVertexData(modelData);

                        dataStore->AddModel(modelData->ModelName->GetName(), modelData->MorphPlaneModel);
                    }

                    MorphPlane* morphPlane = MorphPlane::Load((std::string("mrpPln/") + valName + ".mrpbin").c_str(), a_archive, valSize);
                    
                    MorphPlaneData* morphData = new MorphPlaneData
                    {
                        new Name(valTrueName, m_morphPlaneNamer),
                        morphPlane
                    };
                    morphData->MorphPlaneName->SetName(valName);

                    modelData->MorphPlanes.emplace_back(morphData);

                    dataStore->AddMorphPlane(valName, morphPlane);
                }
            }
        }

        if (modelData != nullptr)
        {
            m_models->emplace_back(modelData);
        }

        if (texName != nullptr)
        {
            delete[] texName;
        }
        delete[] trueName;
        delete[] name;
    }
}

ModelData* ModelEditor::AddModel(const char* a_textureName, const char* a_name, const char* a_trueName, ModelVertex* a_vertices, unsigned int a_vertexCount, unsigned int* a_indices, unsigned int a_indexCount, e_ModelType a_modelType) const
{
    if (a_vertexCount <= 0 || a_indexCount <= 0 || a_trueName == nullptr)
    {
        return nullptr;
    }

    DataStore* dataStore = DataStore::GetInstance();

    ModelData* modelData = new ModelData();

    if (a_textureName != nullptr)
    {
        const size_t strLen = strlen(a_textureName);

        modelData->TextureName = new char[strLen];
        strcpy(modelData->TextureName, a_textureName);
    }
    
    size_t vertexSize = 0;

    Model* model;

    switch (a_modelType)
    {
    case e_ModelType::MorphPlane:
    {
        model = modelData->MorphPlaneModel = new MorphPlaneModel();

        vertexSize = sizeof(MorphPlaneModelVertex);

        break;
    }
    default:
    {
        model = modelData->BaseModel = new Model();            
        
        vertexSize = sizeof(ModelVertex);

        break;
    }
    }
    
    modelData->ModelName = new Name(a_trueName, m_namer);
    modelData->VertexCount = a_vertexCount;
    modelData->Vertices = a_vertices;
    modelData->IndexCount = a_indexCount;
    modelData->Indices = a_indices;
    modelData->MorphPlaneSize = 10;

    if (a_name != nullptr)
    {
        modelData->ModelName->SetName(a_name);
    }

    const unsigned int vbo = model->GetVBO();
    const unsigned int ibo = model->GetIBO(); 

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, a_vertexCount * vertexSize, a_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, a_indexCount * sizeof(unsigned int), a_indices, GL_STATIC_DRAW);

    model->SetIndicesCount(a_indexCount);

    const char* name = modelData->ModelName->GetName();

    dataStore->AddModel(name, model);
    dataStore->SetModelTextureName(name, a_textureName);

    return modelData;
}

void ModelEditor::GenerateMorphVertexData(ModelData* a_model) const
{
    const int ibo = a_model->MorphPlaneModel->GetIBO();
    const int vbo = a_model->MorphPlaneModel->GetVBO();
    
    const unsigned int vertexCount = a_model->VertexCount;

    MorphPlaneModelVertex* verts = new MorphPlaneModelVertex[vertexCount];

    const ModelVertex* modelVerts = a_model->Vertices;

    const unsigned int planeSize = a_model->MorphPlaneSize;
    const unsigned int morphPlaneSize = planeSize + 1;
    const float scale = 1.0f / planeSize; 

    for (unsigned int i = 0; i < vertexCount; ++i)
    {
        const glm::vec4 pos = modelVerts[i].Position;

        verts[i].Position = pos;
        verts[i].TexCoord = modelVerts[i].TexCoord;

        const glm::vec2 pos2 = { pos.x, pos.y };
        const glm::vec2 posScale = pos2 * (float)planeSize;

        const glm::vec2 min = glm::vec2(glm::floor(posScale.x), glm::floor(posScale.y));
        const glm::vec2 max = glm::vec2(glm::ceil(posScale.x), glm::ceil(posScale.y));

        float len = glm::length(glm::vec2(min) - posScale);
        verts[i].MorphPlaneWeights[0].r = min.x + min.y * morphPlaneSize;
        verts[i].MorphPlaneWeights[0].g = glm::clamp((1 - len - scale) / planeSize, 0.0f, 1.0f);

        len = glm::length(glm::vec2(min.x, max.y) - posScale);
        verts[i].MorphPlaneWeights[1].r = min.x + max.y * morphPlaneSize;
        verts[i].MorphPlaneWeights[1].g = glm::clamp((1 - len - scale) / planeSize, 0.0f, 1.0f);
        
        len = glm::length(glm::vec2(max.x, min.y) - posScale);
        verts[i].MorphPlaneWeights[2].r = max.x + min.y * morphPlaneSize;
        verts[i].MorphPlaneWeights[2].g = glm::clamp((1 - len - scale) / planeSize, 0.0f, 1.0f);
        
        len = glm::length(glm::vec2(max) - posScale);
        verts[i].MorphPlaneWeights[3].r = max.x + max.y * morphPlaneSize;
        verts[i].MorphPlaneWeights[3].g = glm::clamp((1 - len - scale) / planeSize, 0.0f, 1.0f);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(MorphPlaneModelVertex), verts, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, a_model->IndexCount * sizeof(unsigned int), a_model->Indices, GL_STATIC_DRAW);

    a_model->MorphPlaneModel->SetIndicesCount(a_model->IndexCount);

    delete[] verts;
}

void ModelEditor::Update(double a_delta)
{
    DataStore* store = DataStore::GetInstance();

    m_window->Update();
}

ModelEditor* ModelEditor::Load(mz_zip_archive& a_archive, Workspace* a_workspace)
{
    ModelEditor* modelEditor = new ModelEditor(a_workspace);

    char* propertiesData = ExtractFileFromArchive("model.prop", a_archive);

    if (propertiesData != nullptr)
    {
        PropertyFile* propertiesFile = new PropertyFile(propertiesData);

        const std::list<PropertyFileProperty*> properties = propertiesFile->GetProperties();
        for (auto iter = properties.begin(); iter != properties.end(); ++iter)
        {
            PropertyFileProperty* prop = *iter;

            if (prop->GetParent() == nullptr)
            {
                modelEditor->GetModelData(*prop, a_archive);
            } 
        } 
    }

    return modelEditor;
}

void ModelEditor::Save(mz_zip_archive& a_archive) const
{
    if (m_models->size() > 0)
    {
        PropertyFile* propertyFile = new PropertyFile();

        for (auto iter = m_models->begin(); iter != m_models->end(); ++iter)
        {
            PropertyFileProperty* property = propertyFile->InsertProperty();

            const unsigned int indexCount = (*iter)->IndexCount;
            const unsigned int vertexCount = (*iter)->VertexCount;

            const char* name = (*iter)->ModelName->GetName();

            const e_ModelType modelType = (*iter)->BaseModel->GetModelType();

            property->SetName("model");
            property->EmplaceValue("name", name);
            property->EmplaceValue("truename", (*iter)->ModelName->GetTrueName());
            property->EmplaceValue("texname", (*iter)->TextureName);
            property->EmplaceValue("indices", std::to_string(indexCount).c_str());
            property->EmplaceValue("vertices", std::to_string(vertexCount).c_str());
            
            const unsigned int morphPlaneSize = (*iter)->MorphPlaneSize;
            const unsigned int scaledSize = morphPlaneSize + 1;
            const unsigned int trueMorphSize = scaledSize * scaledSize;

            for (auto iIter = (*iter)->MorphPlanes.begin(); iIter != (*iter)->MorphPlanes.end(); ++iIter)
            {
                PropertyFileProperty* morphProperty = propertyFile->InsertProperty();

                const char* morphName = (*iIter)->MorphPlaneName->GetName();

                morphProperty->SetName("MorphPlane");
                morphProperty->SetParent(property);
                morphProperty->EmplaceValue("name", morphName);
                morphProperty->EmplaceValue("truename", (*iIter)->MorphPlaneName->GetTrueName());
                morphProperty->EmplaceValue("size", std::to_string(morphPlaneSize).c_str());

                std::string morphPlaneFileName = "mrpPln/" + std::string(morphName) + ".mrpbin";

                mz_zip_writer_add_mem(&a_archive, morphPlaneFileName.c_str(), (*iIter)->Plane->GetMorphPositions(), trueMorphSize * sizeof(glm::vec2), MZ_DEFAULT_COMPRESSION);
            }

            const unsigned int indiciesSize = indexCount * sizeof(unsigned int);
            unsigned int verticesSize = 0;

            switch (modelType)
            {
            case e_ModelType::MorphPlane:
            {
                verticesSize = vertexCount * sizeof(MorphPlaneModelVertex);

                break;
            }
            default:
            {
                verticesSize = vertexCount * sizeof(ModelVertex);

                break;
            }
            }

            const std::string indexFileName = "mdl/" + std::string(name) + ".indbin";
            const std::string vertexFileName = "mdl/" + std::string(name) + ".verbin";

            mz_zip_writer_add_mem(&a_archive, indexFileName.c_str(), (*iter)->Indices, indiciesSize, MZ_DEFAULT_COMPRESSION);
            mz_zip_writer_add_mem(&a_archive, vertexFileName.c_str(), (*iter)->Vertices, verticesSize, MZ_DEFAULT_COMPRESSION);
        }

        if (propertyFile->PropertyCount() > 0)
        {
            char* data = propertyFile->ToString();
            mz_zip_writer_add_mem(&a_archive, "model.prop", data, strlen(data), MZ_DEFAULT_COMPRESSION);
     
            delete[] data;
        }

        delete propertyFile; 
    }
}

void ModelEditor::AddModel(const char* a_textureName, ModelVertex* a_vertices, unsigned int a_vertexCount, unsigned int* a_indices, unsigned int a_indexCount)
{
    ModelData* modelData = AddModel(a_textureName, nullptr, a_textureName, a_vertices, a_vertexCount, a_indices, a_indexCount, e_ModelType::Base);    

    if (modelData != nullptr)
    {
        m_models->emplace_back(modelData);
    }
}

void ModelEditor::DrawPropertiesWindow()
{
    m_window->UpdatePropertiesWindow(m_selectedModelData);
}
void ModelEditor::DrawEditorWindow()
{
    m_intermediateRenderer->Reset();

    m_window->UpdateEditorWindow();
}

const Texture* ModelEditor::DrawEditor()
{
    const Camera* cam = m_window->GetCamera();

    const StaticTransform* transform = cam->GetTransform();

    const glm::mat4 transformMatrix = transform->ToMatrix();
    const glm::mat4 viewMatrix = glm::inverse(transformMatrix);
    const glm::mat4 projMatrix = cam->GetProjection();

    const glm::mat4 finalTransform = projMatrix * viewMatrix;

    const e_ToolMode toolMode = m_window->GetToolMode();

    const bool alpha = m_window->GetAlphaMode();
    const bool solid = m_window->GetSolidMode();
    const bool wireframe = m_window->GetWireFrameMode();

    m_renderTexture->Bind();

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    const ImVec2 winSize = ImGui::GetWindowSize();
    const glm::vec2 scaledWinSize = { winSize.x - 20, winSize.y - 60 };

    glm::vec2 scalar = glm::vec2(1);

    if (scaledWinSize.x > scaledWinSize.y)
    {
        scalar.x = scaledWinSize.y / scaledWinSize.x;
    }
    else
    {
        scalar.y = scaledWinSize.x / scaledWinSize.y;
    }
    

    if (m_selectedMorphPlane)
    {
        const unsigned int size = m_selectedModelData->MorphPlaneSize;
        const unsigned int scaledSize = size + 1;

        glm::vec2 selectMid = glm::vec2(0);

        unsigned int indexCount = 0;
        for (unsigned int x = 0; x <= size; ++x)
        {
            for (unsigned int y = 0; y <= size; ++y)
            {
                const unsigned int index = x + y * scaledSize;

                const glm::vec2 pos = m_selectedMorphPlane->Plane->GetMorphPosition(x, y);
                const glm::vec4 fPos = finalTransform * glm::vec4(pos.x, pos.y, 0, 1);

                auto iter = std::find(m_selectedIndices.begin(), m_selectedIndices.end(), index);

                if (iter != m_selectedIndices.end())
                {
                    ++indexCount;
                    selectMid += pos;

                    m_intermediateRenderer->DrawSolidCircle({ fPos.x, fPos.y, -0.1f }, 20, 0.025f, ACTIVE_COLOR, scalar.x, scalar.y);
                }
                else
                {
                    m_intermediateRenderer->DrawCircle({ fPos.x, fPos.y, -0.1f }, 20, 0.01f, 0.01f, INACTIVE_COLOR, scalar.x, scalar.y);
                }
            }
        }

        selectMid /= indexCount;

        m_window->SetSelectionPoint(selectMid);

        if (toolMode == e_ToolMode::Move)
        {
            const glm::vec4 pos = finalTransform * glm::vec4(selectMid, 0, 1);
            const glm::vec3 pos3 = { pos.x, pos.y, -0.2f };

            const glm::vec3 up = { 0, 1, 0 };
            const glm::vec3 right = { 1, 0, 0 };

            const glm::vec3 upPos = pos3 + (up * 0.1f * scalar.y);
            const glm::vec3 rightPos = pos3 + (right * 0.1f * scalar.x);

            const e_Axis axis = m_window->GetAxis();

            switch (axis)
            {
            case e_Axis::X:
            {
                m_intermediateRenderer->DrawArrow(upPos, up, 0.05f, 0.01f, { 0, 1, 0, 1 });
                m_intermediateRenderer->DrawArrow(rightPos, right, 0.1f, 0.01f, { 0.5f, 0, 0, 1 });

                break;
            }
            case e_Axis::Y:
            {
                m_intermediateRenderer->DrawArrow(upPos, up, 0.1f, 0.01f, { 0, 0.5f, 0, 1 });
                m_intermediateRenderer->DrawArrow(rightPos, right, 0.05f, 0.01f, { 1, 0, 0, 1 });

                break;
            }
            default:
            {
                m_intermediateRenderer->DrawArrow(upPos, up, 0.05f, 0.01f, { 0, 1, 0, 1 });
                m_intermediateRenderer->DrawArrow(rightPos, right, 0.05f, 0.01f, { 1, 0, 0, 1 });

                break;
            }
            }

            m_intermediateRenderer->DrawLine(pos3, upPos, 0.01f, { 0, 1, 0, 1 });
            m_intermediateRenderer->DrawLine(pos3, rightPos, 0.01f, { 1, 0, 0, 1 });
        }

        m_morphPlaneDisplay->SetModelName(m_selectedModelData->ModelName->GetName());
        m_morphPlaneDisplay->SetMorphPlaneName(m_selectedMorphPlane->MorphPlaneName->GetName());

        m_morphPlaneDisplay->Draw(finalTransform, alpha, solid, wireframe);
    }
    else
    {
        m_imageDisplay->SetModelName(m_selectedModelData->ModelName->GetName());

        m_imageDisplay->Draw(finalTransform, alpha, solid, wireframe);
    }

    m_intermediateRenderer->Draw();
    m_renderTexture->Unbind();

    return m_renderTexture->GetTexture();
}

void ModelEditor::RenameModel(const char* a_newName)
{
    DataStore* store = DataStore::GetInstance();

    store->RemoveModelAll(m_selectedModelData->ModelName->GetName());

    if (a_newName[0] != 0)
    {
        m_selectedModelData->ModelName->SetTrueName(a_newName);
    }
    else 
    {
        m_selectedModelData->ModelName->SetTrueName("NULL");
    }

    const char* name = m_selectedModelData->ModelName->GetName();

    if (m_selectedModelData->BaseModel != nullptr)
    {
        store->AddModel(name, m_selectedModelData->BaseModel);
    }
    if (m_selectedModelData->MorphPlaneModel != nullptr)
    {
        store->AddModel(name, m_selectedModelData->MorphPlaneModel);
    }

    store->SetModelTextureName(name, m_selectedModelData->TextureName);
}
void ModelEditor::SetTextureName(const char* a_name)
{
    DataStore* store = DataStore::GetInstance();

    store->SetModelTextureName(m_selectedModelData->ModelName->GetName(), a_name);

    delete[] m_selectedModelData->TextureName;
    m_selectedModelData->TextureName = nullptr;

    if (a_name != nullptr)
    {
        const size_t len = strlen(a_name);

        m_selectedModelData->TextureName = new char[len + 1];

        strcpy(m_selectedModelData->TextureName, a_name);
    }
}

void ModelEditor::ResizeMorphPlane(int a_newSize)
{
    const std::list<MorphPlaneData*> morphPlanes = m_selectedModelData->MorphPlanes;

    DataStore* store = DataStore::GetInstance();

    m_selectedModelData->MorphPlaneSize = a_newSize;

    for (auto iter = morphPlanes.begin(); iter != morphPlanes.end(); ++iter)
    {
        const char* name = (*iter)->MorphPlaneName->GetName();

        MorphPlane* newMorphPlane = new MorphPlane(a_newSize);

        store->RemoveMorphPlane(name);

        delete (*iter)->Plane;

        (*iter)->Plane = newMorphPlane;

        store->AddMorphPlane(name, newMorphPlane);

        GenerateMorphVertexData(m_selectedModelData);
    }
}
void ModelEditor::AddMorphPlaneClicked()
{
    DataStore* store = DataStore::GetInstance();

    if (m_selectedModelData->MorphPlaneModel == nullptr)
    {
        m_selectedModelData->MorphPlaneModel = new MorphPlaneModel();

        GenerateMorphVertexData(m_selectedModelData);

        store->AddModel(m_selectedModelData->ModelName->GetName(), m_selectedModelData->MorphPlaneModel);
    }

    MorphPlaneData* morphData = new MorphPlaneData
    {
        new Name("Morph Plane", m_morphPlaneNamer),
        new MorphPlane(m_selectedModelData->MorphPlaneSize)
    };

    m_selectedModelData->MorphPlanes.emplace_back(morphData);
    store->AddMorphPlane(morphData->MorphPlaneName->GetName(), morphData->Plane);
}
bool ModelEditor::IsMorphPlaneSelected(MorphPlaneData* a_morphPlane) const
{
    return m_selectedMorphPlane == a_morphPlane;
}
void ModelEditor::MorphPlaneSelected(MorphPlaneData* a_morphPlane)
{
    m_selectedMorphPlane = a_morphPlane;

    m_window->ResetTools();

    m_selectedIndices.clear();
}

void ModelEditor::DrawSelectionBox(const glm::vec2& a_startPos, const glm::vec2& a_endPos)
{
    const Camera* camera = m_window->GetCamera();

    const StaticTransform* transform = camera->GetTransform();
    
    const glm::mat4 tranMatrix = transform->ToMatrix();
    const glm::mat4 view = glm::inverse(tranMatrix);
    const glm::mat4 proj = camera->GetProjection();

    const glm::mat4 viewProj = proj * view;

    const glm::vec4 fPosStart = viewProj * glm::vec4(a_startPos, 0, 1); 
    const glm::vec4 fPosEnd = viewProj * glm::vec4(a_endPos, 0, 1);

    m_intermediateRenderer->DrawBox({ fPosStart.x, fPosStart.y, -0.3f }, { fPosEnd.x, fPosEnd.y, -0.2f }, 0.01f, { 1, 1, 1, 1 });
}

void ModelEditor::DragValue(const glm::vec2& a_dragMov)
{
    glm::vec2* morphPositions = m_selectedMorphPlane->Plane->GetMorphPositions();

    for (auto iter = m_selectedIndices.begin(); iter != m_selectedIndices.end(); ++iter)
    {
        morphPositions[*iter] += a_dragMov;
    }
}
void ModelEditor::SelectMouseUp(const glm::vec2& a_startPos, const glm::vec2& a_endPos)
{
    if (IsMorphPlaneSelected())
    {
        const ImGuiIO io = ImGui::GetIO();

        if (!io.KeyShift && !io.KeyCtrl)
        {
            m_selectedIndices.clear();
        } 

        const unsigned int size = m_selectedMorphPlane->Plane->GetSize();
        const unsigned int scaledSize = size + 1;
        const unsigned int trueSize = size * size;

        const glm::vec2* morpPositions = m_selectedMorphPlane->Plane->GetMorphPositions();
        const glm::vec2 bMin = glm::vec2(glm::min(a_endPos.x, a_startPos.x), glm::min(a_endPos.y, a_startPos.y));
        const glm::vec2 bMax = glm::vec2(glm::max(a_endPos.x, a_startPos.x), glm::max(a_endPos.y, a_startPos.y)); 

        for (unsigned int i = 0; i < trueSize; ++i)
        {
            const glm::vec2 morphPosition = morpPositions[i];
            const glm::vec2 nearPoint = glm::vec2(glm::clamp(morphPosition.x, bMin.x, bMax.x), glm::clamp(morphPosition.y, bMin.y, bMax.y));
            const glm::vec2 diff = nearPoint - morphPosition;

            if ((diff.x == 0 && diff.y == 0) || glm::length(diff) <= 0.025f)
            {
                auto iter = std::find(m_selectedIndices.begin(), m_selectedIndices.end(), i);

                if (iter == m_selectedIndices.end())
                {
                    m_selectedIndices.emplace_back(i);
                }
                else if (io.KeyCtrl)
                {
                    m_selectedIndices.erase(iter);
                }
            } 
        }
    }
}