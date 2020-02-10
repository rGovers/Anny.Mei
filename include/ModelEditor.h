#pragma once

#include <list>

#include "miniz.h"

class Model;
struct ModelVertex;
class Name;
class Namer;
class PropertyFileProperty;
class RenderTexture;
class ShaderProgram;

class ModelEditor
{
private:
    struct ModelData
    {
        char* TextureName;
        Name* ModelName; 
        Model* ModelP;
        unsigned int VertexCount;
        ModelVertex* Vertices;  
        unsigned int IndexCount;
        unsigned int* Indices;
    };

    ModelData*             m_selectedModelData;

    std::list<ModelData*>* m_models;

    Namer*                 m_namer;

    ShaderProgram*         m_baseShaderProgram;
    ShaderProgram*         m_wireShaderProgram;

    RenderTexture*         m_renderTexture;

    bool                   m_solid;
    bool                   m_wireframe;

    void GetModelData(PropertyFileProperty& a_property, mz_zip_archive& a_archive);

    ModelData* AddModel(const char* a_textureName, const char* a_name, const char* a_trueName, ModelVertex* a_vertices, unsigned int a_vertexCount, unsigned int* a_indices, unsigned int a_indexCount) const;
protected:

public:
    ModelEditor();
    ~ModelEditor();

    void Update(double a_delta);

    static ModelEditor* Load(mz_zip_archive& a_archive);

    void Save(mz_zip_archive& a_archive) const;

    void AddModel(const char* a_textureName, ModelVertex* a_vertices, unsigned int a_vertexCount, unsigned int* a_indices, unsigned int a_indexCount);
};