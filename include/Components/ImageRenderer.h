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

    void Draw();
protected:

public:
    ImageRenderer(Object* a_object);
    virtual ~ImageRenderer();

    virtual void Update(double a_delta);
    virtual void UpdatePreview(double a_delta);
    virtual void UpdateGUI();

    virtual const char* ComponentName() const;
};