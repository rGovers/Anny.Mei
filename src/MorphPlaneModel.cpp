#include "Models/MorphPlaneModel.h"

#include <glad/glad.h>

MorphPlaneModel::MorphPlaneModel() :
    Model()
{
    static const size_t vertexSize = sizeof(ModelVertex);
    static const size_t morphVertexSize = sizeof(MorphPlaneModelVertex);

    glCreateVertexArrays(1, &m_vao);
    glCreateBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, GetVBO());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GetIBO());

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 4, GL_FLOAT, false, vertexSize, (void*)offsetof(ModelVertex, Position));
    glVertexAttribPointer(1, 2, GL_FLOAT, false, vertexSize, (void*)offsetof(ModelVertex, TexCoord));
    
    static const size_t morphPlaneOffset = offsetof(MorphPlaneModelVertex, MorphPlaneWeights);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    for (int i = 0; i < INDEX_COUNT; ++i)
    {
        const unsigned int index = 2 + i;

        glEnableVertexAttribArray(index);

        glVertexAttribPointer(index, 2, GL_FLOAT, false, morphVertexSize, (void*)(morphPlaneOffset + (sizeof(glm::vec2) * i)));
    }
}
MorphPlaneModel::MorphPlaneModel(const Model& a_model) : 
    Model(a_model)
{
    static const size_t vertexSize = sizeof(ModelVertex);
    static const size_t morphVertexSize = sizeof(MorphPlaneModelVertex);

    glCreateVertexArrays(1, &m_vao);
    glCreateBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, GetVBO());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GetIBO());

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 4, GL_FLOAT, false, vertexSize, (void*)offsetof(ModelVertex, Position));
    glVertexAttribPointer(1, 2, GL_FLOAT, false, vertexSize, (void*)offsetof(ModelVertex, TexCoord));
    
    static const size_t morphPlaneOffset = offsetof(MorphPlaneModelVertex, MorphPlaneWeights);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    for (int i = 0; i < INDEX_COUNT; ++i)
    {
        const unsigned int index = 2 + i;

        glEnableVertexAttribArray(index);

        glVertexAttribPointer(index, 2, GL_FLOAT, false, morphVertexSize, (void*)(morphPlaneOffset + (sizeof(glm::vec2) * i)));
    }
}
MorphPlaneModel::~MorphPlaneModel()
{
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
}

unsigned int MorphPlaneModel::GetVAO() const
{
    return m_vao;
}
unsigned int MorphPlaneModel::GetMorphVBO() const
{
    return m_vbo;
}

e_ModelType MorphPlaneModel::GetModelType() const
{
    return e_ModelType::MorphPlane;
}
