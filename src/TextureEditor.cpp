#include "TextureEditor.h"

#include <glad/glad.h>
#include <stb/stb_image.h>

#include "DataStore.h"
#include "FileLoaders/ImageLoader.h"
#include "FileLoaders/KritaLoader.h"
#include "FileLoaders/PropertyFile.h"
#include "FileUtils.h"
#include "imgui.h"
#include "MemoryStream.h"
#include "ModelEditor.h"
#include "Models/Model.h"
#include "RenderTexture.h"
#include "SkeletonEditor.h"
#include "Texture.h"
#include "TriImage.h"
#include "WindowControls/TextureEditorWindow.h"
#include "Workspace.h"

TextureEditor::TextureEditor(Workspace* a_workspace) 
{
    m_workspace = a_workspace;

    m_layers = new std::list<LayerTexture>();

    m_selectedIndex = m_layers->end();

    m_window = new TextureEditorWindow(this);
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

    delete m_window;
}

bool TextureEditor::LayerSelected() const
{
    return m_selectedIndex != m_layers->end();
}

void TextureEditor::TriangulateClicked() 
{
    const LayerTexture layerTexture = *m_selectedIndex;

    TriImage* triImage = new TriImage(layerTexture.Data, layerTexture.Meta);

    const e_TriangulationMode triMode = m_window->GetTriangulationMode();

    const glm::ivec2 vSize = m_window->GetVoronoiSize();
    const glm::ivec2 texStep = m_window->GetTextureStep();

    const glm::ivec2 quadStep = m_window->GetQuadStep();

    const float channelDiff = m_window->GetChannelDifference();

    const float alphaThreshold = m_window->GetAlphaThreshold();

    switch (triMode)
    {
    case e_TriangulationMode::Alpha:
    {
        triImage->AlphaTriangulation(texStep.x, texStep.y, alphaThreshold, vSize.x, vSize.y);

        break;
    }
    case e_TriangulationMode::Outline:
    {
        triImage->OutlineTriangulation(channelDiff, texStep.x, texStep.y, alphaThreshold, vSize.x, vSize.y);

        break;
    }
    case e_TriangulationMode::Quad:
    {
        triImage->QuadTriangulation(quadStep.x, quadStep.y, texStep.x, texStep.y, alphaThreshold, vSize.x, vSize.y);

        break;
    }
    }

    const unsigned int indexCount = triImage->GetIndexCount();

    if (indexCount >= 3)
    {
        const unsigned int indexSize = indexCount * sizeof(unsigned int);
        unsigned int* indicies = new unsigned int[indexSize];
        memcpy(indicies, triImage->GetIndices(), indexSize);

        const unsigned int vertexCount = triImage->GetVertexCount();
        const unsigned int vertexSize = vertexCount * sizeof(ModelVertex);
        ModelVertex* modelVerticies = new ModelVertex[vertexSize];
        memcpy(modelVerticies, triImage->GetVertices(), vertexSize);

        m_workspace->AddModel(layerTexture.Meta->Name, modelVerticies, vertexCount, indicies, indexCount);
    }

    delete triImage;
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

    switch (iExt)
    {
    // .png
    case 0x676E702E:
    {
        LayerMeta* layerMeta = new LayerMeta();
        layerMeta->Name = nullptr;
        layerMeta->Width = -1;
        layerMeta->Height = -1;
        layerMeta->xOffset = 0;
        layerMeta->yOffset = 0;

        LayerTexture layerTexture;
        layerTexture.Meta = layerMeta;

        int channels;

        unsigned char* data = stbi_load(a_path, &layerMeta->Width, &layerMeta->Height, &channels, STBI_rgb_alpha);

        layerMeta->ImageWidth = layerMeta->Width;
        layerMeta->ImageHeight = layerMeta->Height;

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

        if (layerMeta->Name != nullptr && layerMeta->Width > 0 && layerMeta->Height > 0)
        {
            store->AddTexture(layerMeta->Name, GenerateTexture(layerTexture));

            m_layers->emplace_back(layerTexture);
        }

        break;
    }
    // .kra
    case 0x61726B2E:
    {
        KritaLoader* loader = KritaLoader::Load(a_path);

        const int layerCount = loader->GetLayerCount();

        for (int i = 0; i < layerCount; ++i)
        {
            Layer* layer = loader->ToLayer(i);

            if (layer != nullptr)
            {
                LayerMeta* layerMeta = new LayerMeta();

                LayerTexture layerTexture;

                layerTexture.Meta = layerMeta;
                layerTexture.Data = (unsigned char*)layer->Data;

                *layerMeta = layer->MetaData;

                if (layerMeta->Name != nullptr && layerMeta->Width > 0 && layerMeta->Height > 0)
                {
                    store->AddTexture(layerMeta->Name, GenerateTexture(layerTexture));

                    m_layers->emplace_back(layerTexture);
                }
            }
        }

        delete loader;

        break;
    }
    }
}

void TextureEditor::Update(double a_delta)
{
    m_window->Update();
}
void TextureEditor::DrawLayerGUI() 
{
    DataStore* dataStore = DataStore::GetInstance();

    auto removeIter = m_layers->end();

    for (auto iter = m_layers->begin(); iter != m_layers->end(); ++iter)
    {
        const LayerTexture layerTexture = *iter;
        const LayerMeta* layerMeta = layerTexture.Meta;
        const Texture* tex = dataStore->GetTexture(layerMeta->Name);

        if (tex != nullptr)
        {
            ImGui::Image((ImTextureID)tex->GetHandle(), { 20, 20 });
            ImGui::SameLine();
        }
        else
        {
            ImGui::Indent(20.0f);
        }

        bool is_selected = (m_selectedIndex == iter); 

        if (ImGui::Selectable(layerMeta->Name, &is_selected))
        {
            m_selectedIndex = iter;

            m_workspace->SelectWorkspace(this);
        }

        if (is_selected)
        {
            ImGui::SetItemDefaultFocus();
        }

        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Remove Texture"))
            {
                removeIter = iter;
            }
            ImGui::EndPopup();
        }
    }

    if (removeIter != m_layers->end())
    {   
        delete dataStore->GetTexture(removeIter->Meta->Name);
        dataStore->RemoveTexture(removeIter->Meta->Name);

        delete[] removeIter->Data;
        delete[] removeIter->Meta->Name;
        delete removeIter->Meta;

        if (removeIter == m_selectedIndex)
        {
            m_selectedIndex = m_layers->end();
        }

        m_layers->erase(removeIter);
    }
}

void TextureEditor::GetImageData(PropertyFileProperty& a_property, mz_zip_archive& a_archive)
{
    DataStore* dataStore = DataStore::GetInstance();

    LayerTexture layerTexture;

    LayerMeta* meta = layerTexture.Meta = new LayerMeta();
    meta->ImageWidth = -1;
    meta->ImageHeight = -1;
    meta->xOffset = 0;
    meta->yOffset = 0;

    for (auto iter = a_property.Values().begin(); iter != a_property.Values().end(); ++iter)
    {
        IFSETTOATTVALCPY("name", iter->Name, meta->Name, iter->Value)
        else IFSETTOATTVALI("imagewidth", iter->Name, meta->ImageWidth, iter->Value)
        else IFSETTOATTVALI("imageheight", iter->Name, meta->ImageHeight, iter->Value)
        else IFSETTOATTVALI("width", iter->Name, meta->Width, iter->Value)
        else IFSETTOATTVALI("height", iter->Name, meta->Height, iter->Value)
        else IFSETTOATTVALI("xoffset", iter->Name, meta->xOffset, iter->Value)
        else IFSETTOATTVALI("yoffset", iter->Name, meta->yOffset, iter->Value)
    }

    if (meta->ImageWidth == -1)
    {
        meta->ImageWidth = meta->Width;
    }
    if (meta->ImageHeight == -1)
    {
        meta->ImageHeight = meta->Height;
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

TextureEditor* TextureEditor::Load(mz_zip_archive& a_archive, Workspace* a_workspace)
{
    TextureEditor* textureEditor = new TextureEditor(a_workspace);

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
        property->EmplaceValue("imagewidth", std::to_string(layerMeta->ImageWidth).c_str());
        property->EmplaceValue("imageheight", std::to_string(layerMeta->ImageHeight).c_str());
        property->EmplaceValue("width", std::to_string(layerMeta->Width).c_str());
        property->EmplaceValue("height", std::to_string(layerMeta->Height).c_str());
        property->EmplaceValue("xoffset", std::to_string(layerMeta->xOffset).c_str());
        property->EmplaceValue("yoffset", std::to_string(layerMeta->yOffset).c_str());
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

void TextureEditor::DrawPropertiesWindow()
{
    if (LayerSelected())
    {
        m_window->UpdatePropertiesWindow();
    }
}