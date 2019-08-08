#include "Object.h"

#include <algorithm>
#include <string.h>

#include "Transform.h"

Object::Object() : 
    m_parent(nullptr),
    m_transform(new Transform()),
    m_name(nullptr)
{
    SetName("Object");
}
Object::~Object()
{
    delete m_transform;

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

void Object::SetName(const char* a_name)
{
    if (m_name != nullptr)
    {
        delete[] m_name;
        m_name = nullptr;
    }

    if (a_name != nullptr)
    {
        const size_t len = strlen(a_name);
        m_name = new char[len];

        strcpy(m_name, a_name);
    }
}
const char* Object::GetName() const
{
    return m_name;
}