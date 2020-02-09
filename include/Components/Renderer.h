#pragma once

#include "Component.h"

class Renderer : public Component
{
private:

protected:

public:
    Renderer(Object* a_object);
    virtual ~Renderer();

    virtual void Update(double a_delta, Camera* a_camera) = 0;
    virtual void UpdatePreview(double a_delta, Camera* a_camera) = 0;
    virtual void UpdateGUI() = 0;

    virtual const char* ComponentName() const;

    virtual void Load(PropertyFileProperty* a_property) = 0;
    virtual void Save(PropertyFile* a_propertyFile, PropertyFileProperty* a_parent) const = 0;
};