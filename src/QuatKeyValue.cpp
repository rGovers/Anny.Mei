#include "KeyValues/QuatKeyValue.h"

#include "FileUtils.h"

#include <string>
#include <string.h>

QuatKeyValue::QuatKeyValue() : 
    KeyValue()
{
    m_baseValue = glm::identity<glm::quat>();
    m_value = glm::identity<glm::quat>();
}
QuatKeyValue::QuatKeyValue(const QuatKeyValue& a_other)
{
    m_baseValue = a_other.m_baseValue;
    m_value = a_other.m_value;
}

QuatKeyValue& QuatKeyValue::operator =(const QuatKeyValue& a_other)
{
    m_baseValue = a_other.m_baseValue;
    m_value = a_other.m_value;

    return *this;
}

glm::quat QuatKeyValue::GetBaseValue() const
{
    return m_baseValue;
}
glm::quat QuatKeyValue::GetValue() const
{
    return m_value;
}

void QuatKeyValue::SetBaseValue(const glm::quat& a_value)
{
    m_baseValue = a_value;
}

void QuatKeyValue::UpdateValue(double a_shift)
{
    QuatKeyValue* prevValue = (QuatKeyValue*)GetPrevKeyValue();

    if (prevValue != nullptr)
    {
        m_value = glm::mix(prevValue->m_baseValue, m_baseValue, (float)a_shift);

        return;
    }

    m_value = m_baseValue;
}

char* QuatKeyValue::ToString() const
{
    std::string str;

    str += "{ ";
    str += std::to_string(m_baseValue.x) + ", ";
    str += std::to_string(m_baseValue.y) + ", ";
    str += std::to_string(m_baseValue.z) + ", ";
    str += std::to_string(m_baseValue.w) + " }";
 
    const size_t len = str.length();
    
    char* cStr = new char[len + 1];
    strcpy(cStr, str.c_str());

    return cStr;
}
void QuatKeyValue::Parse(const char* a_data)
{
    STR2V4(a_data, m_baseValue);
    m_value = m_baseValue;
}