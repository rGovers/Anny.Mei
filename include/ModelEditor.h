#pragma once

#include <glm/glm.hpp>
#include <list>

#include "miniz.h"

enum class e_ModelType;

class Camera;
class IntermediateRenderer;
class Model;
struct ModelVertex;
class MorphPlane;
class Name;
class Namer;
class PropertyFileProperty;
class RenderTexture;
class ShaderProgram;

class ModelEditor
{
private:
    struct MorphPlaneData
    {
        Name* MorphPlaneName;
        MorphPlane* Plane;
    };

    struct ModelData
    {
        char* TextureName;
        Name* ModelName; 
        Model* BaseModel;
        unsigned int VertexCount;
        ModelVertex* Vertices;  
        unsigned int IndexCount;
        unsigned int* Indices;

        Model* MorphPlaneModel;
        int MorphPlaneSize;
        std::list<MorphPlaneData*> MorphPlanes;
    };

    IntermediateRenderer*   m_intermediateRenderer;

    ModelData*              m_selectedModelData;
    MorphPlane*             m_selectedMorphPlane;

    std::list<ModelData*>*  m_models;

    Namer*                  m_namer;
    Namer*                  m_morphPlaneNamer;

    ShaderProgram*          m_baseShaderProgram;
    ShaderProgram*          m_wireShaderProgram;

    RenderTexture*          m_renderTexture;

    bool                    m_solid;
    bool                    m_wireframe;
    bool                    m_alpha;

    float                   m_zoom;
    glm::vec2               m_lastMousePos;

    glm::vec3               m_translation;

    void GetModelData(PropertyFileProperty& a_property, mz_zip_archive& a_archive);

    ModelData* AddModel(const char* a_textureName, const char* a_name, const char* a_trueName, ModelVertex* a_vertices, unsigned int a_vertexCount, unsigned int* a_indices, unsigned int a_indexCount, e_ModelType a_modelType) const;

    void GenerateMorphVertexData(ModelData* a_model) const;
protected:

public:
    ModelEditor();
    ~ModelEditor();

    void Update(double a_delta);

    static ModelEditor* Load(mz_zip_archive& a_archive);

    void Save(mz_zip_archive& a_archive) const;

    void AddModel(const char* a_textureName, ModelVertex* a_vertices, unsigned int a_vertexCount, unsigned int* a_indices, unsigned int a_indexCount);
};