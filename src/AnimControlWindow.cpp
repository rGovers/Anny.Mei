#include "WindowControls/AnimControlWindow.h"

#include <glm/glm.hpp>

#include "AnimControl.h"
#include "imgui.h"
#include "Texture.h"

AnimControlWindow::AnimControlWindow(AnimControl* a_animControl)
{
    m_animControl = a_animControl;

    m_maxValue = 1;
    m_selectedTime = 0;
}
AnimControlWindow::~AnimControlWindow()
{

}

void AnimControlWindow::Update()
{
    ImGui::SetNextWindowSize({ 400, 100 }, ImGuiCond_Appearing);
    if (ImGui::Begin("Anim Key Frames"))
    {
        ImGui::InputFloat("Max Time", &m_maxValue);
        ImGui::DragFloat("Selected Time", &m_selectedTime, 0.1f, 0.0f, m_maxValue);

        int height = -1;
        ImGui::Spacing();
        
        const Texture* tex = m_animControl->DrawTimeline(height);

        ImGui::Image((ImTextureID)tex->GetHandle(), { m_maxValue * 500, height * ImGui::GetFrameHeightWithSpacing() });

        m_selectedTime = glm::clamp(m_selectedTime, 0.0f, m_maxValue);
    }
    ImGui::End();
}

float AnimControlWindow::GetMaxTimeValue() const
{
    return m_maxValue;
}
void AnimControlWindow::SetMaxTimeValue(float a_value) 
{
    m_maxValue = a_value;
}

float AnimControlWindow::GetSelectedTime() const
{
    return m_selectedTime;
}