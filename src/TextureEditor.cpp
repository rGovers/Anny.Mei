#include "TextureEditor.h"

#include <glad/glad.h>
#include <stb/stb_image.h>

#include "FileUtils.h"
#include "imgui.h"
#include "MemoryStream.h"
#include "ModelPreview.h"
#include "PropertyFile.h"
#include "RenderTexture.h"
#include "SkeletonEditor.h"
#include "Texture.h"
#include "TriImage.h"

TextureEditor::TextureEditor(DataStore* a_dataStore) : 
    m_selectedIndex(-1),
    m_stepXY({ 64, 64 }),
    m_vSize({ 512, 512 }),
    m_dataStore(a_dataStore)
{
    m_layers = new std::vector<LayerTexture>();
}
TextureEditor::~TextureEditor()
{
    for (auto iter = m_layers->begin(); iter != m_layers->end(); ++iter)
    {
        delete m_dataStore->GetTexture(iter->Meta->Name);
        m_dataStore->RemoveTexture(iter->Meta->Name);

        if (iter->Meta != nullptr)
        {
            delete[] iter->Meta->Name;
            delete iter->Meta;
        }

        if (iter->Data != nullptr)
        {
            delete[] iter->Data;
        }
    }
    delete m_layers;
}

Texture* TextureEditor::GenerateTexture(LayerTexture& a_layerTexture) const
{
    const LayerMeta* layerMeta = a_layerTexture.Meta;

    Texture* texture = new Texture(layerMeta->Width, layerMeta->Height, GL_RGBA);
    const unsigned int handle = texture->GetHandle();

    glBindTexture(GL_TEXTURE_2D, handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, layerMeta->Width, layerMeta->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, a_layerTexture.Data);

    return texture;
}

void TextureEditor::LoadTexture(const char* a_path)
{
    const std::string path = a_path;
    const size_t index = path.find_last_of('.');
    const size_t len = path.length();

    const std::string ext = path.substr(index, len - index);
    const char* cExt = ext.c_str();
    const unsigned int iExt = *(unsigned int*)cExt;

    LayerMeta* layerMeta = new LayerMeta();
    layerMeta->Name = nullptr;

    LayerTexture layerTexture;
    layerTexture.Vertices = nullptr;
    layerTexture.Indices = nullptr;

    // .png
    if (iExt == 0x676E702E)
    {
        int channels;

        unsigned char* data = stbi_load(a_path, &layerMeta->Width, &layerMeta->Height, &channels, STBI_rgb_alpha);

        if (data == nullptr)
        {
            delete layerMeta;

            return;
        }

        const size_t size = layerMeta->Width * layerMeta->Height * 4;
        layerTexture.Data = new unsigned char[size];
        memcpy(layerTexture.Data, data, size);

        stbi_image_free(data);

        const size_t fileNameIndex = path.find_last_of('/');
        const std::string filename = path.substr(fileNameIndex + 1, index - fileNameIndex - 1);

        const size_t fileNameLen = filename.length(); 

        layerMeta->Name = new char[fileNameLen + 1];
        strcpy(layerMeta->Name, filename.c_str());
    }

    layerTexture.Meta = layerMeta;
    layerTexture.ModelData = nullptr;

    m_dataStore->AddTexture(layerMeta->Name, GenerateTexture(layerTexture));

    m_layers->emplace_back(layerTexture);
}

void TextureEditor::Update(double a_delta)
{
    if (m_selectedIndex != -1)
    {
        LayerTexture layerTexture = m_layers->at(m_selectedIndex);
        ModelPreview* modelData = layerTexture.ModelData;

        if (modelData != nullptr)
        {
            if (ImGui::Begin("Texture Editor Preview", nullptr, ImGuiWindowFlags_MenuBar))
            {
                modelData->Update();
                modelData->Render();

                ImGui::Image((ImTextureID)modelData->GetRenderTexture()->GetTexture()->GetHandle(), { 512, 512 });
            }
            ImGui::End();
        }
    }
        
    if (m_selectedIndex != -1)
    {
        ImGui::SetNextWindowSize({ 200, 200 }, ImGuiCond_Appearing);
        if (ImGui::Begin("Texture Editor Toolbox"))
        {
            ImGui::InputInt2("Step XY", (int*)m_stepXY);
            ImGui::InputInt2("Voronoi Size", (int*)m_vSize);

            if (ImGui::Button("Triangulate", { 200, 20 }))
            {
                LayerTexture layerTexture = m_layers->at(m_selectedIndex);

                if (layerTexture.ModelData != nullptr)
                {
                    delete layerTexture.ModelData;
                }
                if (layerTexture.Indices != nullptr)
                {
                    delete[] layerTexture.Indices;
                }
                if (layerTexture.Vertices != nullptr)
                {
                    delete[] layerTexture.Vertices;
                }

                TriImage* triImage = new TriImage(layerTexture.Data, m_stepXY[0], m_stepXY[1], layerTexture.Meta->Width, layerTexture.Meta->Height, m_vSize[0], m_vSize[1], 0.1f);
                
                const unsigned int indexCount = triImage->GetIndexCount();

                const unsigned int indexSize = indexCount * sizeof(unsigned int);
                unsigned int* indicies = new unsigned int[indexSize];
                memcpy(indicies, triImage->GetIndicies(), indexSize);

                const unsigned int vertexCount = triImage->GetVertexCount();
                ModelVertex* modelVerticies = triImage->ToModelVerticies();

                Model* model = new Model();
                const unsigned int vbo = model->GetVBO();
                const unsigned int ibo = model->GetIBO(); 

                glBindBuffer(GL_ARRAY_BUFFER, vbo);
                glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(ModelVertex), modelVerticies, GL_STATIC_DRAW);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indicies, GL_STATIC_DRAW);

                model->SetIndiciesCount(indexCount);
                model->SetVerticiesCount(vertexCount);

                m_dataStore->AddModel(layerTexture.Meta->Name, e_ModelType::Base, model);
                m_dataStore->SetModelTextureName(layerTexture.Meta->Name, layerTexture.Meta->Name);

                layerTexture.ModelData = new ModelPreview(layerTexture.Meta->Name, layerTexture.Meta->Name, m_dataStore);
                layerTexture.Indices = indicies;
                layerTexture.Vertices = modelVerticies;

                (*m_layers)[m_selectedIndex] = layerTexture;

                delete triImage;
            }
        }
        ImGui::End();
    }

    ImGui::SetNextWindowSize({ 250, 600 }, ImGuiCond_Appearing);
    if (ImGui::Begin("Texture Editor Layers"))
    {
        ImGui::BeginChild("Layer Scroll");
        
        int remove = -1;

        for (int i = 0; i < m_layers->size(); ++i)
        {
            const LayerTexture layerTexture = m_layers->at(i);
            const LayerMeta* layerMeta = layerTexture.Meta;

            if (ImGui::SmallButton("X"))
            {
               remove = i;
            }

            ImGui::SameLine();

            if (ImGui::Button(layerMeta->Name, { 180, 20 }))
            {
                m_selectedIndex = i;
            }

            const Texture* tex = m_dataStore->GetTexture(layerTexture.Meta->Name);

            if (tex != nullptr)
            {
                ImGui::SameLine();
                ImGui::Image((ImTextureID)tex->GetHandle(), { 20, 20 });
            }
        }

        if (remove != -1)
        {
            auto iter = m_layers->begin() + remove;
            
            delete m_dataStore->GetTexture(iter->Meta->Name);
            m_dataStore->RemoveTexture(iter->Meta->Name);
            delete[] iter->Data;
            delete[] iter->Meta->Name;
            delete iter->Meta;

            m_layers->erase(iter);

            m_selectedIndex = -1;
        }
        ImGui::EndChild();
    }
    ImGui::End();
}

void TextureEditor::GetImageData(PropertyFileProperty& a_property, mz_zip_archive& a_archive)
{
    LayerTexture layerTexture;
    layerTexture.Vertices = nullptr;
    layerTexture.Indices = nullptr;

    LayerMeta* meta = layerTexture.Meta = new LayerMeta();

    for (auto iter = a_property.Values().begin(); iter != a_property.Values().end(); ++iter)
    {
        IFSETTOATTVALCPY("name", iter->Name, meta->Name, iter->Value)
        else IFSETTOATTVALI("width", iter->Name, meta->Width, iter->Value)
        else IFSETTOATTVALI("height", iter->Name, meta->Height, iter->Value)
    }

    std::string fileName = "img/" + std::string(meta->Name) + ".imgbin";
    char* data = ExtractFileFromArchive(fileName.c_str(), a_archive);
    if (data != nullptr)
    {
        layerTexture.Data = (unsigned char*)data;
        m_dataStore->AddTexture(layerTexture.Meta->Name, GenerateTexture(layerTexture));
    }

    layerTexture.ModelData = nullptr;

    m_layers->emplace_back(layerTexture);
}
void TextureEditor::GetModelData(PropertyFileProperty& a_property, mz_zip_archive& a_archive)
{
    char* name = nullptr;
    int vertexCount = -1;
    int indexCount = -1;

    for (auto iter = a_property.Values().begin(); iter != a_property.Values().end(); ++iter)
    {
        IFSETTOATTVALCPY("name", iter->Name, name, iter->Value)
        else IFSETTOATTVALI("verticies", iter->Name, vertexCount, iter->Value)
        else IFSETTOATTVALI("indicies", iter->Name, indexCount, iter->Value)
    }

    if (name != nullptr && vertexCount > 0 && indexCount > 0)
    {
        for (int i = 0; i < m_layers->size(); ++i)
        {
            LayerTexture layerTexture = m_layers->at(i);

            if (strcmp(layerTexture.Meta->Name, name) == 0)
            {
                std::string vertexFileName = "mdl/" + std::string(name) + ".verbin";
                std::string indexFileName = "mdl/" + std::string(name) + ".indbin";

                unsigned int* indicies = (unsigned int*)ExtractFileFromArchive(indexFileName.c_str(), a_archive);
                ModelVertex* vertices = (ModelVertex*)ExtractFileFromArchive(vertexFileName.c_str(), a_archive);

                if (indicies != nullptr && vertices != nullptr)
                {
                    const unsigned int vertexSize = sizeof(ModelVertex) * vertexCount;
                    const unsigned int indexSize = sizeof(unsigned int) * indexCount;

                    Model* model = new Model();
                    const unsigned int vbo = model->GetVBO();
                    const unsigned int ibo = model->GetIBO(); 

                    glBindBuffer(GL_ARRAY_BUFFER, vbo);
                    glBufferData(GL_ARRAY_BUFFER, vertexSize, vertices, GL_STATIC_DRAW);

                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
                    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, indicies, GL_STATIC_DRAW);

                    model->SetIndiciesCount(indexCount);
                    model->SetVerticiesCount(vertexCount);

                    m_dataStore->AddModel(name, e_ModelType::Base, model);
                    m_dataStore->SetModelTextureName(name, layerTexture.Meta->Name);

                    layerTexture.ModelData = new ModelPreview(layerTexture.Meta->Name, layerTexture.Meta->Name, m_dataStore);

                    layerTexture.Vertices = vertices;
                    layerTexture.Indices = indicies;

                    (*m_layers)[i] = layerTexture;
                }

                break;
            }
        }

        delete[] name;
    }
}

unsigned int TextureEditor::GetLayerCount() const
{
    return m_layers->size();
}

LayerMeta TextureEditor::GetLayerMeta(unsigned int a_index) const
{
    return *m_layers->at(a_index).Meta;
}

TextureEditor* TextureEditor::Load(mz_zip_archive& a_archive, DataStore* a_dataStore)
{
    TextureEditor* textureEditor = new TextureEditor(a_dataStore);

    char* propertiesData;

    propertiesData = ExtractFileFromArchive("texture.prop", a_archive);
    if (propertiesData != nullptr)
    {
        PropertyFile* propertiesFile = new PropertyFile(propertiesData);

        const std::list<PropertyFileProperty*> properties = propertiesFile->GetProperties();
        for (auto iter = properties.begin(); iter != properties.end(); ++iter)
        {
            PropertyFileProperty* prop = *iter;
            // Looking for root elements
            // I am lazy and cant be stuff writing a iterator for the file
            if (prop->GetParent() == nullptr)
            {
                textureEditor->GetImageData(*prop, a_archive);
            } 
        } 

        delete propertiesFile;
        mz_free(propertiesData);     
    }

    propertiesData = ExtractFileFromArchive("model.prop", a_archive);
    if (propertiesData != nullptr)
    {
        PropertyFile* propertiesFile = new PropertyFile(propertiesData);

        const std::list<PropertyFileProperty*> properties = propertiesFile->GetProperties();

        for (auto iter = properties.begin(); iter != properties.end(); ++iter)
        {
            PropertyFileProperty* prop = *iter;

            if (prop->GetParent() == nullptr)
            {
                textureEditor->GetModelData(*prop, a_archive);
            }
        }

        delete propertiesFile;
        mz_free(propertiesData);
    }

    return textureEditor;
}

void TextureEditor::SaveImageData(mz_zip_archive& a_archive) const
{
    PropertyFile* propertyFile = new PropertyFile();
    for (auto iter = m_layers->begin(); iter != m_layers->end(); ++iter)
    {
        const LayerMeta* layerMeta = iter->Meta;

        PropertyFileProperty* property = propertyFile->InsertProperty();

        property->SetName("image");
        property->EmplaceValue("name", layerMeta->Name);
        property->EmplaceValue("width", std::to_string(layerMeta->Width).c_str());
        property->EmplaceValue("height", std::to_string(layerMeta->Height).c_str());
    }

    char* data = propertyFile->ToString();

    mz_zip_writer_add_mem(&a_archive, "texture.prop", data, strlen(data), MZ_DEFAULT_COMPRESSION);

    delete propertyFile;
    delete[] data;

    for (auto iter = m_layers->begin(); iter != m_layers->end(); ++iter)
    {
        const LayerMeta* layerMeta = iter->Meta;
        const size_t size = layerMeta->Width * layerMeta->Height * 4;

        const std::string name = "img/" + std::string(layerMeta->Name) + ".imgbin";

        mz_zip_writer_add_mem(&a_archive, name.c_str(), iter->Data, size, MZ_DEFAULT_COMPRESSION);
    }
}
void TextureEditor::SaveModelData(mz_zip_archive& a_archive) const
{
    PropertyFile* propertyFile = new PropertyFile();
    for (auto iter = m_layers->begin(); iter != m_layers->end(); ++iter)
    {
        const char* name = iter->Meta->Name;

        const Model* model = m_dataStore->GetModel(name, e_ModelType::Base);

        if (model != nullptr)
        {
            const unsigned int vertexCount = model->GetVerticiesCount(); 
            const unsigned int indexCount = model->GetIndiciesCount();
    
            if (vertexCount != 0 && indexCount != 0)
            {
                PropertyFileProperty* property = propertyFile->InsertProperty();
    
                property->SetName("model");
                property->EmplaceValue("name", name);
                property->EmplaceValue("indicies", std::to_string(indexCount).c_str());
                property->EmplaceValue("verticies", std::to_string(vertexCount).c_str());
                property->EmplaceValue("modeltype", std::to_string((int)e_ModelType::Base).c_str());

                const unsigned int indiciesSize = indexCount * sizeof(unsigned int);
                const unsigned int verticiesSize = vertexCount * sizeof(ModelVertex);

                std::string indexFileName = "mdl/" + std::string(name) + ".indbin";
                std::string vertexFileName = "mdl/" + std::string(name) + ".verbin";

                mz_zip_writer_add_mem(&a_archive, indexFileName.c_str(), iter->Indices, indiciesSize, MZ_DEFAULT_COMPRESSION);
                mz_zip_writer_add_mem(&a_archive, vertexFileName.c_str(), iter->Vertices, verticiesSize, MZ_DEFAULT_COMPRESSION);
            }
        }
    }


    if (propertyFile->PropertyCount() > 0)
    {
        char* data = propertyFile->ToString();

        mz_zip_writer_add_mem(&a_archive, "model.prop", data, strlen(data), MZ_DEFAULT_COMPRESSION);
    
        delete[] data;
    }
    
    delete propertyFile; 
}

void TextureEditor::Save(mz_zip_archive& a_archive) const
{
    PropertyFile* propertyFile;
    char* data;

    if (m_layers->size() > 0)
    {
        SaveImageData(a_archive);
        SaveModelData(a_archive);
    }
}
