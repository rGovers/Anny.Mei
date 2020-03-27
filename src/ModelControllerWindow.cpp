#include "WindowControls/ModelControllerWindow.h"

#include "Application.h"
#include "imgui.h"
#include "ModelController.h"
#include "Texture.h"

ModelControllerWindow::ModelControllerWindow(ModelController* a_modelController)
{
    m_modelController = a_modelController;

    m_backgroundColor = glm::vec4(0, 1, 0, 1);
}
ModelControllerWindow::~ModelControllerWindow()
{

}

void ModelControllerWindow::Update()
{
    Application* app = Application::GetInstance();

    ImGui::SetNextWindowSize({ 660, 400 }, ImGuiCond_Appearing);
    if (ImGui::Begin("Preview"))
    {
        const Texture* tex = m_modelController->GetPreviewTexture();
        const ImVec2 size = ImGui::GetWindowSize();

        const glm::vec2 sScale = { size.x / 16, size.y / 9 };

        glm::vec2 fSize;

        if (sScale.x < sScale.y)
        {
            const float xSize = size.x - 20;

            fSize = { xSize, xSize * 0.5625f };
        }
        else
        {
            const float ySize = size.y - 40;

            fSize = { ySize * 1.77777777778f, ySize };
        }
        

        ImGui::Image((ImTextureID)tex->GetHandle(), { fSize.x, fSize.y });
    }
    ImGui::End();

    ImGui::SetNextWindowSize({ 200, 300 }, ImGuiCond_Appearing);
    if (ImGui::Begin("Options"))
    {
        ImGui::ColorPicker4("Background Color", (float*)&m_backgroundColor);
    }
    ImGui::End();
}

glm::vec4 ModelControllerWindow::GetBackgroundColor() const
{
    return m_backgroundColor;
}
void ModelControllerWindow::SetBackgroundColor(const glm::vec4& a_color)
{
    m_backgroundColor = a_color;
}