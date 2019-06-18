#include "ModelEditor.h"

#include "imgui.h"
#include "FileLoaders/KritaLoader.h"
#include "FileLoaders/PSDLoader.h"

ModelEditor::ModelEditor(const char* a_path) :
    m_selectedIndex(-1)
{
    // m_imageLoader = new KritaLoader(a_path);

    m_imageLoader = new PSDLoader(a_path);

    const int layerCount = m_imageLoader->GetLayerCount();

    m_layerMeta = new std::list<LayerMeta*>(layerCount);
    m_layers = new std::vector<LayerTexture>(layerCount);

    int index = 0;
    for (auto iter = m_layerMeta->begin(); iter != m_layerMeta->end(); ++iter)
    {
        *iter = m_imageLoader->GetLayerMeta(index);
        (*m_layers)[index].LayerData = nullptr;
        (*m_layers)[index++].Handle = -1;
    }
}

void ModelEditor::Update(double a_delta)
{
    const int layerCount = m_imageLoader->GetLayerCount();

    if (ImGui::Begin("Editor"))
    {
        ImGui::BeginGroup();
        ImGui::TextColored({1, 0, 0, 1}, "Preview");        

        int texture = 0;
        if (m_selectedIndex != -1)
        {
            LayerTexture layerTexture = m_layers->at(m_selectedIndex);
            
            if (layerTexture.Handle == -1)
            {
                layerTexture.LayerData = m_imageLoader->GetLayer(m_selectedIndex);
            }

        }
        
        ImGui::Image((ImTextureID)texture, { 512, 512 });

        ImGui::EndGroup();

        ImGui::SameLine();

        ImGui::BeginGroup();
        ImGui::TextColored({ 1, 0, 0, 1}, "Layers");

        ImGui::BeginChild("Layer Scroll");
        for (int i = 0; i < layerCount; ++i)
        {
            LayerMeta* layerMeta = m_imageLoader->GetLayerMeta(i);

            if (ImGui::Button(layerMeta->Name, { 200, 20 }))
            {
                m_selectedIndex = i;
            }

            delete layerMeta;
        }
        ImGui::EndChild();
        ImGui::EndGroup();
    }
    ImGui::End();
}

ModelEditor::~ModelEditor()
{
    delete m_imageLoader;

    for (auto iter = m_layerMeta->begin(); iter != m_layerMeta->end(); ++iter)
    {
        delete *iter;
    }
    delete m_layerMeta;

    for (auto iter = m_layers->begin(); iter != m_layers->end(); ++iter)
    {
        if (iter->LayerData != nullptr)
        {
            delete iter->LayerData;
        }
    }
    delete m_layers;
}