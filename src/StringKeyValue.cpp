#include "KeyValues/StringKeyValue.h"

#include <string.h>

#include "imgui.h"

const int BUFFER_SIZE = 1024;
static char* BUFFER = new char[BUFFER_SIZE];

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

void StringKeyValue::UpdateGUI(const char* a_label, bool a_showLerpMode)
{
    if (m_value != nullptr)
    {
        strcpy(BUFFER, m_value);
    }
    else
    {
        memset(BUFFER, 0, BUFFER_SIZE - 1);
    }
    
    ImGui::InputText(a_label, BUFFER, BUFFER_SIZE);

    if (m_value == nullptr)
    {
        const size_t len = strlen(BUFFER);

        m_value = new char[len + 1];
        strcpy(m_value, BUFFER);
    }
    else if (strcmp(BUFFER, m_value) != 0)
    {
        delete[] m_value;

        const size_t len = strlen(BUFFER);

        m_value = new char[len + 1];
        strcpy(m_value, BUFFER);   
    }
}

char* StringKeyValue::ToString() const
{
    if (m_value != nullptr)
    {
        const size_t len = strlen(m_value);
    
        char* cStr = new char[len + 1];
        strcpy(cStr, m_value);

        return cStr;
    }
    
    return new char[1] { 0 };
}
void StringKeyValue::Parse(const char* a_data)
{
    const size_t len = strlen(a_data);
    m_value = new char[len + 1];

    strcpy(m_value, a_data);
}