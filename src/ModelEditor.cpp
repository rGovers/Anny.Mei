#include "ModelEditor.h"

#include <glad/glad.h>
#include <string.h>

#include "DataStore.h"
#include "FileUtils.h"
#include "imgui/imgui.h"
#include "Material.h"
#include "Models/Model.h"
#include "Name.h"
#include "Namer.h"
#include "PropertyFile.h"
#include "RenderTexture.h"
#include "ShaderProgram.h"
#include "Shaders/ModelVertex.h"
#include "Shaders/SolidPixel.h"
#include "Shaders/StandardPixel.h"
#include "Texture.h"

ModelEditor::ModelEditor()
{
    m_models = new std::list<ModelData*>();

    m_namer = new Namer();

    m_renderTexture = new RenderTexture(2048, 2048, GL_RGBA);

    m_selectedModelData = nullptr;

    m_solid = true;
    m_wireframe = false;

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
        delete (*iter)->ModelP;

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

    for (auto iter = a_property.Values().begin(); iter != a_property.Values().end(); ++iter)
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

        ModelData* modelData = AddModel(texName, name, trueName, vertices, vertexCount, indicies, indexCount);

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

ModelEditor::ModelData* ModelEditor::AddModel(const char* a_textureName, const char* a_name, const char* a_trueName, ModelVertex* a_vertices, unsigned int a_vertexCount, unsigned int* a_indices, unsigned int a_indexCount) const
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
    
    modelData->ModelName = new Name(a_trueName, m_namer);
    modelData->ModelP = new Model();
    modelData->VertexCount = a_vertexCount;
    modelData->Vertices = a_vertices;
    modelData->IndexCount = a_indexCount;
    modelData->Indices = a_indices;

    if (a_name != nullptr)
    {
        modelData->ModelName->SetName(a_name);
    }

    const unsigned int vbo = modelData->ModelP->GetVBO();
    const unsigned int ibo = modelData->ModelP->GetIBO(); 

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, a_vertexCount * sizeof(ModelVertex), a_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, a_indexCount * sizeof(unsigned int), a_indices, GL_STATIC_DRAW);

    modelData->ModelP->SetIndiciesCount(a_indexCount);

    const char* name = modelData->ModelName->GetName();

    dataStore->AddModel(name, e_ModelType::Base, modelData->ModelP);
    dataStore->SetModelTextureName(name, a_textureName);

    return modelData;
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
            if (m_selectedModelData == *removeIter)
            {
                m_selectedModelData = nullptr;
            }

            delete[] (*removeIter)->TextureName;
            delete (*removeIter)->ModelName;
            delete (*removeIter)->ModelP;

            delete *removeIter;

            m_models->erase(removeIter);
        }

        ImGui::EndChild();
    }
    ImGui::End();

    if (m_selectedModelData != nullptr)
    {
        glm::mat4 transform = glm::mat4(2);
        transform[3] = { -1.0f, -1.0f, 0, 1 };

        ImGui::SetNextWindowSize({ 420, 440 }, ImGuiCond_Appearing);
        if (ImGui::Begin("Model Editor", nullptr, ImGuiWindowFlags_MenuBar))
        {
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("View"))
                {
                    ImGui::MenuItem("Solid", nullptr, &m_solid);
                    ImGui::MenuItem("Wireframe", nullptr, &m_wireframe);
        
                    ImGui::EndMenu();
                }
        
                ImGui::EndMenuBar();
            }

            m_renderTexture->Bind();

            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

            Texture* tex = nullptr;

            const char* texName = m_selectedModelData->TextureName;

            if (texName != nullptr)
            {
                tex = store->GetTexture(texName);
            }

            if (tex != nullptr)
            {
                glBindVertexArray(m_selectedModelData->ModelP->GetVAO());

                if (m_wireframe)
                {
                    const int wireHandle = m_wireShaderProgram->GetHandle();
                    glUseProgram(wireHandle);

                    const int modelLocation = glGetUniformLocation(wireHandle, "model");
                    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, (float*)&transform);

                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    glDrawElements(GL_TRIANGLES, m_selectedModelData->IndexCount, GL_UNSIGNED_INT, 0);
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                }

                if (m_solid)
                {
                    const int baseHandle = m_baseShaderProgram->GetHandle();
                    glUseProgram(baseHandle);

                    const int modelLocation = glGetUniformLocation(baseHandle, "model");
                    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, (float*)&transform);

                    const int location = glGetUniformLocation(baseHandle, "MainTex");
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, tex->GetHandle());
                    glUniform1i(location, 0);

                    glDrawElements(GL_TRIANGLES, m_selectedModelData->IndexCount, GL_UNSIGNED_INT, 0);
                }
            }

            m_renderTexture->Unbind();

            ImGui::Image((ImTextureID)m_renderTexture->GetTexture()->GetHandle(), { 400, 400 });
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

            const e_ModelType modelType = (*iter)->ModelP->GetModelType();

            property->SetName("model");
            property->EmplaceValue("name", name);
            property->EmplaceValue("truename", (*iter)->ModelName->GetTrueName());
            property->EmplaceValue("texname", (*iter)->TextureName);
            property->EmplaceValue("indices", std::to_string(indexCount).c_str());
            property->EmplaceValue("vertices", std::to_string(vertexCount).c_str());
            property->EmplaceValue("modeltype", std::to_string((int)modelType).c_str());

            const unsigned int indiciesSize = indexCount * sizeof(unsigned int);
            unsigned int verticesSize = -1;
            switch (modelType)
            {
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
    ModelData* modelData = AddModel(a_textureName, nullptr, a_textureName, a_vertices, a_vertexCount, a_indices, a_indexCount);    

    if (modelData != nullptr)
    {
        m_models->emplace_back(modelData);
    }
}