#pragma once

#include <glm/glm.hpp>

enum class e_ModelType
{
    Base,
    Skinned,
    MorphPlane,
    MorphTarget
};

struct ModelVertex
{
    glm::vec4 Position;
    glm::vec2 TexCoord;
};

class Model
{
private:
    const static int IBO = 0;
    const static int VBO = 1;

    unsigned long* m_ref;

    unsigned int*  m_buffers;
    unsigned int   m_vao;
 
    unsigned int   m_indices;
    unsigned int   m_vertices;

protected:
    Model(const Model& a_model);

public:
    Model();
    virtual ~Model();

    virtual unsigned int GetVAO() const;
    unsigned int GetVBO() const;
    unsigned int GetIBO() const;

    void SetIndicesCount(unsigned int a_indicies);
    unsigned int GetIndicesCount() const;

    void SetVertexCount(unsigned int a_vertices);
    unsigned int GetVerticesCount() const;

    virtual e_ModelType GetModelType() const;
};