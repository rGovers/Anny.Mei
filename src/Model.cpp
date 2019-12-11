#include "Models/Model.h"

#include <glad/glad.h>

Model::Model() :
    m_buffers(new unsigned int[2])
{
    glGenBuffers(2, m_buffers);
    glGenVertexArrays(1, &m_vao);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[VBO]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[IBO]);

    const size_t vertexSize = sizeof(ModelVertex);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 4, GL_FLOAT, false, vertexSize, (void*)offsetof(ModelVertex, Position));
    glVertexAttribPointer(1, 2, GL_FLOAT, false, vertexSize, (void*)offsetof(ModelVertex, TexCoord));
}
Model::~Model()
{
    glDeleteBuffers(2, m_buffers);
    glDeleteVertexArrays(1, &m_vao);

    delete[] m_buffers;
}

unsigned int Model::GetVAO() const
{
    return m_vao;
}
unsigned int Model::GetVBO() const
{
    return m_buffers[VBO];
}
unsigned int Model::GetIBO() const
{
    return m_buffers[IBO];
}

void Model::SetVerticiesCount(unsigned int a_verticies)
{
    m_verticies = a_verticies;
}
unsigned int Model::GetVerticiesCount() const
{
    return m_verticies;
}

void Model::SetIndiciesCount(unsigned int a_indicies)
{
    m_indicies = a_indicies;
}
unsigned int Model::GetIndiciesCount() const
{
    return m_indicies;
}

