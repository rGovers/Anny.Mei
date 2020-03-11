#include "FileLoaders/XMLFile.h"

#include <string.h>

XMLFileProperty::XMLFileProperty()
{
    m_parent = nullptr;

    m_name = nullptr;
    m_value = nullptr;
}
XMLFileProperty::~XMLFileProperty()
{
    if (m_name != nullptr)
    {
        delete[] m_name;
    }
    if (m_value != nullptr)
    {
        delete[] m_value;
    }

    for (auto iter = m_attributes.begin(); iter != m_attributes.end(); ++iter)
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

const char* XMLFileProperty::GetName() const
{
    return m_name;
}
void XMLFileProperty::SetName(const char* a_name)
{
    if (m_name != nullptr)
    {
        delete[] m_name;
        m_name = nullptr;
    }

    if (a_name != nullptr)
    {
        const size_t len = strlen(a_name);

        m_name = new char[len + 1];

        strcpy(m_name, a_name);
    }
}

std::list<XMLFileAttribute>& XMLFileProperty::Attributes()
{
    return m_attributes;
}

void XMLFileProperty::EmplaceAttribute(const char* a_name, const char* a_value)
{
    XMLFileAttribute att;

    const size_t nameLen = strlen(a_name);
    const size_t valLen = strlen(a_value);

    att.Name = new char[nameLen + 1];
    att.Value = new char[valLen + 1];

    strcpy(att.Name, a_name);
    strcpy(att.Value, a_value);

    m_attributes.emplace_back(att);
}

const char* XMLFileProperty::GetValue() const
{
    return m_value;
}
void XMLFileProperty::SetValue(const char* a_value)
{
    if (m_value != nullptr)
    {
        delete[] m_value;
        m_value = nullptr;
    }

    if (a_value != nullptr)
    {
        const size_t len = strlen(a_value);

        m_value = new char[len + 1];

        strcpy(m_value, a_value);
    }
}

void XMLFileProperty::SetParent(XMLFileProperty* a_parent)
{
    m_parent = a_parent;

    if (m_parent != nullptr)
    {
        m_parent->m_children.emplace_back(this);
    }
}
XMLFileProperty* XMLFileProperty::GetParent() const
{
    return m_parent;
}

std::list<XMLFileProperty*> XMLFileProperty::GetChildren() const
{
    return m_children;
}