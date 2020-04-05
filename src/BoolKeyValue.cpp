#include "KeyValues/BoolKeyValue.h"

#include <string.h>

#include "imgui.h"

BoolKeyValue::BoolKeyValue() :
    KeyValue()
{
    m_value = false;
}
BoolKeyValue::BoolKeyValue(const BoolKeyValue& a_other)
{
    m_value = a_other.m_value;
}
BoolKeyValue::~BoolKeyValue()
{

}

BoolKeyValue& BoolKeyValue::operator =(const BoolKeyValue& a_other)
{
    m_value = a_other.m_value;

    return *this;
}

bool BoolKeyValue::GetBaseBoolean() const
{
    return m_value;
}
bool BoolKeyValue::GetBoolean() const
{
    const BoolKeyValue* prevValue = (BoolKeyValue*)GetPrevKeyValue();

    if (prevValue != nullptr)
    {
        return prevValue->m_value;
    }

    return m_value;
}

void BoolKeyValue::SetBoolean(bool a_value)
{
    m_value = a_value;
}

void BoolKeyValue::UpdateValue(double a_shift)
{

}

void BoolKeyValue::UpdateGUI(const char* a_label, bool a_showLerpMode)
{
    ImGui::Checkbox(a_label, &m_value);
}

char* BoolKeyValue::ToString() const
{
    char* data = new char[6];

    if (m_value)
    {
        strcpy(data, "true");
    }
    else
    {
        strcpy(data, "false");
    }
    
    return data;
}
void BoolKeyValue::Parse(const char* a_data)
{
    if (strcmp(a_data, "true") == 0 || a_data[0] == '1')
    {
        m_value = true;
    }
    else
    {
        m_value = false;
    }
}
