#pragma once

#include <list>

class AnimControl;
class Camera;
class Component;
class Name;
class Namer;
class PropertyFile;
class PropertyFileProperty;
class Transform;

class Object
{
private:
    AnimControl*                      m_animControl;

    Name*                             m_name;

    Object*                           m_parent;
    std::list<Object*>                m_children;

    std::list<Component*>             m_components;

    Transform*                        m_transform;

    bool                              m_windowOpen;

protected:

public:
    Object() = delete;
    Object(Namer* a_namer, AnimControl* a_animControl);
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

    void DisplayValues(bool a_state);

    void MoveChildUp(Object* a_child);
    void MoveChildDown(Object* a_child);

    void LoadComponent(PropertyFileProperty* a_propertyFile);
    void SaveComponents(PropertyFile* a_propertyFile, PropertyFileProperty* a_parent) const;

    void UpdateComponentUI();
    void UpdateComponents(bool a_preview, Camera* a_camera, double a_delta);
};