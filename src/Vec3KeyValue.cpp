#include "KeyValues/Vec3KeyValue.h"

#include "FileUtils.h"

#include <string>
#include <string.h>

Vec3KeyValue::Vec3KeyValue() :
    KeyValue()
{
    m_baseValue = glm::vec3(0);
    m_value = glm::vec3(0);
}
Vec3KeyValue::Vec3KeyValue(const Vec3KeyValue& a_other)
{
    m_baseValue = a_other.m_baseValue;
    m_value = a_other.m_value;
}

Vec3KeyValue& Vec3KeyValue::operator =(const Vec3KeyValue& a_other)
{
    m_baseValue = a_other.m_baseValue;
    m_value = a_other.m_value;

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
        m_value = glm::mix(prevValue->m_baseValue, m_baseValue, a_shift);

        return;
    }

    m_value = m_baseValue;
}

char* Vec3KeyValue::ToString() const
{
    std::string str;

    str += " { ";
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