#pragma once

#include <map>
#include <list>

class Component;
class Transform;

class Object
{
private:
    struct ID
    {
        long ID;
        long Objects;
    };

    struct ComponentControl
    {
        bool Open;
        Component* Comp;
    };

    static std::map<std::string, ID>* OBJECT_NAMES;

    char*                             m_name;
    char*                             m_trueName;

    Object*                           m_parent;
    std::list<Object*>                m_children;

    std::list<ComponentControl>      m_components;

    Transform*                        m_transform;

    bool                              m_windowOpen;
protected:

public:
    Object();
    ~Object();

    Transform* GetTransform() const;
    
    Object* GetParent() const;
    void SetParent(Object* a_parent);

    std::list<Object*> GetChildren() const;

    void SetTrueName(const char* a_trueName);
    const char* GetTrueName() const;
    const char* GetName() const;

    void RemoveComponent(Component* a_component);

    void UpdateComponentUI();
};