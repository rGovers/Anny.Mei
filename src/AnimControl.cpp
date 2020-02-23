#include "AnimControl.h"

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

    for (auto iter = m_animatedObjects.begin(); iter != m_animatedObjects.end(); ++iter)
    {
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