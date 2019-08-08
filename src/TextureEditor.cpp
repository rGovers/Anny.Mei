#include "TextureEditor.h"

#include <glad/glad.h>
#include <stb/stb_image.h>

#include "FileUtils.h"
#include "imgui.h"
#include "MemoryStream.h"
#include "ModelPreview.h"
#include "PropertyFile.h"
#include "RenderTexture.h"
#include "SkeletonController.h"
#include "Texture.h"
#include "TriImage.h"

TextureEditor::TextureEditor() : 
    m_selectedIndex(-1),
    m_stepXY({128, 128})
{
    m_layers = new std::vector<LayerTexture>();
}
TextureEditor::~TextureEditor()
{
    for (auto iter = m_layers->begin(); iter != m_layers->end(); ++iter)
    {
        if (iter->Meta != nullptr)
        {
            delete[] iter->Meta->Name;
            delete iter->Meta;
        }

        if (iter->Data != nullptr)
        {
            delete[] iter->Data;
        }
        if (iter->TextureData != nullptr)
        {
            delete iter->TextureData;
        }
    }
    delete m_layers;
}

void TextureEditor::GenerateTexture(LayerTexture& a_layerTexture) const
{
    const LayerMeta* layerMeta = a_layerTexture.Meta;

    a_layerTexture.TextureData = new Texture(layerMeta->Width, layerMeta->Height, GL_RGBA);
    const unsigned int handle = a_layerTexture.TextureData->GetHandle();

    glBindTexture(GL_TEXTURE_2D, handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, layerMeta->Width, layerMeta->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, a_layerTexture.Data);
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

    GenerateTexture(layerTexture);

    m_layers->emplace_back(layerTexture);
}

void TextureEditor::Update(double a_delta, SkeletonController* a_skeletonController)
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
            ImGui::InputInt2("StepXY", (int*)m_stepXY);

            if (ImGui::Button("Triangulate", { 200, 20 }))
            {
                LayerTexture layerTexture = m_layers->at(m_selectedIndex);

                if (layerTexture.ModelData != nullptr)
                {
                    delete layerTexture.ModelData;
                }

                TriImage* triImage = new TriImage(layerTexture.Data, m_stepXY[0], m_stepXY[1], layerTexture.Meta->Width, layerTexture.Meta->Height, 0.1f);
                
                const unsigned int indexCount = triImage->GetIndexCount();
                unsigned int* indicies = triImage->GetIndicies();

                const unsigned int vertexCount = triImage->GetVertexCount();
                ModelVertex* modelVerticies = triImage->ToModelVerticies();

                Model* model = new Model();
                const unsigned int vbo = model->GetVBO();
                const unsigned int ibo = model->GetIBO(); 

                a_skeletonController->SetModel(m_selectedIndex, model);

                glBindBuffer(GL_ARRAY_BUFFER, vbo);
                glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(ModelVertex), modelVerticies, GL_STATIC_DRAW);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(int), indicies, GL_STATIC_DRAW);

                model->SetIndicies(indexCount);

                layerTexture.ModelData = new ModelPreview(layerTexture.TextureData, model);

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

            ImGui::SameLine();

            ImGui::Image((ImTextureID)layerTexture.TextureData->GetHandle(), { 20, 20 });
        }

        if (remove != -1)
        {
            auto iter = m_layers->begin() + remove;

            delete iter->TextureData;
            delete[] iter->Data;
            delete[] iter->Meta->Name;
            delete iter->Meta;

            m_layers->erase(iter);

            if (remove == m_selectedIndex)
            {
                m_selectedIndex = -1;
            }
        }
        ImGui::EndChild();
    }
    ImGui::End();
}

void TextureEditor::GetImageData(PropertyFileProperty& a_property, ZipArchive::Ptr& a_archive)
{
    const char* name = a_property.GetName();

    LayerTexture layerTexture;
    LayerMeta* meta = layerTexture.Meta = new LayerMeta();

    for (auto iter = a_property.Values().begin(); iter != a_property.Values().end(); ++iter)
    {
        IFSETTOATTVALCPY("name", iter->Name, meta->Name, iter->Value)
        else IFSETTOATTVALI("width", iter->Name, meta->Width, iter->Value)
        else IFSETTOATTVALI("height", iter->Name, meta->Height, iter->Value)
    }

    std::shared_ptr<ZipArchiveEntry> entry = a_archive->GetEntry(std::string(meta->Name) + ".imgbin");

    if (entry != nullptr)
    {
        char* data;
        GETFILEDATA(data, entry);

        layerTexture.Data = (unsigned char*)data;
        GenerateTexture(layerTexture);
    }

    layerTexture.ModelData = nullptr;
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

TextureEditor* TextureEditor::Load(ZipArchive::Ptr& a_archive)
{
    TextureEditor* textureEditor = new TextureEditor();

    std::shared_ptr<ZipArchiveEntry> modelEntry = a_archive->GetEntry("model.prop");
    if (modelEntry != nullptr)
    {
        char* propertiesData;
        GETFILEDATA(propertiesData, modelEntry);

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
        delete[] propertiesData;
    }

    return textureEditor;
}
std::istream* TextureEditor::SaveToStream() const
{
    if (m_layers->size() > 0)
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

        IMemoryStream* memoryStream = new IMemoryStream(data, strlen(data));

        delete[] data;

        return memoryStream;
    }

    return nullptr;   
}
std::list<ModelFile> TextureEditor::SaveLayer(unsigned int a_index) const
{
    std::list<ModelFile> outputStreams;
    
    const LayerTexture layerTexture = m_layers->at(a_index);
    const size_t size = layerTexture.Meta->Width * layerTexture.Meta->Height * 4;

    IMemoryStream* memoryStream = new IMemoryStream((char*)layerTexture.Data, size);
    outputStreams.emplace_back(ModelFile{ e_ModelType::Image, memoryStream });

    return outputStreams;
}