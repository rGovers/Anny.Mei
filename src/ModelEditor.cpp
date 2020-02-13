#include "ModelEditor.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <string.h>

#include "Camera.h"
#include "DataStore.h"
#include "FileUtils.h"
#include "imgui/imgui.h"
#include "Models/Model.h"
#include "Models/MorphPlaneModel.h"
#include "MorphPlane.h"
#include "Name.h"
#include "Namer.h"
#include "PropertyFile.h"
#include "RenderTexture.h"
#include "ShaderProgram.h"
#include "Shaders/ModelVertex.h"
#include "Shaders/SolidPixel.h"
#include "Shaders/StandardPixel.h"
#include "Texture.h"
#include "Transform.h"

const static int BUFFER_SIZE = 1024;

const static float MOUSE_SENSITIVITY = 0.001f;
const static float MOUSE_WHEEL_SENSITIVITY = 0.1f;
const static int IMAGE_SIZE = 2048;
const static float MAX_ZOOM = 2.5f;

ModelEditor::ModelEditor()
{
    m_models = new std::list<ModelData*>();

    m_namer = new Namer();
    m_morphPlaneNamer = new Namer();

    m_renderTexture = new RenderTexture(IMAGE_SIZE, IMAGE_SIZE, GL_RGBA);

    m_selectedModelData = nullptr;

    m_solid = true;
    m_wireframe = false;
    m_alpha = false;

    m_zoom = 1.0f;
    m_translation = glm::vec3(0);

    const unsigned int pixelShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(pixelShader, 1, &STANDARDPIXEL, 0);
    glCompileShader(pixelShader);
        
    const unsigned int solidPixelShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(solidPixelShader, 1, &SOLIDPIXEL, 0);
    glCompileShader(solidPixelShader);

    const unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &MODELVERTEX, 0);
    glCompileShader(vertexShader);

    m_baseShaderProgram = new ShaderProgram(pixelShader, vertexShader);
    m_wireShaderProgram = new ShaderProgram(solidPixelShader, vertexShader);

    glDeleteShader(pixelShader);
    glDeleteShader(solidPixelShader);
    glDeleteShader(vertexShader);
}  
ModelEditor::~ModelEditor()
{
    for (auto iter = m_models->begin(); iter != m_models->end(); ++iter)
    {
        delete[] (*iter)->TextureName;
        delete (*iter)->ModelName;
        delete (*iter)->BaseModel;

        delete *iter;
    }

    delete m_models;

    delete m_renderTexture;

    delete m_baseShaderProgram;
    delete m_wireShaderProgram;

    delete m_namer;
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

    for (auto iter = a_property.Values().begin(); iter != a_property.Values().end(); ++iter)
    {
        IFSETTOATTVALCPY("name", iter->Name, name, iter->Value)
        else IFSETTOATTVALCPY("truename", iter->Name, trueName, iter->Value)
        else IFSETTOATTVALCPY("texname", iter->Name, texName, iter->Value)
        else IFSETTOATTVALI("vertices", iter->Name, vertexCount, iter->Value)
        else IFSETTOATTVALI("indices", iter->Name, indexCount, iter->Value)
        else IFSETTOATTVALI("modeltype", iter->Name, modelType, iter->Value)
    }

    if (name != nullptr && trueName != nullptr && vertexCount > 0 && indexCount > 0)
    {
        std::string vertexFileName = "mdl/" + std::string(name) + ".verbin";
        std::string indexFileName = "mdl/" + std::string(name) + ".indbin";

        unsigned int* indicies = (unsigned int*)ExtractFileFromArchive(indexFileName.c_str(), a_archive);
        ModelVertex* vertices = (ModelVertex*)ExtractFileFromArchive(vertexFileName.c_str(), a_archive);

        ModelData* modelData = AddModel(texName, name, trueName, vertices, vertexCount, indicies, indexCount, (e_ModelType)modelType);

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

ModelEditor::ModelData* ModelEditor::AddModel(const char* a_textureName, const char* a_name, const char* a_trueName, ModelVertex* a_vertices, unsigned int a_vertexCount, unsigned int* a_indices, unsigned int a_indexCount, e_ModelType a_modelType) const
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

    model->SetIndiciesCount(a_indexCount);

    const char* name = modelData->ModelName->GetName();

    dataStore->AddModel(name, model);
    dataStore->SetModelTextureName(name, a_textureName);

    return modelData;
}

void ModelEditor::GenerateMorphVertexData() const
{
    const int ibo = m_selectedModelData->MorphPlaneModel->GetIBO();
    const int vbo = m_selectedModelData->MorphPlaneModel->GetVBO();
    
    const unsigned int vertexCount = m_selectedModelData->VertexCount;

    MorphPlaneModelVertex* verts = new MorphPlaneModelVertex[vertexCount];

    const ModelVertex* modelVerts = m_selectedModelData->Vertices;

    const float scale = 1.0f / m_selectedModelData->MorphPlaneSize; 

    for (unsigned int i = 0; i < vertexCount; ++i)
    {
        const glm::vec4 pos = modelVerts[i].Position;

        verts[i].Position = pos;
        verts[i].TexCoord = modelVerts[i].TexCoord;

        const glm::vec2 pos2 = { pos.x, pos.y };
        const glm::vec2 posScale = pos2 * (float)m_selectedModelData->MorphPlaneSize;

        const glm::vec2 min = { glm::floor(posScale.x), glm::floor(posScale.y) };
        const glm::vec2 max = { glm::ceil(posScale.x), glm::ceil(posScale.y) };

        verts[i].MorphPlaneWeights[0].r = min.x + min.y * m_selectedModelData->MorphPlaneSize;
        verts[i].MorphPlaneWeights[0].g = glm::max(1 - glm::length(min * scale - pos2) / scale, 0.0f);

        verts[i].MorphPlaneWeights[1].r = min.x + max.y * m_selectedModelData->MorphPlaneSize;
        verts[i].MorphPlaneWeights[1].g = glm::max(1 - glm::length(glm::vec2(min.x, max.y) * scale - pos2) / scale, 0.0f);

        verts[i].MorphPlaneWeights[2].r = max.x + min.y * m_selectedModelData->MorphPlaneSize;
        verts[i].MorphPlaneWeights[2].g = glm::max(1 - glm::length(glm::vec2(max.x, min.y) * scale - pos2) / scale, 0.0f);

        verts[i].MorphPlaneWeights[3].r = max.x + max.y * m_selectedModelData->MorphPlaneSize;
        verts[i].MorphPlaneWeights[3].g = glm::max(1 - glm::length(max * scale - pos2) / scale, 0.0f);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(MorphPlaneModelVertex), verts, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_selectedModelData->IndexCount * sizeof(unsigned int), m_selectedModelData->Indices, GL_STATIC_DRAW);

    delete[] verts;
}

void ModelEditor::Update(double a_delta)
{
    DataStore* store = DataStore::GetInstance();

    ImGui::SetNextWindowSize({ 250, 600 }, ImGuiCond_Appearing);
    if (ImGui::Begin("Model List"))
    {
        ImGui::BeginChild("Layer Model Scroll");

        auto removeIter = m_models->end();

        for (auto iter = m_models->begin(); iter != m_models->end(); ++iter)
        {
            if (ImGui::Selectable((*iter)->ModelName->GetName()))
            {
                m_selectedModelData = *iter;
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
    ImGui::End();

    if (m_selectedModelData != nullptr)
    {
        ImGui::SetNextWindowSize({ 200, 400 }, ImGuiCond_Appearing);
        if (ImGui::Begin("Model Properties"))
        {
            const char* name = m_selectedModelData->ModelName->GetTrueName();

            size_t len = strlen(name);

            char* buff = new char[len + 1];
            strcpy(buff, name);

            ImGui::InputText("Model Name", buff, BUFFER_SIZE);

            if (strcmp(name, buff) != 0)
            {
                store->RemoveModelAll(m_selectedModelData->ModelName->GetName());

                m_selectedModelData->ModelName->SetTrueName(buff);

                name = m_selectedModelData->ModelName->GetName();

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

            delete[] buff;

            name = m_selectedModelData->TextureName;

            len = strlen(name);

            buff = new char[len + 1];
            strcpy(buff, name);

            ImGui::InputText("Texture Name", buff, BUFFER_SIZE);

            if (strcmp(name, buff) != 0)
            {
                store->SetModelTextureName(m_selectedModelData->ModelName->GetName(), buff);

                delete[] m_selectedModelData->TextureName;
                m_selectedModelData->TextureName = buff;
            }
            else
            {
                delete[] buff;
            }

            if (ImGui::TreeNode("Morph Planes"))
            {
                const std::list<MorphPlaneData*> morphPlanes = m_selectedModelData->MorphPlanes;

                int newSize = m_selectedModelData->MorphPlaneSize;

                ImGui::InputInt("Morph Plane Size", &newSize);

                newSize = glm::clamp(newSize, 1, 100);
                if (m_selectedModelData->MorphPlaneSize != newSize)
                {
                    for (auto iter = morphPlanes.begin(); iter != morphPlanes.end(); ++iter)
                    {
                        (*iter)->Plane->Resize(newSize);

                        GenerateMorphVertexData();
                    }

                    m_selectedModelData->MorphPlaneSize = newSize;
                }

                if (ImGui::Button("Add Morph Plane"))
                {
                    if (m_selectedModelData->MorphPlaneModel == nullptr)
                    {
                        m_selectedModelData->MorphPlaneModel = new MorphPlaneModel();

                        GenerateMorphVertexData();

                        store->AddModel(m_selectedModelData->ModelName->GetName(), m_selectedModelData->MorphPlaneModel);
                    }

                    MorphPlaneData* morphData = new MorphPlaneData
                    {
                        new Name("Morph Plane", m_morphPlaneNamer),
                        new MorphPlane(m_selectedModelData->MorphPlaneSize)
                    };

                    m_selectedModelData->MorphPlanes.emplace_back(morphData);
                }

                if (m_selectedModelData->MorphPlanes.size() > 0)
                {
                    ImGui::Separator();

                    for (auto iter = morphPlanes.begin(); iter != morphPlanes.end(); ++iter)
                    {
                        if (ImGui::Selectable((*iter)->MorphPlaneName->GetName()))
                        {
                            
                        }
                    }
                }

                ImGui::TreePop();
            }
        }
        ImGui::End();

        ImGui::SetNextWindowSize({ 420, 440 }, ImGuiCond_Appearing);
        if (ImGui::Begin("Model Editor", nullptr, ImGuiWindowFlags_MenuBar))
        {
            if (ImGui::IsWindowFocused())
            {
                if (ImGui::IsMouseDown(2))
                {
                    const ImVec2 tPos = ImGui::GetMousePos();
                    const glm::vec2 mousePos = { tPos.x, tPos.y };

                    if (m_lastMousePos.x >= 0 && m_lastMousePos.y >= 0)
                    {
                        glm::vec2 mov = m_lastMousePos - mousePos;

                        m_translation += glm::vec3(mov.x, mov.y, 0.0f) * MOUSE_SENSITIVITY;
                    }

                    m_lastMousePos = mousePos;
                }
                else
                {
                    m_lastMousePos = glm::vec2(-1);
                }

                const float mouseWheel = ImGui::GetIO().MouseWheel;

                const float wheelDelta = mouseWheel * MOUSE_WHEEL_SENSITIVITY;

                m_zoom = glm::clamp(m_zoom - wheelDelta, 0.01f, MAX_ZOOM);

                m_translation.x = glm::clamp(m_translation.x, -1.0f, 1.0f);
                m_translation.y = glm::clamp(m_translation.y, -1.0f, 1.0f);
            }

            const ImVec2 size = ImGui::GetWindowSize();
            const glm::vec2 trueSize = { (size.x - 20) / IMAGE_SIZE, (size.y - 60) / IMAGE_SIZE };

            const glm::mat4 proj = glm::orthoRH(0.0f, trueSize.x * m_zoom * 5, 0.0f, trueSize.y * m_zoom * 5, -1.0f, 1.0f);
            const glm::mat4 view = glm::inverse(glm::translate(glm::mat4(1), m_translation));

            const glm::mat4 finalTransform = proj * view;

            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("View"))
                {
                    ImGui::MenuItem("Solid", nullptr, &m_solid);
                    ImGui::MenuItem("Wireframe", nullptr, &m_wireframe);
                    ImGui::MenuItem("Alpha", nullptr, &m_alpha);
        
                    ImGui::EndMenu();
                }
        
                ImGui::EndMenuBar();
            }

            m_renderTexture->Bind();

            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

            Texture* tex = nullptr;

            const char* texName = m_selectedModelData->TextureName;

            if (m_alpha)
            {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);     
            }

            if (texName != nullptr)
            {
                tex = store->GetTexture(texName);
            }

            if (tex != nullptr)
            {
                glBindVertexArray(m_selectedModelData->BaseModel->GetVAO());

                if (m_wireframe)
                {
                    const int wireHandle = m_wireShaderProgram->GetHandle();
                    glUseProgram(wireHandle);

                    const int modelLocation = glGetUniformLocation(wireHandle, "model");
                    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, (float*)&finalTransform);

                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    glDrawElements(GL_TRIANGLES, m_selectedModelData->IndexCount, GL_UNSIGNED_INT, 0);
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                }

                if (m_solid)
                {
                    const int baseHandle = m_baseShaderProgram->GetHandle();
                    glUseProgram(baseHandle);

                    const int modelLocation = glGetUniformLocation(baseHandle, "model");
                    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, (float*)&finalTransform);

                    const int location = glGetUniformLocation(baseHandle, "MainTex");
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, tex->GetHandle());
                    glUniform1i(location, 0);

                    glDrawElements(GL_TRIANGLES, m_selectedModelData->IndexCount, GL_UNSIGNED_INT, 0);
                }
            }

            glDisable(GL_BLEND);

            m_renderTexture->Unbind();

            ImGui::Image((ImTextureID)m_renderTexture->GetTexture()->GetHandle(), { size.x - 20, size.y - 60 });
        }
        ImGui::End();
    }
}

ModelEditor* ModelEditor::Load(mz_zip_archive& a_archive)
{
    ModelEditor* modelEditor = new ModelEditor();

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
            property->EmplaceValue("modeltype", std::to_string((int)modelType).c_str());

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