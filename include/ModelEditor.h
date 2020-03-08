#pragma once

#include <glm/glm.hpp>
#include <list>

#include "EditorController.h"
#include "miniz.h"

enum class e_ModelType;

class Camera;
class ImageDisplay;
class IntermediateRenderer;
class Model;
class ModelEditorWindow;
struct ModelVertex;
class MorphPlane;
class MorphPlaneDisplay;
class Name;
class Namer;
class PropertyFileProperty;
class RenderTexture;
class Texture;
class Workspace;

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

class ModelEditor : public EditorController
{
private:
    ModelEditorWindow*      m_window;

    Workspace*              m_workspace;

    IntermediateRenderer*   m_intermediateRenderer;

    ModelData*              m_selectedModelData;
    MorphPlaneData*         m_selectedMorphPlane;

    std::list<ModelData*>*  m_models;

    Namer*                  m_namer;
    Namer*                  m_morphPlaneNamer;

    RenderTexture*          m_renderTexture;

    ImageDisplay*           m_imageDisplay;
    MorphPlaneDisplay*      m_morphPlaneDisplay;

    std::list<unsigned int> m_selectedIndices;

    void GetModelData(PropertyFileProperty& a_property, mz_zip_archive& a_archive);

    ModelData* AddModel(const char* a_textureName, const char* a_name, const char* a_trueName, ModelVertex* a_vertices, unsigned int a_vertexCount, unsigned int* a_indices, unsigned int a_indexCount, e_ModelType a_modelType) const;

    void SetSelectTool();
    void SetMoveTool();

    void GenerateMorphVertexData(ModelData* a_model) const;
protected:

public:
    ModelEditor() = delete;
    ModelEditor(Workspace* a_workspace);
    ~ModelEditor();

    bool IsModelSelected() const;
    bool IsMorphPlaneSelected() const;

    void DrawModelList();

    void Update(double a_delta);

    static ModelEditor* Load(mz_zip_archive& a_archive, Workspace* a_workspace);
    void Save(mz_zip_archive& a_archive) const;

    void AddModel(const char* a_textureName, ModelVertex* a_vertices, unsigned int a_vertexCount, unsigned int* a_indices, unsigned int a_indexCount);

    virtual void DrawPropertiesWindow();
    virtual void DrawEditorWindow();

    const Texture* DrawEditor();

    void RenameModel(const char* a_newName);
    void SetTextureName(const char* a_name);

    void ResizeMorphPlane(int a_newSize);
    void AddMorphPlaneClicked();
    bool IsMorphPlaneSelected(MorphPlaneData* a_morphPlane) const;
    void MorphPlaneSelected(MorphPlaneData* a_morphPlane);

    void DrawSelectionBox(const glm::vec2& a_startPos, const glm::vec2& a_endPos);

    void DragValue(const glm::vec2& a_dragMov);
    void SelectMouseUp(const glm::vec2& a_startPos, const glm::vec2& a_endPos);
};