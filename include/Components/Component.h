#pragma once

class Camera;
class Object;
class PropertyFile;
class PropertyFileProperty;

enum class e_ComponentType
{
    Null,
    Renderer
};

class Component
{
private:
    Object* m_object;

protected:
    Object* GetObject() const;

public:
    Component(Object* a_object);
    virtual ~Component();

    virtual void Update(double a_delta, Camera* a_camera) = 0;
    virtual void UpdatePreview(double a_delta, Camera* a_camera) = 0;
    virtual void UpdateGUI() = 0;

    virtual const char* ComponentName() const = 0;

    virtual void Load(PropertyFileProperty* a_property) = 0;
    virtual void Save(PropertyFile* a_propertyFile, PropertyFileProperty* a_parent) const = 0;
};