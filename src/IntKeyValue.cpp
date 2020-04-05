#include "KeyValues/IntKeyValue.h"

#include <glm/glm.hpp>
#include <string>
#include <string.h>

#include "imgui.h"

IntKeyValue::IntKeyValue() :
    KeyValue()
{
    m_baseValue = 0;

    m_min = std::numeric_limits<int>::min();
    m_max = std::numeric_limits<int>::max();

    SetDisplayLerp(true);
    SetDisplaySlerp(false);
    SetDisplaySet(true);
}
IntKeyValue::IntKeyValue(const IntKeyValue& a_other)
{
    m_baseValue = a_other.m_baseValue;

    SetDisplayLerp(a_other.GetDisplayLerp());
    SetDisplaySlerp(a_other.GetDisplaySlerp());
    SetDisplaySet(a_other.GetDisplaySet());
}
IntKeyValue::~IntKeyValue()
{

}

IntKeyValue& IntKeyValue::operator =(const IntKeyValue& a_other)
{
    m_baseValue = a_other.m_baseValue;

    SetDisplayLerp(a_other.GetDisplayLerp());
    SetDisplaySlerp(a_other.GetDisplaySlerp());
    SetDisplaySet(a_other.GetDisplaySet());

    return *this;
}

int IntKeyValue::GetBaseValue() const
{
    return m_baseValue;
}
int IntKeyValue::GetValue() const
{
    return m_value;
}

void IntKeyValue::SetBaseValue(int a_value)
{
    m_baseValue = a_value;
}

void IntKeyValue::UpdateValue(double a_shift)
{
    IntKeyValue* prevValue = (IntKeyValue*)GetPrevKeyValue();
    
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

void IntKeyValue::SetMinLimit(int a_value)
{
    m_min = a_value;
}
int IntKeyValue::GetMinLimit() const
{
    return m_min;
}

void IntKeyValue::SetMaxLimit(int a_value)
{
    m_max = a_value;
}
int IntKeyValue::GetMaxLimit() const
{
    return m_max;
}

void IntKeyValue::UpdateGUI(const char* a_label, bool a_showLerpMode)
{
    ImGui::DragInt(a_label, &m_baseValue);

    if (a_showLerpMode)
    {
        ImGui::SameLine();

        UpdateLerpGUI();
    }

    m_baseValue = glm::clamp(m_baseValue, m_min, m_max);
}

char* IntKeyValue::ToString() const
{
    std::string str = std::to_string(m_baseValue);
 
    const size_t len = str.length();
    
    char* cStr = new char[len + 1];
    strcpy(cStr, str.c_str());

    return cStr;
}
void IntKeyValue::Parse(const char* a_data)
{
    m_baseValue = std::stoi(a_data);
}