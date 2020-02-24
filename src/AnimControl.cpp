#include "AnimControl.h"

#include <glm/glm.hpp>

#include "AnimValue.h"
#include "imgui.h"
#include "Object.h"

AnimControl::AnimControl()
{
    m_timer = 0;
    m_maxValue = 1;
}
AnimControl::~AnimControl()
{
    
}

void AnimControl::Update(double a_delta)
{
    if (m_timer >= m_maxValue)
    {
        m_timer -= m_maxValue;
    }

    m_timer += a_delta;

    ImGui::SetNextWindowSize({ 400, 100 }, ImGuiCond_Appearing);
    if (ImGui::Begin("Anim Key Frames"))
    {
        float maxValue = (float)m_maxValue;
        float selectedTime = (float)m_selectedTime;
        
        ImGui::InputFloat("Max Time", &maxValue);
        ImGui::DragFloat("Selected Time", &selectedTime, 0.1f, 0.0f, maxValue);

        m_maxValue = maxValue;
        m_selectedTime = glm::clamp(selectedTime, 0.0f, maxValue);
    }
    ImGui::End();

    for (auto iter = m_animatedObjects.begin(); iter != m_animatedObjects.end(); ++iter)
    {
        (*iter)->SelectKeyFrame(m_selectedTime);
        (*iter)->UpdateAnimValue(m_timer);
    }
}

void AnimControl::AddValue(AValue* a_value)
{
    m_animatedObjects.emplace_back(a_value);
}
void AnimControl::RemoveValue(AValue* a_value)
{
    for (auto iter = m_animatedObjects.begin(); iter != m_animatedObjects.end(); ++iter)
    {
        if (*iter == a_value)
        {
            m_animatedObjects.erase(iter);

            return;
        }
    }
}