#include "WindowControls/TextureEditorWindow.h"

#include "imgui.h"
#include "TextureEditor.h"

const char* TextureEditorWindow::ITEMS[] = { "Alpha", "Quad", "Outline" };

TextureEditorWindow::TextureEditorWindow(TextureEditor* a_textureEditor)
{
    m_textureEditor = a_textureEditor;

    m_channelDiff = 0.1f;

    m_triangulationMode = e_TriangulationMode::Quad;

    m_alphaThreshold = 0.9f;

    m_selectedMode = ITEMS[1];

    m_stepQuad = { 64, 64 };
    m_vSize = { 512, 512 };
    m_texStep = { 4, 4 };
}
TextureEditorWindow::~TextureEditorWindow()
{

}

void TextureEditorWindow::Update()
{
    ImGui::SetNextWindowSize({ 250, 600 }, ImGuiCond_Appearing);
    if (ImGui::Begin("Texture List"))
    {
        ImGui::BeginChild("Layer Scroll");
        
        m_textureEditor->DrawLayerGUI();

        ImGui::EndChild();
    }
    ImGui::End();
}
void TextureEditorWindow::UpdatePropertiesWindow()
{
    ImGui::InputInt2("Voronoi Size", (int*)&m_vSize);
    m_vSize.x = glm::max(1, m_vSize.x);
    m_vSize.y = glm::max(1, m_vSize.y);

    ImGui::InputInt2("Texture Step", (int*)&m_texStep);
    m_texStep.x = glm::max(1, m_texStep.x);
    m_texStep.y = glm::max(1, m_texStep.y);

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
        ImGui::InputInt2("Step Quad", (int*)&m_stepQuad);

        m_stepQuad.x = glm::max(0, m_stepQuad.x);
        m_stepQuad.y = glm::max(0, m_stepQuad.y);

        break;
    }
    }

    if (ImGui::Button("Triangulate", { 200, 20 }))
    {
        m_textureEditor->TriangulateClicked();
    }
}

e_TriangulationMode TextureEditorWindow::GetTriangulationMode() const
{
    return m_triangulationMode;
}

glm::ivec2 TextureEditorWindow::GetVoronoiSize() const
{
    return m_vSize;
}
glm::ivec2 TextureEditorWindow::GetTextureStep() const
{
    return m_texStep;
}

float TextureEditorWindow::GetChannelDifference() const
{
    return m_channelDiff;
}

float TextureEditorWindow::GetAlphaThreshold() const
{
    return m_alphaThreshold;
}

glm::ivec2 TextureEditorWindow::GetQuadStep() const
{
    return m_stepQuad;
}