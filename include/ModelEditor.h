#pragma once

#include <glm/glm.hpp>
#include <list>

#include "miniz.h"

enum class e_ModelType;

class Camera;
class ImageDisplay;
class IntermediateRenderer;
class Model;
struct ModelVertex;
class MorphPlane;
class MorphPlaneDisplay;
class Name;
class Namer;
class PropertyFileProperty;
class RenderTexture;
class ShaderProgram;

class ModelEditor
{
private:
    enum class e_ToolMode
    {
        Select,
        Move
    };

    enum class e_Axis
    {
        Null = -1,
        X,
        Y
    };

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
    MorphPlaneData*         m_selectedMorphPlane;

    std::list<ModelData*>*  m_models;

    Namer*                  m_namer;
    Namer*                  m_morphPlaneNamer;

    RenderTexture*          m_renderTexture;

    ImageDisplay*           m_imageDisplay;
    MorphPlaneDisplay*      m_morphPlaneDisplay;

    bool                    m_solid;
    bool                    m_wireframe;
    bool                    m_alpha;

    float                   m_zoom;
    glm::vec2               m_lastMousePos;

    e_ToolMode              m_toolMode;

    bool                    m_dragging;
    e_Axis                  m_axis;
    glm::vec2               m_selectMid;
    glm::vec2               m_lastPos;

    glm::vec2               m_startDragPos;
    glm::vec2               m_endDragPos;

    glm::vec3               m_translation;

    std::list<unsigned int> m_selectedIndices;

    void GetModelData(PropertyFileProperty& a_property, mz_zip_archive& a_archive);

    ModelData* AddModel(const char* a_textureName, const char* a_name, const char* a_trueName, ModelVertex* a_vertices, unsigned int a_vertexCount, unsigned int* a_indices, unsigned int a_indexCount, e_ModelType a_modelType) const;

    void SetSelectTool();
    void SetMoveTool();

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