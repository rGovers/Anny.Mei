#include "Models/MorphTargetModel.h"

#include <glad/glad.h>

MorphTargetModel::MorphTargetModel() :
    Model()
{
    static const size_t vertexSize = sizeof(ModelVertex);

    static const size_t positionOffset = offsetof(ModelVertex, Position);
    static const size_t texCoordOffset = offsetof(ModelVertex, TexCoord);

    m_vbo = new unsigned int[8];

    glCreateVertexArrays(1, &m_vao);
    glCreateBuffers(8, m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, GetVBO());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GetIBO());

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 4, GL_FLOAT, false, vertexSize, (void*)positionOffset);
    glVertexAttribPointer(1, 2, GL_FLOAT, false, vertexSize, (void*)texCoordOffset);

    for (int i = 0; i < 8; ++i)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo[i]);

        const unsigned int positionIndex = 2 + i;

        glEnableVertexAttribArray(positionIndex);

        glVertexAttribPointer(positionIndex, 4, GL_FLOAT, false, sizeof(glm::vec4), (void*)0);
    }
}
MorphTargetModel::MorphTargetModel(const Model& a_model) :
    Model(a_model)
{
    static const size_t vertexSize = sizeof(ModelVertex);

    static const size_t positionOffset = offsetof(ModelVertex, Position);
    static const size_t texCoordOffset = offsetof(ModelVertex, TexCoord);

    m_vbo = new unsigned int[8];

    glCreateVertexArrays(1, &m_vao);
    glCreateBuffers(8, m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, GetVBO());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GetIBO());

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 4, GL_FLOAT, false, vertexSize, (void*)positionOffset);
    glVertexAttribPointer(1, 2, GL_FLOAT, false, vertexSize, (void*)texCoordOffset);

    for (int i = 0; i < 8; ++i)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo[i]);

        const unsigned int positionIndex = 2 + i;

        glEnableVertexAttribArray(positionIndex);

        glVertexAttribPointer(positionIndex, 4, GL_FLOAT, false, sizeof(glm::vec4), (void*)0);
    }
}
MorphTargetModel::~MorphTargetModel()
{
    glDeleteBuffers(8, m_vbo);
    delete[] m_vbo;

    glDeleteVertexArrays(1, &m_vao);
}

unsigned int MorphTargetModel::GetVAO() const
{
    return m_vao;
}
unsigned int MorphTargetModel::GetMorphVBO(int a_index) const
{
    return m_vbo[a_index];
}

e_ModelType MorphTargetModel::GetModelType() const
{
    return e_ModelType::MorphTarget;
}