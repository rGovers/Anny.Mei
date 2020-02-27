#include "KeyValues/StringKeyValue.h"

#include <string.h>

StringKeyValue::StringKeyValue() :
    KeyValue()
{
    m_value = nullptr;
}
StringKeyValue::StringKeyValue(const StringKeyValue& a_other)
{
    m_value = nullptr;

    if (a_other.m_value != nullptr)
    {
        const size_t len = strlen(a_other.m_value);

        m_value = new char[len + 1];

        strcpy(m_value, a_other.m_value);
    }
}
StringKeyValue::~StringKeyValue()
{
    if (m_value != nullptr)
    {
        delete[] m_value;
    }
}

StringKeyValue& StringKeyValue::operator =(const StringKeyValue& a_other)
{
    m_value = nullptr;

    if (a_other.m_value != nullptr)
    {
        const size_t len = strlen(a_other.m_value);

        m_value = new char[len + 1];

        strcpy(m_value, a_other.m_value);
    }

    return *this;
}

const char* StringKeyValue::GetBaseString() const
{
    return m_value;
}
const char* StringKeyValue::GetString() const
{
    StringKeyValue* prevValue = (StringKeyValue*)GetPrevKeyValue();

    if (prevValue != nullptr)
    {
        return prevValue->m_value;
    }

    return m_value;
}

void StringKeyValue::SetString(const char* a_value)
{
    if (m_value != nullptr)
    {
        delete[] m_value;
        m_value = nullptr;
    }

    if (a_value != nullptr)
    {
        const size_t size = strlen(a_value);

        m_value = new char[size + 1];

        strcpy(m_value, a_value);
    }
}

void StringKeyValue::UpdateValue(double a_shift)
{

}

char* StringKeyValue::ToString() const
{
    const size_t len = strlen(m_value);
    
    char* cStr = new char[len + 1];
    strcpy(cStr, m_value);

    return cStr;
}
void StringKeyValue::Parse(const char* a_data)
{
    const size_t len = strlen(a_data);
    m_value = new char[len + 1];

    strcpy(m_value, a_data);
}