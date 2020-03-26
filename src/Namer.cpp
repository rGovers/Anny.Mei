#include "Namer.h"

#include <string>
#include <string.h>

#include "Name.h"

Namer::Namer()
{
    m_objectNames = new std::map<std::string, ID>();
    m_nameList = new std::list<const char*>();
}
Namer::~Namer()
{
    delete m_objectNames;
    delete m_nameList;
}

char* Namer::CreateName(ID& a_id, const char* a_trueName)
{
    const std::string str = "[" + std::to_string(a_id.ID) + "]";

    const size_t len = strlen(a_trueName);
    const size_t sLen = str.length();

    size_t cLen = len + sLen;

    char* name = new char[cLen + 1];
    strcpy(name, a_trueName);
    strcpy(name + len, str.c_str());
 
    ++a_id.ID;

    for (auto iter = m_nameList->begin(); iter != m_nameList->end(); ++iter)
    {
        if (strcmp(name, *iter) == 0)
        {
            delete[] name;

            return CreateName(a_id, a_trueName);

            break;
        }
    }

    return name;
}

void Namer::RemoveName(const Name& a_name)
{
    const char* name = a_name.GetName();
    const char* trueName = a_name.GetTrueName();

    RemoveUniqueName(name);

    auto iter = m_objectNames->find(trueName);
    if (iter != m_objectNames->end())
    {
        if (--iter->second.Objects <= 0)
        {
            m_objectNames->erase(iter);
        }
    }
}
void Namer::ClearName(const Name& a_name)
{
    const char* name = a_name.GetName();
    const char* trueName = a_name.GetTrueName();

    RemoveUniqueName(name);

    m_objectNames->erase(trueName);
}

void Namer::SetUniqueName(Name& a_name)
{
    const char* trueName = a_name.GetTrueName();

    char* name = nullptr;

    auto iter = m_objectNames->find(trueName);
    if (iter != m_objectNames->end())
    {
        name = CreateName(iter->second, trueName);
        ++iter->second.Objects;
    }
    else
    {
        const size_t len = strlen(trueName);

        name = new char[len + 1];
        strcpy(name, trueName);

        m_objectNames->emplace(trueName, ID { 1, 1 });
    }

    m_nameList->emplace_back(name);
    a_name.SetName(name, false);
}

void Namer::RemoveUniqueName(const char* a_name)
{
    for (auto iter = m_nameList->begin(); iter != m_nameList->end(); ++iter)
    {
        if (strcmp(a_name, *iter) == 0)
        {
            m_nameList->erase(iter);

			return;
        }
    }
}
void Namer::AddUniqueName(const char* a_name)
{
    for (auto iter = m_nameList->begin(); iter != m_nameList->end(); ++iter)
    {
        if (strcmp(*iter, a_name) == 0)
        {
            return;
        }
    }

    m_nameList->emplace_back(a_name);
}