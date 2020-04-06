#include "KeyValues/Vec2KeyValue.h"

#include <string>
#include <string.h>

#include "FileUtils.h"
#include "imgui.h"

Vec2KeyValue::Vec2KeyValue() :
    KeyValue()
{
    m_baseValue = glm::vec2(0);

    m_min = glm::vec2(-std::numeric_limits<float>::infinity());
    m_max = glm::vec2(std::numeric_limits<float>::infinity());

    SetDisplayLerp(true);
    SetDisplaySlerp(false);
    SetDisplaySet(true);
}
Vec2KeyValue::Vec2KeyValue(const Vec2KeyValue& a_other)
{
    m_baseValue = a_other.m_baseValue;

    m_min = a_other.m_min;
    m_max = a_other.m_max;

    SetDisplayLerp(a_other.GetDisplayLerp());
    SetDisplaySlerp(a_other.GetDisplaySlerp());
    SetDisplaySet(a_other.GetDisplaySet());
}

Vec2KeyValue& Vec2KeyValue::operator =(const Vec2KeyValue& a_other)
{
    m_baseValue = a_other.m_baseValue;

    m_min = a_other.m_min;
    m_max = a_other.m_max;

    SetDisplayLerp(a_other.GetDisplayLerp());
    SetDisplaySlerp(a_other.GetDisplaySlerp());
    SetDisplaySet(a_other.GetDisplaySet());

    return *this;
}

glm::vec2 Vec2KeyValue::GetBaseValue() const
{
    return m_baseValue;
}
glm::vec2 Vec2KeyValue::GetValue() const
{
    return m_value;
}

void Vec2KeyValue::SetBaseValue(const glm::vec2& a_value)
{
    m_baseValue = a_value;
}

void Vec2KeyValue::UpdateValue(double a_shift)
{
    Vec2KeyValue* prevValue = (Vec2KeyValue*)GetPrevKeyValue();

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

void Vec2KeyValue::SetMinValue(const glm::vec2& a_value)
{
    m_min = a_value;
}
glm::vec2 Vec2KeyValue::GetMinValue() const
{
    return m_min;
}

void Vec2KeyValue::SetMaxValue(const glm::vec2& a_value)
{
    m_max = a_value;
}
glm::vec2 Vec2KeyValue::GetMaxValue() const
{
    return m_max;
}

void Vec2KeyValue::UpdateGUI(const char* a_label, bool a_showLerpMode)
{
    ImGui::DragFloat2(a_label, (float*)&m_baseValue, 0.1f, 0.0f, 0.0f, "%.6f");

    if (a_showLerpMode)
    {
        ImGui::SameLine();

        UpdateLerpGUI();
    }

    m_baseValue = glm::clamp(m_baseValue, m_min, m_max);
}   

char* Vec2KeyValue::ToString() const
{
    std::string str;

    str += " { ";
    str += std::to_string(m_baseValue.x) + ", ";
    str += std::to_string(m_baseValue.y) + " }";
 
    const size_t len = str.length();
    
    char* cStr = new char[len + 1];
    strcpy(cStr, str.c_str());

    return cStr;
}
void Vec2KeyValue::Parse(const char* a_data)
{
    STR2V2(a_data, m_baseValue);
    m_value = m_baseValue;
}