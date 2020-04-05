#include "KeyValues/FloatKeyValue.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <limits>
#include <string>
#include <string.h>

#include "imgui.h"

FloatKeyValue::FloatKeyValue() :
    KeyValue()
{
    m_baseValue = 0.0f;
    m_value = 0.0f;

    m_min = -std::numeric_limits<float>::infinity();
    m_max = std::numeric_limits<float>::infinity();

    SetDisplayLerp(true);
    SetDisplaySlerp(false);
    SetDisplaySet(true);
}
FloatKeyValue::FloatKeyValue(const FloatKeyValue& a_other)
{
    m_baseValue = a_other.m_baseValue;
    m_value = a_other.m_value;

    m_min = a_other.m_min;
    m_max = a_other.m_max;

    SetDisplayLerp(a_other.GetDisplayLerp());
    SetDisplaySlerp(a_other.GetDisplaySlerp());
    SetDisplaySet(a_other.GetDisplaySet());
}

FloatKeyValue& FloatKeyValue::operator =(const FloatKeyValue& a_other)
{
    m_baseValue = a_other.m_baseValue;
    m_value = a_other.m_value;

    m_min = a_other.m_min;
    m_max = a_other.m_max;

    SetDisplayLerp(a_other.GetDisplayLerp());
    SetDisplaySlerp(a_other.GetDisplaySlerp());
    SetDisplaySet(a_other.GetDisplaySet());

    return *this;
}

float FloatKeyValue::GetBaseValue() const
{
    return m_baseValue;
}
float FloatKeyValue::GetValue() const
{
    return m_value;
}

void FloatKeyValue::SetBaseValue(float a_value)
{
    m_baseValue = a_value;
}

void FloatKeyValue::UpdateValue(double a_shift)
{
    FloatKeyValue* prevValue = (FloatKeyValue*)GetPrevKeyValue();
    
    if (prevValue != nullptr)
    {
        switch (GetLerpMode())
        {
        case e_LerpMode::Lerp:
        {
            m_value = glm::mix(prevValue->m_baseValue, m_baseValue, a_shift);

            break;
        }
        default:
        {
            m_value = prevValue->m_baseValue;

            break;
        }
        }

        return;
    }

    m_value = m_baseValue;
}

void FloatKeyValue::SetMinLimit(float a_value)
{
    m_min = a_value;
}
float FloatKeyValue::GetMinLimit() const
{
    return m_min;
}

void FloatKeyValue::SetMaxLimit(float a_value)
{
    m_max = a_value;
}
float FloatKeyValue::GetMaxLimit() const
{
    return m_max;
}

void FloatKeyValue::DisplayGUI(const char* a_label, bool a_showLerpMode)
{
    ImGui::DragFloat(a_label, &m_baseValue, 0.1f);

    if (a_showLerpMode)
    {
        ImGui::SameLine();

        UpdateLerpGUI();
    }

    m_baseValue = glm::clamp(m_baseValue, m_min, m_max);
}

char* FloatKeyValue::ToString() const
{
    const std::string str = std::to_string(m_baseValue);
 
    const size_t len = str.length();
    
    char* cStr = new char[len + 1];
    strcpy(cStr, str.c_str());

    return cStr;
}
void FloatKeyValue::Parse(const char* a_data)
{
    m_baseValue = std::stof(a_data);
}