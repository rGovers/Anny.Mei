#pragma once

#include "Renderer.h"

class ShaderProgram;

class MorphPlaneRenderer : public Renderer
{
private:
    ShaderProgram* m_shaderProgram;

    void Draw(bool a_preview, double a_delta, Camera* a_camera);
protected:

public:
    const static char* COMPONENT_NAME;

    MorphPlaneRenderer(Object* a_object);
    virtual ~MorphPlaneRenderer();

    virtual void Update(double a_delta, Camera* a_camera);
    virtual void UpdatePreview(double a_delta, Camera* a_camera);
    virtual void UpdateGUI();

    virtual const char* ComponentName() const;

    virtual void Load(PropertyFileProperty* a_property);
    virtual void Save(PropertyFile* a_propertyFile, PropertyFileProperty* a_parent) const;
};