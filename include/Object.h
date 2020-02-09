#pragma once

#include <list>
#include <map>

class Camera;
class Component;
class PropertyFile;
class PropertyFileProperty;
class Transform;

class Object
{
private:
    struct ID
    {
        long ID;
        long Objects;
    };

    static std::map<std::string, ID>* ObjectNames;
    static std::list<const char*>*     NameList;

    char*                             m_name;
    char*                             m_trueName;

    Object*                           m_parent;
    std::list<Object*>                m_children;

    std::list<Component*>             m_components;

    Transform*                        m_transform;

    bool                              m_windowOpen;

    void CreateName(ID& a_id);
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
    // Note that true name takes priority and can overwrite the name
    // Only use when necessary
    void SetName(const char* a_name);
    const char* GetName() const;

    void RemoveComponent(Component* a_component);

    void LoadComponent(PropertyFileProperty* a_propertyFile);
    void SaveComponents(PropertyFile* a_propertyFile, PropertyFileProperty* a_parent) const;

    void UpdateComponentUI();
    void UpdateComponents(bool a_preview, Camera* a_camera, double a_delta);
};