#include "ModelEditor.h"

#include <algorithm>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <string.h>

#include "Camera.h"
#include "ColorTheme.h"
#include "DataStore.h"
#include "FileLoaders/PropertyFile.h"
#include "FileUtils.h"
#include "imgui/imgui.h"
#include "IntermediateRenderer.h"
#include "Models/Model.h"
#include "Models/MorphPlaneModel.h"
#include "Models/MorphTargetModel.h"
#include "MorphPlane.h"
#include "Name.h"
#include "Namer.h"
#include "Renderers/ImageDisplay.h"
#include "Renderers/MorphPlaneDisplay.h"
#include "Renderers/MorphTargetDisplay.h"
#include "RenderTexture.h"
#include "StaticTransform.h"
#include "WindowControls/ModelEditorWindow.h"
#include "Workspace.h"

const static int IMAGE_SIZE = 4096;

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
    m_morphTargetDisplay = new MorphTargetDisplay();

    m_selectedModelData = nullptr;
    m_selectedMorphPlane = nullptr;
}  
ModelEditor::~ModelEditor()
{
    delete m_window;

    for (auto iter = m_models->begin(); iter != m_models->end(); ++iter)
    {
        DeleteModelData(*iter);
    }

    delete m_models;

    delete m_renderTexture;

    delete m_imageDisplay;
    delete m_morphPlaneDisplay;
    delete m_morphTargetDisplay;

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
bool ModelEditor::IsMorphTargetSelected(const glm::vec4* a_morphTarget) const
{
    return m_selectedMorphTarget == a_morphTarget;
}

e_ToolMode ModelEditor::GetToolMode() const
{
    return m_window->GetToolMode();
}

glm::vec2 ModelEditor::GetSelectionMid() const
{
    return m_selectMid;
}

int ModelEditor::MorphTargetPtrToIndex(const glm::vec4* a_ptr) const
{
    if (m_selectedModelData != nullptr)
    {   
        for (int i = 0; i < 8; ++i)
        {
            if (m_selectedModelData->MorphTargetData[i] == a_ptr)
            {
                return i;
            }
        }
    }

    return -1;
}
glm::vec2 ModelEditor::MorphTargetIndexToLerp(int a_index) const
{
    // Might be better to just have a static lookup table
    // It works but
    switch (a_index)
    {
    case 0:
    {
        return { 0, 1 };
    }
    case 1:
    {
        return { 0, -1 };
    }
    case 2:
    {
        return { 1, 0 };
    }
    case 3:
    {
        return { -1, 0 };
    }
    case 4:
    {
        return { 1, 1 };
    }
    case 5:
    {
        return { 1, -1 };
    }
    case 6:
    {
        return { -1, -1 };
    }
    case 7:
    {
        return { -1, 1 };
    }
    }

    return glm::vec2(std::numeric_limits<float>::infinity());
}

void ModelEditor::DrawModelList()
{
    ImGui::BeginChild("Layer Model Scroll");

    auto removeIter = m_models->end();

    for (auto iter = m_models->begin(); iter != m_models->end(); ++iter)
    {
        bool is_selected = (*iter == m_selectedModelData);

        if (ImGui::Selectable((*iter)->ModelName->GetName(), &is_selected))
        {
            m_selectedModelData = *iter;
            m_selectedMorphPlane = nullptr;
            m_selectedMorphTarget = nullptr;

            m_selectedIndices.clear();

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
            if (ImGui::MenuItem("Duplicate Model"))
            {
                const unsigned int vertexCount = (*iter)->VertexCount;
                const unsigned int indexCount = (*iter)->IndexCount;

                ModelVertex* vertices = new ModelVertex[vertexCount];
                unsigned int* indices = new unsigned int[indexCount];

                memcpy(vertices, (*iter)->Vertices, vertexCount * sizeof(ModelVertex));
                memcpy(indices, (*iter)->Indices, indexCount * sizeof(unsigned int));

                AddModel((*iter)->TextureName, vertices, vertexCount, indices, indexCount, (*iter)->MinConstraint, (*iter)->MaxConstraint);
            }

            ImGui::EndPopup();
        }
    }

    if (removeIter != m_models->end())
    {
        if (m_selectedModelData == *removeIter)
        {
            m_selectedModelData = nullptr;
            m_selectedMorphPlane = nullptr;
            m_selectedMorphTarget = nullptr;

            m_selectedIndices.clear();
        }

        DeleteModelData(*removeIter);

        m_models->erase(removeIter);
    }

    ImGui::EndChild();
}

void ModelEditor::DeleteModelData(ModelData* a_modelData)
{
    DataStore* store = DataStore::GetInstance();

    store->RemoveModelAll(a_modelData->ModelName->GetName());

    delete[] a_modelData->TextureName;
    delete a_modelData->ModelName;

    delete a_modelData->BaseModel;
    if (a_modelData->PlaneModel != nullptr)
    {
        delete a_modelData->PlaneModel;
    }
    if (a_modelData->TargetModel != nullptr)
    {
        delete a_modelData->TargetModel;
    }

    delete[] a_modelData->Vertices;
    delete[] a_modelData->Indices;

    for (auto iter = a_modelData->MorphPlanes.begin(); iter != a_modelData->MorphPlanes.end(); ++iter)
    {
        delete (*iter)->Plane;
        delete (*iter)->MorphPlaneName;

        delete *iter;
    }

    if (a_modelData->MorphTargetData != nullptr)
    {
        for (int i = 0; i < 8; ++i)
        {
            delete[] a_modelData->MorphTargetData[i];
        }
        delete[] a_modelData->MorphTargetData;
    }

    delete a_modelData;
}

void ModelEditor::LoadMorphTargetData(const char* a_modelName, const char* a_dirName, int a_index, ModelData* a_modelData, mz_zip_archive& a_archive) const
{
    DataStore* store = DataStore::GetInstance();

    std::string morphTargetFileName = "mrpTrg/[" + std::string(a_modelName) + "] " + a_dirName + ".trgbin";

    char* data = ExtractFileFromArchive(morphTargetFileName.c_str(), a_archive);

    const unsigned int vertexCount = a_modelData->VertexCount;

    if (data != nullptr)
    {
        if (a_modelData->TargetModel == nullptr)
        {
            a_modelData->TargetModel = new MorphTargetModel(*a_modelData->BaseModel);

            a_modelData->MorphTargetData = new glm::vec4*[8];
            
            store->AddModel(a_modelData->ModelName->GetName(), a_modelData->TargetModel);
        }

        a_modelData->MorphTargetData[a_index] = new glm::vec4[vertexCount];

        const size_t vertexSize = sizeof(glm::vec4);

        a_modelData->MorphTargetData[a_index] = (glm::vec4*)data;

        const unsigned int vbo = a_modelData->TargetModel->GetMorphVBO(a_index);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertexCount * vertexSize, a_modelData->MorphTargetData[a_index], GL_STATIC_DRAW);

    }
    else
    {
        if (a_modelData->TargetModel != nullptr)
        {
            if (a_modelData->MorphTargetData == nullptr)
            {
                a_modelData->MorphTargetData = new glm::vec4*[8];
            }

            a_modelData->MorphTargetData[a_index] = new glm::vec4[vertexCount];
        }
    }
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

    glm::vec2 min = glm::vec2(0);
    glm::vec2 max = glm::vec2(1);

    const std::list<PropertyFileValue> values = a_property.Values();
    for (auto iter = values.begin(); iter != values.end(); ++iter)
    {
        IFSETTOATTVALCPY("name", iter->Name, name, iter->Value)
        else IFSETTOATTVALCPY("truename", iter->Name, trueName, iter->Value)
        else IFSETTOATTVALCPY("texname", iter->Name, texName, iter->Value)
        else IFSETTOATTVALI("vertices", iter->Name, vertexCount, iter->Value)
        else IFSETTOATTVALI("indices", iter->Name, indexCount, iter->Value)
        else IFSETTOATTVALV2("min", iter->Name, min, iter->Value)
        else IFSETTOATTVALV2("max", iter->Name, max, iter->Value)
    }

    if (name != nullptr && trueName != nullptr && vertexCount > 0 && indexCount > 0)
    {
        std::string vertexFileName = "mdl/" + std::string(name) + ".verbin";
        std::string indexFileName = "mdl/" + std::string(name) + ".indbin";

        unsigned int* indicies = (unsigned int*)ExtractFileFromArchive(indexFileName.c_str(), a_archive);
        ModelVertex* vertices = (ModelVertex*)ExtractFileFromArchive(vertexFileName.c_str(), a_archive);

        ModelData* modelData = AddModel(texName, name, trueName, vertices, vertexCount, indicies, indexCount, (e_ModelType)modelType, min, max);

        LoadMorphTargetData(name, "North", 0, modelData, a_archive);
        LoadMorphTargetData(name, "South", 1, modelData, a_archive);
        LoadMorphTargetData(name, "East", 2, modelData, a_archive);
        LoadMorphTargetData(name, "West", 3, modelData, a_archive);

        LoadMorphTargetData(name, "North East", 4, modelData, a_archive);
        LoadMorphTargetData(name, "South East", 5, modelData, a_archive);
        LoadMorphTargetData(name, "South West", 6, modelData, a_archive);
        LoadMorphTargetData(name, "North West", 7, modelData, a_archive);

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
                    if (modelData->PlaneModel == nullptr)
                    {
                        modelData->PlaneModel = new MorphPlaneModel(*modelData->BaseModel);

                        modelData->MorphPlaneSize = valSize;
                        
                        GenerateMorphVertexData(modelData);

                        dataStore->AddModel(modelData->ModelName->GetName(), modelData->PlaneModel);
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

ModelData* ModelEditor::AddModel(const char* a_textureName, const char* a_name, const char* a_trueName, ModelVertex* a_vertices, unsigned int a_vertexCount, unsigned int* a_indices, unsigned int a_indexCount, e_ModelType a_modelType, const glm::vec2& a_min, const glm::vec2& a_max) const
{
    if (a_vertexCount <= 0 || a_indexCount <= 0 || a_trueName == nullptr)
    {
        return nullptr;
    }

    DataStore* dataStore = DataStore::GetInstance();

    ModelData* modelData = new ModelData();

    modelData->BaseModel = nullptr;
    modelData->PlaneModel = nullptr;
    modelData->TargetModel = nullptr;

    if (a_textureName != nullptr)
    {
        const size_t strLen = strlen(a_textureName);

        modelData->TextureName = new char[strLen + 1];
        strcpy(modelData->TextureName, a_textureName);
    }
    else
    {
        modelData->TextureName = nullptr;
    }
    
    size_t vertexSize = sizeof(ModelVertex);

    Model* model;

    switch (a_modelType)
    {
    default:
    {
        model = modelData->BaseModel = new Model();            

        break;
    }
    }
    
    modelData->ModelName = new Name(a_trueName, m_namer);
    modelData->VertexCount = a_vertexCount;
    modelData->Vertices = a_vertices;
    modelData->IndexCount = a_indexCount;
    modelData->Indices = a_indices;
    modelData->MorphPlaneSize = 10;
    modelData->MorphTargetData = nullptr;
    modelData->MinConstraint = a_min;
    modelData->MaxConstraint = a_max;

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
    model->SetVertexCount(a_vertexCount);

    const char* name = modelData->ModelName->GetName();

    dataStore->AddModel(name, model);
    dataStore->SetModelTextureName(name, a_textureName);

    return modelData;
}

void ModelEditor::GenerateMorphVertexData(ModelData* a_model) const
{
    const int vbo = a_model->PlaneModel->GetMorphVBO();
    
    const unsigned int vertexCount = a_model->VertexCount;

    MorphPlaneModelVertex* verts = new MorphPlaneModelVertex[vertexCount];

    const ModelVertex* modelVerts = a_model->Vertices;

    const unsigned int planeSize = a_model->MorphPlaneSize;
    const unsigned int morphPlaneSize = planeSize + 1;
    const glm::vec2 planeScale = a_model->MaxConstraint - a_model->MinConstraint;
    const double scale = 1.0 / planeSize; 

    for (unsigned int i = 0; i < vertexCount; ++i)
    {
        const glm::vec4 pos = modelVerts[i].Position;

        const glm::vec2 pos2 = glm::vec2(pos.x, pos.y) - a_model->MinConstraint;
        const glm::vec<2, double> posScale = glm::vec<2, double>((pos2.x * (double)planeSize) / planeScale.x, (pos2.y * (double)planeSize) / planeScale.y);

        const glm::vec<2, double> min = glm::vec<2, double>(glm::floor(posScale.x), glm::floor(posScale.y));
        const glm::vec<2, double> max = glm::vec2(min.x + 1, min.y + 1);

        glm::vec<2, double> diff = min - posScale;
        double len = glm::length(diff);
        verts[i].MorphPlaneWeights[0].r = min.x + min.y * morphPlaneSize;
        verts[i].MorphPlaneWeights[0].g = (float)glm::clamp((1 - len), 0.0, 1.0);

        diff = glm::vec<2, double>(min.x, max.y) - posScale;
        len = glm::length(diff);
        verts[i].MorphPlaneWeights[1].r = min.x + max.y * morphPlaneSize;
        verts[i].MorphPlaneWeights[1].g = (float)glm::clamp((1 - len), 0.0, 1.0);
        
        diff = glm::vec<2, double>(max.x, min.y) - posScale;
        len = glm::length(diff);
        verts[i].MorphPlaneWeights[2].r = max.x + min.y * morphPlaneSize;
        verts[i].MorphPlaneWeights[2].g = (float)glm::clamp((1 - len), 0.0, 1.0);

        diff = max - posScale;
        len = glm::length(diff);
        verts[i].MorphPlaneWeights[3].r = max.x + max.y * morphPlaneSize;
        verts[i].MorphPlaneWeights[3].g = (float)glm::clamp((1 - len), 0.0, 1.0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(MorphPlaneModelVertex), verts, GL_STATIC_DRAW);

    delete[] verts;
}

void ModelEditor::TransformArrows(const glm::vec2& a_pos, const glm::mat4& a_transform, e_ToolMode a_toolMode, const glm::vec2& a_scalar) const
{
    m_window->SetSelectionPoint(a_pos);

    if (a_toolMode == e_ToolMode::Move)
    {
        const glm::vec4 pos = a_transform * glm::vec4(a_pos, 0, 1);
        const glm::vec3 pos3 = { pos.x, pos.y, -0.2f };

        const glm::vec3 up = { 0, 1, 0 };
        const glm::vec3 right = { 1, 0, 0 };

        const glm::vec3 upPos = pos3 + (up * 0.1f * a_scalar.y);
        const glm::vec3 rightPos = pos3 + (right * 0.1f * a_scalar.x);

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
void ModelEditor::SaveMorphTargetData(const char* a_modelName, const char* a_dirName, unsigned int a_vertexCount, const glm::vec4* a_data, mz_zip_archive& a_archive) const
{
    std::string morphTargetFileName = "mrpTrg/[" + std::string(a_modelName) + "] " + a_dirName + ".trgbin";

    mz_zip_writer_add_mem(&a_archive, morphTargetFileName.c_str(), a_data, a_vertexCount * sizeof(glm::vec4), MZ_DEFAULT_COMPRESSION);
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
            property->EmplaceValue("min", ("{ " + std::to_string((*iter)->MinConstraint.x) + ", " + std::to_string((*iter)->MinConstraint.y) + " }").c_str());
            property->EmplaceValue("max", ("{ " + std::to_string((*iter)->MaxConstraint.x) + ", " + std::to_string((*iter)->MaxConstraint.y) + " }").c_str());
            
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

            if ((*iter)->TargetModel != nullptr)
            {
                SaveMorphTargetData(name, "North", vertexCount, (*iter)->MorphTargetData[0], a_archive);
                SaveMorphTargetData(name, "South", vertexCount, (*iter)->MorphTargetData[1], a_archive);
                SaveMorphTargetData(name, "East", vertexCount, (*iter)->MorphTargetData[2], a_archive);
                SaveMorphTargetData(name, "West", vertexCount, (*iter)->MorphTargetData[3], a_archive);

                SaveMorphTargetData(name, "North East", vertexCount, (*iter)->MorphTargetData[4], a_archive);
                SaveMorphTargetData(name, "South East", vertexCount, (*iter)->MorphTargetData[5], a_archive);
                SaveMorphTargetData(name, "South West", vertexCount, (*iter)->MorphTargetData[6], a_archive);
                SaveMorphTargetData(name, "North West", vertexCount, (*iter)->MorphTargetData[7], a_archive);
            }

            const unsigned int indiciesSize = indexCount * sizeof(unsigned int);
            unsigned int verticesSize = vertexCount * sizeof(ModelVertex);

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

void ModelEditor::AddModel(const char* a_textureName, ModelVertex* a_vertices, unsigned int a_vertexCount, unsigned int* a_indices, unsigned int a_indexCount, const glm::vec2& a_min, const glm::vec2& a_max)
{
    ModelData* modelData = AddModel(a_textureName, nullptr, a_textureName, a_vertices, a_vertexCount, a_indices, a_indexCount, e_ModelType::Base, a_min, a_max);    

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

void ModelEditor::DrawIMRenderer()
{
    m_intermediateRenderer->Draw();
}
void ModelEditor::ResetIMRenderer()
{
    m_intermediateRenderer->Reset();
}

void ModelEditor::ToolsDisplayOverride(bool a_state)
{
    m_window->ToolDisplayOverride(a_state);
}

void ModelEditor::DrawMorphPlaneEditor(const glm::mat4& a_transformMatrix, const char* a_modelName, const char* a_morphPlaneName, float a_z, const glm::vec2& a_scalar, bool a_alpha, bool a_solid, bool a_wireframe)
{
    DataStore* store = DataStore::GetInstance();

    MorphPlane* morphPlane = store->GetMorphPlane(a_morphPlaneName);
    if (morphPlane == nullptr)
    {
        return;
    }

    const unsigned int size = morphPlane->GetSize();

    glm::vec2 selectMid = glm::vec2(0);

    unsigned int indexCount = 0;
    for (unsigned int x = 0; x <= size; ++x)
    {
        for (unsigned int y = 0; y <= size; ++y)
        {
            const unsigned int index = x + y * size;

            const glm::vec2 pos = morphPlane->GetMorphPosition(x, y);
            const glm::vec4 fPos = a_transformMatrix * glm::vec4(pos.x, pos.y, a_z, 1);

            auto iter = std::find(m_selectedIndices.begin(), m_selectedIndices.end(), index);

            if (iter != m_selectedIndices.end())
            {
                ++indexCount;
                selectMid += pos;

                m_intermediateRenderer->DrawSolidCircle({ fPos.x, fPos.y, a_z - 0.1f }, 20, 0.025f, ACTIVE_COLOR, a_scalar.x, a_scalar.y);
            }
            else
            {
                m_intermediateRenderer->DrawCircle({ fPos.x, fPos.y, -0.1f }, 20, 0.01f, 0.01f, INACTIVE_COLOR, a_scalar.x, a_scalar.y);
            }
        }
    }

    selectMid /= indexCount;

    const e_ToolMode toolMode = m_window->GetToolMode();

    TransformArrows(selectMid, a_transformMatrix, toolMode, a_scalar);

    m_morphPlaneDisplay->SetModelName(a_modelName);
    m_morphPlaneDisplay->SetMorphPlaneName(a_morphPlaneName);

    m_morphPlaneDisplay->Draw(a_transformMatrix, a_alpha, a_solid, a_wireframe);
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

    glEnable(GL_DEPTH_TEST);

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
    
    if (m_selectedMorphPlane != nullptr)
    {
        DrawMorphPlaneEditor(finalTransform, m_selectedModelData->ModelName->GetName(), m_selectedMorphPlane->MorphPlaneName->GetName(), 0, scalar, alpha, solid, wireframe);
    }
    else if (m_selectedMorphTarget != nullptr)
    {
        m_selectMid = glm::vec2(0);

        unsigned int indexCount = 0;
        for (auto iter = m_selectedIndices.begin(); iter != m_selectedIndices.end(); ++iter)
        {
            const glm::vec4 pos = m_selectedMorphTarget[*iter];
            const glm::vec4 fPos = finalTransform * pos;

            m_selectMid += glm::vec2(pos.x, pos.y);

            m_intermediateRenderer->DrawSolidCircle({ fPos.x, fPos.y, -0.1f }, 20, 0.025f, ACTIVE_COLOR, scalar.x, scalar.y);

            ++indexCount;
        }

        m_selectMid /= indexCount;

        TransformArrows(m_selectMid, finalTransform, toolMode, scalar);

        m_morphTargetDisplay->SetModelName(m_selectedModelData->ModelName->GetName());

        m_morphTargetDisplay->Draw9Point(finalTransform, MorphTargetIndexToLerp(MorphTargetPtrToIndex(m_selectedMorphTarget)), alpha, solid, wireframe);
    }
    else if (m_selectedModelData != nullptr)
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
    if (m_selectedModelData->PlaneModel != nullptr)
    {
        store->AddModel(name, m_selectedModelData->PlaneModel);
    }

    store->SetModelTextureName(name, m_selectedModelData->TextureName);
}
void ModelEditor::SetTextureName(const char* a_name)
{
    DataStore* store = DataStore::GetInstance();

    store->SetModelTextureName(m_selectedModelData->ModelName->GetName(), a_name);

    if (m_selectedModelData->TextureName != nullptr)
    {
        delete[] m_selectedModelData->TextureName;
        m_selectedModelData->TextureName = nullptr;
    }
    
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

        MorphPlane* newMorphPlane = new MorphPlane(m_selectedModelData->MinConstraint, m_selectedModelData->MaxConstraint, a_newSize);

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

    if (m_selectedModelData->PlaneModel == nullptr)
    {
        m_selectedModelData->PlaneModel = new MorphPlaneModel(*m_selectedModelData->BaseModel);

        GenerateMorphVertexData(m_selectedModelData);

        store->AddModel(m_selectedModelData->ModelName->GetName(), m_selectedModelData->PlaneModel);
    }

    MorphPlaneData* morphData = new MorphPlaneData
    {
        new Name("Morph Plane", m_morphPlaneNamer),
        new MorphPlane(m_selectedModelData->MinConstraint, m_selectedModelData->MaxConstraint, m_selectedModelData->MorphPlaneSize)
    };

    m_selectedModelData->MorphPlanes.emplace_back(morphData);
    store->AddMorphPlane(morphData->MorphPlaneName->GetName(), morphData->Plane);
}
void ModelEditor::RemoveMorphPlane(MorphPlaneData* a_morphPlane)
{
    DataStore* store = DataStore::GetInstance();

    if (a_morphPlane == m_selectedMorphPlane)
    {
        m_selectedMorphPlane = nullptr;
    }

    store->RemoveMorphPlane(a_morphPlane->MorphPlaneName->GetName());

    for (auto iter = m_selectedModelData->MorphPlanes.begin(); iter != m_selectedModelData->MorphPlanes.end(); ++iter)
    {
        if (*iter == a_morphPlane)
        {
            m_selectedModelData->MorphPlanes.erase(iter);

            break;
        }
    }

    if (m_selectedModelData->MorphPlanes.size() <= 0)
    {
        store->RemoveModel(m_selectedModelData->ModelName->GetName(), e_ModelType::MorphPlane);

        delete m_selectedModelData->PlaneModel;
        m_selectedModelData->PlaneModel = nullptr;
    }

    delete a_morphPlane->Plane;
    delete a_morphPlane->MorphPlaneName;
}
bool ModelEditor::IsMorphPlaneSelected(const MorphPlaneData* a_morphPlane) const
{
    return m_selectedMorphPlane == a_morphPlane;
}
void ModelEditor::MorphPlaneSelected(MorphPlaneData* a_morphPlane)
{
    m_selectedMorphPlane = a_morphPlane;
    m_selectedMorphTarget = nullptr;

    m_window->ResetTools();

    m_selectedIndices.clear();
}

void ModelEditor::AddMorphTargetsClicked()
{
    DataStore* store = DataStore::GetInstance();

    if (m_selectedModelData->TargetModel == nullptr)
    {
        m_selectedModelData->TargetModel = new MorphTargetModel(*m_selectedModelData->BaseModel);

        m_selectedModelData->MorphTargetData = new glm::vec4*[8];

        const unsigned int vertexCount = m_selectedModelData->VertexCount;
        const ModelVertex* modelVertices = m_selectedModelData->Vertices;

        const size_t vertexSize = sizeof(glm::vec4);

        for (int i = 0; i < 8; ++i)
        {
            m_selectedModelData->MorphTargetData[i] = new glm::vec4[vertexCount];

            for (unsigned int j = 0; j < vertexCount; ++j)
            {
                m_selectedModelData->MorphTargetData[i][j] = modelVertices[j].Position;
            }

            const unsigned int vbo = m_selectedModelData->TargetModel->GetMorphVBO(i);

            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, vertexCount * vertexSize, m_selectedModelData->MorphTargetData[i], GL_STATIC_DRAW);
        }

        store->AddModel(m_selectedModelData->ModelName->GetName(), m_selectedModelData->TargetModel);
    }
}
void ModelEditor::RemoveMorphTargets()
{
    DataStore* store = DataStore::GetInstance();

    if (m_selectedModelData->TargetModel != nullptr)
    {
        store->RemoveModel(m_selectedModelData->ModelName->GetName(), e_ModelType::MorphTarget);

        delete m_selectedModelData->TargetModel;
        m_selectedModelData->TargetModel = nullptr;

        if (m_selectedModelData->MorphTargetData != nullptr)
        {
            for (int i = 0; i < 8; ++i)
            {
                delete[] m_selectedModelData->MorphTargetData[i];
            }

            delete[] m_selectedModelData->MorphTargetData;
            m_selectedModelData->MorphTargetData = nullptr;
        }
    }

    m_selectedMorphTarget = nullptr;
}
void ModelEditor::MorphTargetSelected(glm::vec4* a_morphTarget)
{
    m_selectedMorphPlane = nullptr;
    m_selectedMorphTarget = a_morphTarget;

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

    DrawSelectionBox(viewProj, a_startPos, a_endPos);
}
void ModelEditor::DrawSelectionBox(const glm::mat4& a_transform, const glm::vec2& a_startPos, const glm::vec2& a_endPos)
{
    const glm::vec4 fPosStart = a_transform * glm::vec4(a_startPos, 0, 1); 
    const glm::vec4 fPosEnd = a_transform * glm::vec4(a_endPos, 0, 1);

    m_intermediateRenderer->DrawBox({ fPosStart.x, fPosStart.y, -0.3f }, { fPosEnd.x, fPosEnd.y, -0.3f }, 0.01f, { 1, 1, 1, 1 });
}

void ModelEditor::DragValue(const glm::vec2& a_dragMov, MorphPlane* a_morphPlane)
{
    if (m_selectedMorphPlane != nullptr)
    {
        glm::vec2* morphPositions = a_morphPlane->GetMorphPositions();

        for (auto iter = m_selectedIndices.begin(); iter != m_selectedIndices.end(); ++iter)
        {
            morphPositions[*iter] += a_dragMov;
        }
    }
}
void ModelEditor::SelectMouseUp(const glm::vec2& a_startPos, const glm::vec2& a_endPos, float a_scalar, MorphPlane* a_morphPlane)
{
    // Needs to be reference otherwise I get exceptions from ImGui
    const ImGuiIO& io = ImGui::GetIO();

    const glm::vec2 bMin = glm::vec2(glm::min(a_endPos.x, a_startPos.x), glm::min(a_endPos.y, a_startPos.y));
    const glm::vec2 bMax = glm::vec2(glm::max(a_endPos.x, a_startPos.x), glm::max(a_endPos.y, a_startPos.y)); 

    if (!io.KeyShift && !io.KeyCtrl)
    {
        m_selectedIndices.clear();
    } 

    const unsigned int size = a_morphPlane->GetSize();
    const unsigned int trueSize = size * size;

    const glm::vec2* morpPositions = a_morphPlane->GetMorphPositions();

    for (unsigned int i = 0; i < trueSize; ++i)
    {
        const glm::vec2 morphPosition = morpPositions[i];
        const glm::vec2 nearPoint = glm::vec2(glm::clamp(morphPosition.x, bMin.x, bMax.x), glm::clamp(morphPosition.y, bMin.y, bMax.y));
        const glm::vec2 diff = nearPoint - morphPosition;

        if ((diff.x == 0 && diff.y == 0) || glm::length(diff) <= a_scalar * 0.01f)
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

void ModelEditor::DragValue(const glm::vec2& a_dragMov)
{
    if (m_selectedMorphPlane != nullptr)
    {
        DragValue(a_dragMov, m_selectedMorphPlane->Plane);
    }
    else if (m_selectedMorphTarget != nullptr)
    {
        const glm::vec4 mov = glm::vec4(a_dragMov, 0, 0);

        for (auto iter = m_selectedIndices.begin(); iter != m_selectedIndices.end(); ++iter)
        {
            m_selectedMorphTarget[*iter] += mov;
        }

        int index = MorphTargetPtrToIndex(m_selectedMorphTarget);

        const unsigned int vbo = m_selectedModelData->TargetModel->GetMorphVBO(index);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, m_selectedModelData->VertexCount * sizeof(glm::vec4), m_selectedMorphTarget, GL_STATIC_DRAW);
    }
}
void ModelEditor::SelectMouseUp(const glm::vec2& a_startPos, const glm::vec2& a_endPos)
{
    // Needs to be reference otherwise I get exceptions from ImGui
    const ImGuiIO& io = ImGui::GetIO();

    const glm::vec2 bMin = glm::vec2(glm::min(a_endPos.x, a_startPos.x), glm::min(a_endPos.y, a_startPos.y));
    const glm::vec2 bMax = glm::vec2(glm::max(a_endPos.x, a_startPos.x), glm::max(a_endPos.y, a_startPos.y)); 

    const float zoom = m_window->GetZoom();
    const float maxZoom = m_window->GetMaxZoom();

    const float scalar = zoom / maxZoom;

    if (m_selectedMorphPlane != nullptr)
    {
        SelectMouseUp(a_startPos, a_endPos, scalar, m_selectedMorphPlane->Plane);
    }
    else if (m_selectedMorphTarget != nullptr)
    {
        if (!io.KeyShift && !io.KeyCtrl)
        {
            m_selectedIndices.clear();
        } 

        const unsigned int vertexCount = m_selectedModelData->VertexCount;

        for (unsigned int i = 0; i < vertexCount; ++i)
        {
            const glm::vec2 morphPosition = m_selectedMorphTarget[i];
            const glm::vec2 nearPoint = glm::vec2(glm::clamp(morphPosition.x, bMin.x, bMax.x), glm::clamp(morphPosition.y, bMin.y, bMax.y));
            const glm::vec2 diff = nearPoint - morphPosition;

            if ((diff.x == 0 && diff.y == 0) || glm::length(diff) <= scalar * 0.01f)
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