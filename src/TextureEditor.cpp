#include "TextureEditor.h"

#include <glad/glad.h>
#include <stb/stb_image.h>

#include "FileUtils.h"
#include "imgui.h"
#include "MemoryStream.h"
#include "ModelEditor.h"
#include "PropertyFile.h"
#include "RenderTexture.h"
#include "SkeletonEditor.h"
#include "Texture.h"
#include "TriImage.h"

const char* TextureEditor::ITEMS[] = { "Alpha", "Quad", "Outline" };

TextureEditor::TextureEditor() : 
    m_stepXY({ 64, 64 }),
    m_vSize({ 512, 512 }),
    m_texStep({ 4, 4 })
{
    m_channelDiff = 0.1f;

    m_layers = new std::vector<LayerTexture>();

    m_selectedIndex = -1;

    m_selectedMode = ITEMS[1];
    m_triangulationMode = e_TriangulationMode::Quad;

    m_alphaThreshold = 0.9f;
}
TextureEditor::~TextureEditor()
{
    DataStore* dataStore = DataStore::GetInstance();

    for (auto iter = m_layers->begin(); iter != m_layers->end(); ++iter)
    {
        delete dataStore->GetTexture(iter->Meta->Name);
        dataStore->RemoveTexture(iter->Meta->Name);

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
    DataStore* store = DataStore::GetInstance();

    const std::string path = a_path;
    const size_t index = path.find_last_of('.');
    const size_t len = path.length();

    const std::string ext = path.substr(index, len - index);
    const char* cExt = ext.c_str();
    const unsigned int iExt = *(unsigned int*)cExt;

    LayerMeta* layerMeta = new LayerMeta();
    layerMeta->Name = nullptr;

    LayerTexture layerTexture;

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
        layerTexture.Data = new unsigned char[size];/* constant-expression */
        memcpy(layerTexture.Data, data, size);

        stbi_image_free(data);

        const size_t fileNameIndex = path.find_last_of('/');
        const std::string filename = path.substr(fileNameIndex + 1, index - fileNameIndex - 1);

        const size_t fileNameLen = filename.length(); 

        layerMeta->Name = new char[fileNameLen + 1];
        strcpy(layerMeta->Name, filename.c_str());
    }

    layerTexture.Meta = layerMeta;

    store->AddTexture(layerMeta->Name, GenerateTexture(layerTexture));

    m_layers->emplace_back(layerTexture);
}

void TextureEditor::Update(double a_delta, ModelEditor* a_modelEditor)
{
    DataStore* dataStore = DataStore::GetInstance();
        
    if (m_selectedIndex != -1)
    {
        ImGui::SetNextWindowSize({ 200, 200 }, ImGuiCond_Appearing);
        if (ImGui::Begin("Texture Editor Toolbox"))
        {
            ImGui::InputInt2("Voronoi Size", (int*)m_vSize);
            ImGui::InputInt2("Texture Step", (int*)m_texStep);
            ImGui::DragFloat("Alpha Threshold", &m_alphaThreshold, 0.01f, 0.0001f, 1.0f);

            if (ImGui::BeginCombo("Triangulation Mode", m_selectedMode))
            {
                for (int i = 0; i < IM_ARRAYSIZE(ITEMS); ++i)
                {
                    bool is_selected = (m_selectedMode == ITEMS[i]); 
                    if (ImGui::Selectable(ITEMS[i], is_selected))
                    {
                        m_selectedMode = ITEMS[i];
                    }
                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            if (strcmp(m_selectedMode, "Outline") == 0)
            {
                m_triangulationMode = e_TriangulationMode::Outline;
            }
            else if (strcmp(m_selectedMode, "Alpha") == 0)
            {
                m_triangulationMode = e_TriangulationMode::Alpha;
            }
            else
            {
                m_triangulationMode = e_TriangulationMode::Quad;
            }
            
            switch (m_triangulationMode)
            {
            case e_TriangulationMode::Outline:
            {
                ImGui::DragFloat("Channel Difference", &m_channelDiff, 0.01f, 0.0001f, 1.0f);

                break;
            }
            case e_TriangulationMode::Quad:
            {
                ImGui::InputInt2("Step XY", (int*)m_stepXY);

                break;
            }
            }

            if (ImGui::Button("Triangulate", { 200, 20 }))
            {
                const LayerTexture layerTexture = m_layers->at(m_selectedIndex);

                TriImage* triImage = new TriImage(layerTexture.Data, layerTexture.Meta->Width, layerTexture.Meta->Height);

                switch (m_triangulationMode)
                {
                case e_TriangulationMode::Alpha:
                {
                    triImage->AlphaTriangulation(m_texStep[0], m_texStep[1], m_alphaThreshold, m_vSize[0], m_vSize[1]);

                    break;
                }
                case e_TriangulationMode::Outline:
                {
                    triImage->OutlineTriangulation(m_channelDiff, m_texStep[0], m_texStep[1], m_alphaThreshold, m_vSize[0], m_vSize[1]);

                    break;
                }
                case e_TriangulationMode::Quad:
                {
                    triImage->QuadTriangulation(m_stepXY[0], m_stepXY[1], m_texStep[0], m_texStep[1], m_alphaThreshold, m_vSize[0], m_vSize[1]);

                    break;
                }
                }

                const unsigned int indexCount = triImage->GetIndexCount();

                const unsigned int indexSize = indexCount * sizeof(unsigned int);
                unsigned int* indicies = new unsigned int[indexSize];
                memcpy(indicies, triImage->GetIndices(), indexSize);

                const unsigned int vertexCount = triImage->GetVertexCount();
                ModelVertex* modelVerticies = triImage->ToModelVertices();

                a_modelEditor->AddModel(layerTexture.Meta->Name, modelVerticies, vertexCount, indicies, indexCount);

                delete triImage;
            }
        }
        ImGui::End();
    }

    ImGui::SetNextWindowSize({ 250, 600 }, ImGuiCond_Appearing);
    if (ImGui::Begin("Texture List"))
    {
        ImGui::BeginChild("Layer Scroll");
        
        int remove = -1;

        for (int i = 0; i < m_layers->size(); ++i)
        {
            const LayerTexture layerTexture = m_layers->at(i);
            const LayerMeta* layerMeta = layerTexture.Meta;

            const Texture* tex = dataStore->GetTexture(layerTexture.Meta->Name);

            if (tex != nullptr)
            {
                ImGui::Image((ImTextureID)tex->GetHandle(), { 20, 20 });
                ImGui::SameLine();
            }
            else
            {
                ImGui::Indent(20.0f);
            }

            if (ImGui::Selectable(layerMeta->Name))
            {
                m_selectedIndex = i;
            }

            if (ImGui::BeginPopupContextItem())
            {
                if (ImGui::MenuItem("Remove Texture"))
                {
                    remove = i;
                }

                ImGui::EndPopup();
            }
        }

        if (remove != -1)
        {
            auto iter = m_layers->begin() + remove;
            
            delete dataStore->GetTexture(iter->Meta->Name);
            dataStore->RemoveTexture(iter->Meta->Name);
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
    DataStore* dataStore = DataStore::GetInstance();

    LayerTexture layerTexture;

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
        dataStore->AddTexture(layerTexture.Meta->Name, GenerateTexture(layerTexture));
    }

    m_layers->emplace_back(layerTexture);
}

unsigned int TextureEditor::GetLayerCount() const
{
    return m_layers->size();
}

LayerMeta TextureEditor::GetLayerMeta(unsigned int a_index) const
{
    return *m_layers->at(a_index).Meta;
}

TextureEditor* TextureEditor::Load(mz_zip_archive& a_archive)
{
    TextureEditor* textureEditor = new TextureEditor();

    char* propertiesData = ExtractFileFromArchive("texture.prop", a_archive);

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

void TextureEditor::Save(mz_zip_archive& a_archive) const
{
    if (m_layers->size() > 0)
    {
        SaveImageData(a_archive);
    }
}
