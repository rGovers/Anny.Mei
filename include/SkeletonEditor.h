#pragma once

#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <istream>
#include <map>

#include "miniz.h"

class Camera;
class IntermediateRenderer;
class Model;
struct ModelVertex;
class Namer;
class Object;
class PropertyFile;
class PropertyFileProperty;
class RenderTexture;

struct ModelData
{
    unsigned int* Indices;
    ModelVertex* Vertices;
    unsigned int IndexCount;
    unsigned int VertexCount;
    Model* GModel;
};

class SkeletonEditor
{
private:
    Namer*                m_namer;

    Object*               m_baseObject;
    Object*               m_selectedObject;

    Camera*               m_camera;

    IntermediateRenderer* m_imRenderer;
    RenderTexture*        m_renderTexture;

    glm::vec2             m_lastMousePos;

    float                 m_zoom;

    void ListObjects(Object* a_object, int& a_node);

    void LoadObject(Object* a_object, PropertyFileProperty* a_property);
    void SaveObject(PropertyFile* a_propertyFile, PropertyFileProperty* a_parent, Object* a_object) const;
protected:

public:
    SkeletonEditor();
    ~SkeletonEditor();

    Object* GetBaseObject() const;

    void DrawObjectDetail(Object* a_object) const;

    void Update(double a_delta);

    static SkeletonEditor* Load(mz_zip_archive& a_archive);
    void Save(mz_zip_archive& a_archive) const;
};