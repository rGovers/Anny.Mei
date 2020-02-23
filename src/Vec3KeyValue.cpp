#include "KeyValues/Vec3KeyValue.h"

#include "AnimControl.h"

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