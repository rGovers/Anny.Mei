#include "ModelEditor.h"

#include <glad/glad.h>
#include <stb/stb_image.h>

#include "FileUtils.h"
#include "imgui.h"
#include "MemoryStream.h"
#include "PropertyFile.h"

ModelEditor::ModelEditor() : 
    m_selectedIndex(-1)
{
    m_layers = new std::vector<LayerTexture>();
}
ModelEditor::~ModelEditor()
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
            glDeleteTextures(1, &iter->Handle);
            delete[] iter->Data;
        }
    }
    delete m_layers;
}

void ModelEditor::GenerateTexture(LayerTexture& a_layerTexture) const
{
    const LayerMeta* layerMeta = a_layerTexture.Meta;

    glGenTextures(1, &a_layerTexture.Handle);
    glBindTexture(GL_TEXTURE_2D, a_layerTexture.Handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, layerMeta->Width, layerMeta->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, a_layerTexture.Data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
}

void ModelEditor::LoadTexture(const char* a_path)
{
    const std::string path = a_path;
    const size_t index = path.find_last_of('.');
    const size_t len = path.length();

    const std::string ext = path.substr(index, len - index);
    const char* cExt = ext.c_str();
    const unsigned int iExt = *(unsigned int*)cExt;

    unsigned char* data = nullptr;

    LayerMeta* layerMeta = new LayerMeta();
    layerMeta->Name = nullptr;

    // .png
    if (iExt == 0x676E702E)
    {
        int channels;

        data = stbi_load(a_path, &layerMeta->Width, &layerMeta->Height, &channels, STBI_rgb_alpha);

        const size_t fileNameIndex = path.find_last_of('/');
        const std::string filename = path.substr(fileNameIndex + 1, index - fileNameIndex - 1);

        const size_t fileNameLen = filename.length(); 

        layerMeta->Name = new char[fileNameLen];
        strcpy(layerMeta->Name, filename.c_str());
    }

    if (data == nullptr)
    {
        if (layerMeta->Name != nullptr)
        {
            delete[] layerMeta->Name;
        }

        delete layerMeta;
    }

    LayerTexture layerTexture;
    layerTexture.Data = data;
    layerTexture.Meta = layerMeta;

    GenerateTexture(layerTexture);

    m_layers->emplace_back(layerTexture);
}

void ModelEditor::Update(double a_delta)
{
    if (ImGui::Begin("Editor Preview"))
    {
        int texture = 0;
        if (m_selectedIndex != -1)
        {
            LayerTexture layerTexture = m_layers->at(m_selectedIndex);
            texture = layerTexture.Handle;
        }
        
        ImGui::Image((ImTextureID)texture, { 512, 512 });
    }
    ImGui::End();

    if (ImGui::Begin("Editor Layers"))
    {
        ImGui::BeginChild("Layer Scroll");
        for (int i = 0; i < m_layers->size(); ++i)
        {
            LayerMeta* layerMeta = m_layers->at(i).Meta;

            if (ImGui::Button(layerMeta->Name, { 200, 20 }))
            {
                m_selectedIndex = i;
            }
        }
        ImGui::EndChild();
    }
    ImGui::End();
}

void ModelEditor::GetImageData(PropertyFileProperty& a_property, ZipArchive::Ptr& a_archive)
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

    m_layers->emplace_back(layerTexture);
}

unsigned int ModelEditor::GetLayerCount() const
{
    return m_layers->size();
}

LayerMeta ModelEditor::GetLayerMeta(unsigned int a_index) const
{
    return *m_layers->at(a_index).Meta;
}

ModelEditor* ModelEditor::Load(ZipArchive::Ptr& a_archive)
{
    ModelEditor* modelEditor = new ModelEditor();

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
                modelEditor->GetImageData(*prop, a_archive);
            }            
        }

        delete propertiesFile;
        delete[] propertiesData;
    }

    return modelEditor;
}
std::istream* ModelEditor::SaveToStream() const
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
std::list<ModelFile> ModelEditor::SaveLayer(unsigned int a_index) const
{
    std::list<ModelFile> outputStreams;
    
    const LayerTexture layerTexture = m_layers->at(a_index);
    const size_t size = layerTexture.Meta->Width * layerTexture.Meta->Height * 4;

    IMemoryStream* memoryStream = new IMemoryStream((char*)layerTexture.Data, size);
    outputStreams.emplace_back(ModelFile{ e_ModelType::Image, memoryStream });

    return outputStreams;
}