#include "ModelEditor.h"

#include <glad/glad.h>
#include <stb/stb_image.h>

#include "FileLoaders/KritaLoader.h"
#include "FileLoaders/PSDLoader.h"
#include "MemoryStream.h"
#include "imgui.h"

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
            delete[] iter->Data;
        }
    }
    delete m_layers;
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

    glGenTextures(1, &layerTexture.Handle);
    glBindTexture(GL_TEXTURE_2D, layerTexture.Handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, layerMeta->Width, layerMeta->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

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

std::istream* ModelEditor::SaveToStream() const
{
    // IMemoryStream memoryStream = new IMemoryStream();
}