#include "ModelEditor.h"

#include "imgui.h"
#include "FileLoaders/KritaLoader.h"
#include "FileLoaders/PSDLoader.h"

ModelEditor::ModelEditor() : 
    m_selectedIndex(-1)
{
    m_layers = new std::vector<LayerTexture>();
}

void ModelEditor::LoadTexture(const char* a_path)
{
    std::string path = a_path;
    size_t index = path.find_last_of('.');
    size_t len = path.length();

    std::string ext = path.substr(index, len - index);
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

            delete layerMeta;
        }
        ImGui::EndChild();
    }
    ImGui::End();
}

ModelEditor::~ModelEditor()
{
    for (auto iter = m_layers->begin(); iter != m_layers->end(); ++iter)
    {
        if (iter->Meta != nullptr)
        {
            delete iter->Meta;
        }
    }
    delete m_layers;
}

void ModelEditor::SaveToArchive(ZipArchive::Ptr& a_archive) const
{
    
}