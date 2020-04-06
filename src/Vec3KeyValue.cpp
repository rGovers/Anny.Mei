#include "KeyValues/Vec3KeyValue.h"

#include <glm/gtx/rotate_vector.hpp>
#include <string>
#include <string.h>

#include "FileUtils.h"
#include "imgui.h"

Vec3KeyValue::Vec3KeyValue() :
    KeyValue()
{
    m_baseValue = glm::vec3(0);

    m_min = glm::vec3(-std::numeric_limits<float>::infinity());
    m_max = glm::vec3(std::numeric_limits<float>::infinity());

    SetDisplayLerp(true);
    SetDisplaySlerp(true);
    SetDisplaySet(true);
}
Vec3KeyValue::Vec3KeyValue(const Vec3KeyValue& a_other)
{
    m_baseValue = a_other.m_baseValue;

    m_min = a_other.m_min;
    m_max = a_other.m_max;

    SetDisplayLerp(a_other.GetDisplayLerp());
    SetDisplaySlerp(a_other.GetDisplaySlerp());
    SetDisplaySet(a_other.GetDisplaySet());
}

Vec3KeyValue& Vec3KeyValue::operator =(const Vec3KeyValue& a_other)
{
    m_baseValue = a_other.m_baseValue;

    m_min = a_other.m_min;
    m_max = a_other.m_max;

    SetDisplayLerp(a_other.GetDisplayLerp());
    SetDisplaySlerp(a_other.GetDisplaySlerp());
    SetDisplaySet(a_other.GetDisplaySet());

    return *this;
}

glm::vec3 Vec3KeyValue::GetBaseValue() const
{
    return m_baseValue;
}
glm::vec3 Vec3KeyValue::GetValue() const
{
    return m_value;
}

void Vec3KeyValue::SetBaseValue(const glm::vec3& a_value)
{
    m_baseValue = a_value;
}

void Vec3KeyValue::UpdateValue(double a_shift)
{
    Vec3KeyValue* prevValue = (Vec3KeyValue*)GetPrevKeyValue();

    if (prevValue != nullptr)
    {
        switch (GetLerpMode())
        {
        case e_LerpMode::Lerp:
        {
            m_value = glm::mix(prevValue->m_baseValue, m_baseValue, a_shift);

            break;
        }
        case e_LerpMode::Slerp:
        {
            m_value = glm::slerp(prevValue->m_baseValue, m_baseValue, (float)a_shift);

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

void Vec3KeyValue::SetMinValue(const glm::vec3& a_value)
{
    m_min = a_value;
}
glm::vec3 Vec3KeyValue::GetMinValue() const
{
    return m_min;
}

void Vec3KeyValue::SetMaxValue(const glm::vec3& a_value)
{
    m_max = a_value;
}
glm::vec3 Vec3KeyValue::GetMaxValue() const
{
    return m_max;
}

void Vec3KeyValue::UpdateGUI(const char* a_label, bool a_showLerpMode)
{
    ImGui::DragFloat3(a_label, (float*)&m_baseValue, 0.01f, 0.0f, 0.0f, "%.6f");

    if (a_showLerpMode)
    {
        ImGui::SameLine();

        UpdateLerpGUI();
    }

    m_baseValue = glm::clamp(m_baseValue, m_min, m_max);
}

char* Vec3KeyValue::ToString() const
{
    std::string str;

    str += "{ ";
    str += std::to_string(m_baseValue.x) + ", ";
    str += std::to_string(m_baseValue.y) + ", ";
    str += std::to_string(m_baseValue.z) + " }";
 
    const size_t len = str.length();
    
    char* cStr = new char[len + 1];
    strcpy(cStr, str.c_str());

    return cStr;
}
void Vec3KeyValue::Parse(const char* a_data)
{
    STR2V3(a_data, m_baseValue);
    m_value = m_baseValue;
}