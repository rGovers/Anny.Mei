#pragma once

class Object;

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

    virtual void Update(double a_delta) = 0;
    virtual void UpdatePreview(double a_delta) = 0;
    virtual void UpdateGUI() = 0;

    virtual const char* ComponentName() const = 0;
};