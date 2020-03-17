#include "KeyValues/FloatKeyValue.h"

#include <glm/glm.hpp>
#include <string>
#include <string.h>

FloatKeyValue::FloatKeyValue() :
    KeyValue()
{
    m_baseValue = 0.0f;
    m_value = 0.0f;
}
FloatKeyValue::FloatKeyValue(const FloatKeyValue& a_other)
{
    m_baseValue = a_other.m_baseValue;
    m_value = a_other.m_value;
}

FloatKeyValue& FloatKeyValue::operator =(const FloatKeyValue& a_other)
{
    m_baseValue = a_other.m_baseValue;
    m_value = a_other.m_value;

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
        m_value = glm::mix(prevValue->m_baseValue, m_baseValue, a_shift);

        return;
    }

    m_value = m_baseValue;
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