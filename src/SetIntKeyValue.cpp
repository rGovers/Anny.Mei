#include "KeyValues/SetIntKeyValue.h"

#include <string>
#include <string.h>

SetIntKeyValue::SetIntKeyValue() :
    KeyValue()
{
    m_value = 0;
}
SetIntKeyValue::SetIntKeyValue(const SetIntKeyValue& a_other)
{
    m_value = a_other.m_value;
}
SetIntKeyValue::~SetIntKeyValue()
{

}

SetIntKeyValue& SetIntKeyValue::operator =(const SetIntKeyValue& a_other)
{
    m_value = a_other.m_value;

    return *this;
}

int SetIntKeyValue::GetBaseInt() const
{
    return m_value;
}
int SetIntKeyValue::GetInt() const
{
    SetIntKeyValue* prevValue = (SetIntKeyValue*)GetPrevKeyValue();

    if (prevValue != nullptr)
    {
        return prevValue->m_value;
    }

    return m_value;
}

void SetIntKeyValue::SetInt(int a_value)
{
    m_value = a_value;
}

void SetIntKeyValue::UpdateValue(double a_shift)
{

}

char* SetIntKeyValue::ToString() const
{
    std::string str = std::to_string(m_value);
 
    const size_t len = str.length();
    
    char* cStr = new char[len + 1];
    strcpy(cStr, str.c_str());

    return cStr;
}
void SetIntKeyValue::Parse(const char* a_data)
{
    m_value = std::stoi(a_data);
}