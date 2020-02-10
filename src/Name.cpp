#include "Name.h"

#include <string.h>

#include "Namer.h"

Name::Name(const char* a_trueName, Namer* a_namer)
{
    m_namer = a_namer;

    m_name = nullptr;
    m_trueName = nullptr;

    SetTrueName(a_trueName);
}

Name::~Name()
{
    m_namer->RemoveName(*this);

    if (m_name != nullptr)
    {
        delete[] m_name;
    }

    if (m_trueName != nullptr)
    {
        delete[] m_trueName;
    }
}

void Name::SetTrueName(const char* a_name)
{
    if (m_trueName != nullptr && a_name != nullptr && strcmp(a_name, m_trueName) == 0)
    {
        return;
    }
    else if (m_trueName == nullptr && a_name == nullptr)
    {
        return;
    }


    if (m_trueName != nullptr)
    {
        m_namer->RemoveName(*this);

        delete[] m_trueName;
        m_trueName = nullptr;
    }

    if (a_name != nullptr)
    {
        const size_t len = strlen(a_name);

        if (len > 0)
        {
            m_trueName = new char[len];
            strcpy(m_trueName, a_name);
        
            m_namer->SetUniqueName(*this);
        }
    }
}
const char* Name::GetTrueName() const
{
    return m_trueName;
}

void Name::SetName(const char* a_name, bool a_inc)
{
    if (m_name != nullptr)
    {
        if (strcmp(a_name, m_name) == 0)
        {
            return;
        }

        m_namer->RemoveUniqueName(m_name);

        delete[] m_name;
        m_name = nullptr;
    }

    if (a_name != nullptr)
    {
        const size_t len = strlen(a_name); 

        m_name = new char[len];

        strcpy(m_name, a_name);

        if (a_inc)
        {
            m_namer->AddUniqueName(m_name);
        }
    }
}
const char* Name::GetName() const
{
    return m_name;
}