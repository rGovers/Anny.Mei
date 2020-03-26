#include "FileLoaders/PropertyFile.h"

#include <string.h>

PropertyFileProperty::PropertyFileProperty()
{
    m_name = nullptr;
    m_parent = nullptr;
}
PropertyFileProperty::~PropertyFileProperty()
{
    delete[] m_name;

    for (auto iter = m_values.begin(); iter != m_values.end(); ++iter)
    {
        delete[] iter->Name;
        delete[] iter->Value;
    }

    for (auto iter = m_children.begin(); iter != m_children.end(); ++iter)
    {
        (*iter)->m_parent = m_parent;

        if (m_parent != nullptr)
        {
            m_parent->m_children.emplace_back(*iter);
        }
    }

    if (m_parent != nullptr)
    {
        m_parent->m_children.remove(this);
    }
}

const char* PropertyFileProperty::GetName() const
{
    return m_name;
}
void PropertyFileProperty::SetName(const char* a_name)
{
    if (m_name != nullptr)
    {
        delete[] m_name;
    }

    const size_t len = strlen(a_name);

    m_name = new char[len + 1];

    strcpy(m_name, a_name);
}

std::list<PropertyFileValue>& PropertyFileProperty::Values()
{
    return m_values;
}

void PropertyFileProperty::EmplaceValue(const char* a_name, const char* a_value)
{
    PropertyFileValue value;

    const size_t nameLen = strlen(a_name);
    const size_t valLen = strlen(a_value);

    value.Name = new char[nameLen + 1];
    value.Value = new char[valLen + 1];

    strcpy(value.Name, a_name);
    strcpy(value.Value, a_value);

    m_values.emplace_back(value);
}

void PropertyFileProperty::SetParent(PropertyFileProperty* a_parent)
{
    if (m_parent != nullptr)
    {
        m_parent->m_children.remove(this);
    }

    m_parent = a_parent;

    if (m_parent != nullptr)
    {
        m_parent->m_children.emplace_back(this);
    }
}
PropertyFileProperty* PropertyFileProperty::GetParent() const
{
    return m_parent;
}

std::list<PropertyFileProperty*> PropertyFileProperty::GetChildren() const
{
    return m_children;
}