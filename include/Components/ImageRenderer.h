#pragma once

#include "Renderer.h"

class Model;
class ShaderProgram;
class Texture;

class ImageRenderer : public Renderer
{
private:
    char*          m_modelName;

    ShaderProgram* m_shaderProgram;

    void Draw(bool a_preview, Camera* a_camera);
protected:

public:
    const static char* COMPONENT_NAME;

    ImageRenderer(Object* a_object);
    virtual ~ImageRenderer();

    virtual void Update(double a_delta, Camera* a_camera);
    virtual void UpdatePreview(double a_delta, Camera* a_camera);
    virtual void UpdateGUI();

    virtual const char* ComponentName() const;

    virtual void Load(PropertyFileProperty* a_property);
    virtual void Save(PropertyFile* a_propertyFile, PropertyFileProperty* a_parent) const;
};