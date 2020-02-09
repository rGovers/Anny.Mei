#pragma once

#include "Renderer.h"

#define GLM_SWIZZLE
#include <glm/glm.hpp>

class Material;
class Model;
class Texture;

class ImageRenderer : public Renderer
{
private:
    const int BUFFER_SIZE = 1024;

    char*     m_modelName;

    Model*    m_model;
    Texture*  m_texture;

    Material* m_material;

    glm::vec3 m_anchor;

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