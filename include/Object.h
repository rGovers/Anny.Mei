#pragma once

#include <list>

class Transform;

class Object
{
private:
    char*              m_name;

    Object*            m_parent;

    std::list<Object*> m_children;

    Transform*         m_transform;
protected:

public:
    Object();
    ~Object();

    Transform* GetTransform() const;
    
    Object* GetParent() const;
    void SetParent(Object* a_parent);

    std::list<Object*> GetChildren() const;

    void SetName(const char* a_name);
    const char* GetName() const;
};