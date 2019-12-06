#include "Object.h"

#include <algorithm>
#include <iostream>
#include <string.h>

#include "Transform.h"

std::map<std::string, Object::ID>* Object::OBJECT_NAMES = nullptr;

Object::Object() : 
    m_parent(nullptr),
    m_transform(new Transform()),
    m_name(nullptr),
    m_trueName(nullptr)
{
    if (OBJECT_NAMES == nullptr)
    {
        OBJECT_NAMES = new std::map<std::string, ID>();
    }

    SetTrueName("Object");
}
Object::~Object()
{
    delete m_transform;

    auto iter = OBJECT_NAMES->find(m_trueName);
    
    if (iter != OBJECT_NAMES->end())
    {
        if (--iter->second.Objects <= 0)
        {
            OBJECT_NAMES->erase(iter);
        }
    }

    if (m_parent != nullptr)
    {
        m_parent->m_children.remove(this);
    }

    for (auto iter = m_children.begin(); iter != m_children.end(); ++iter)
    {
        (*iter)->m_parent = nullptr;

        delete *iter;
    }
}

Transform* Object::GetTransform() const
{
    return m_transform;
}

Object* Object::GetParent() const
{
    return m_parent;
}
void Object::SetParent(Object* a_parent)
{
    if (m_parent == a_parent)
    {
        return;
    }

    if (m_parent != nullptr)
    {
        m_parent->m_children.remove(this);
    }

    m_transform->SetParent(a_parent->m_transform);

    m_parent = a_parent;

    a_parent->m_children.emplace_back(this);
}

std::list<Object*> Object::GetChildren() const
{
    return m_children;
}

void Object::SetTrueName(const char* a_trueName)
{
    if (m_trueName != nullptr)
    {
        if (strcmp(a_trueName, m_trueName) == 0)
        {
            return;
        }

        auto iter = OBJECT_NAMES->find(m_trueName);

        if (iter != OBJECT_NAMES->end())
        {
            if (--iter->second.Objects <= 0)
            {
                OBJECT_NAMES->erase(iter);
            }
        }

        delete[] m_trueName;
        m_trueName = nullptr;
    }
    if (m_name != nullptr)
    {
        delete m_name;
        m_name = nullptr;
    }

    if (a_trueName != nullptr)
    {
        const size_t len = strlen(a_trueName);

        if (len > 0)
        {
            m_trueName = new char[len];
            strcpy(m_trueName, a_trueName);
        
            auto iter = OBJECT_NAMES->find(m_trueName);
            std::cout << std::to_string(OBJECT_NAMES->size()) << std::endl;

            if (iter == OBJECT_NAMES->end())
            {
                m_name = new char[len];
                strcpy(m_name, m_trueName);

                OBJECT_NAMES->emplace(m_trueName, ID{ 1, 1 });
            }
            else
            {
                const std::string str = std::to_string(iter->second.ID);

                const size_t sLen = str.length();

                size_t cLen = len + sLen;

                m_name = new char[cLen];
                strcpy(m_name, m_trueName);
                strcpy(m_name + len, str.c_str());

                iter->second.ID++;
                iter->second.Objects++;
            }
        }
    }
}
const char* Object::GetTrueName() const
{
    return m_trueName;
}
const char* Object::GetName() const
{
    return m_name;
}