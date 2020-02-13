#pragma once

#include "Component.h"

#include <glm/glm.hpp>

class PropertyFileProperty;

class Renderer : public Component
{
private:
    char*     m_modelName;

    glm::vec3 m_anchor;
protected:
    void UpdateRendererGUI();

    void LoadValues(PropertyFileProperty* a_property);
    PropertyFileProperty* SaveValues(PropertyFile* a_propertyFile, PropertyFileProperty* a_parent) const;

    const char* GetModelName() const;
    glm::vec3 GetAnchor() const;
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