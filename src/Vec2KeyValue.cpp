#include "KeyValues/Vec2KeyValue.h"

#include "FileUtils.h"

#include <string>
#include <string.h>

Vec2KeyValue::Vec2KeyValue() :
    KeyValue()
{
    m_baseValue = glm::vec2(0);
    m_value = glm::vec2(0);
}
Vec2KeyValue::Vec2KeyValue(const Vec2KeyValue& a_other)
{
    m_baseValue = a_other.m_baseValue;
    m_value = a_other.m_value;
}

Vec2KeyValue& Vec2KeyValue::operator =(const Vec2KeyValue& a_other)
{
    m_baseValue = a_other.m_baseValue;
    m_value = a_other.m_value;

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
        m_value = glm::mix(prevValue->m_baseValue, m_baseValue, a_shift);

        return;
    }

    m_value = m_baseValue;
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