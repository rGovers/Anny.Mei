#include "Models/Model.h"

#include <glad/glad.h>

Model::Model() 
{
    m_buffers = new unsigned int[2];

    m_ref = new unsigned long(1UL);

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
Model::Model(const Model& a_model)
{
    m_ref = a_model.m_ref;

    ++(*m_ref);

    m_buffers = a_model.m_buffers;
    m_vao = a_model.m_vao;
    
    m_indices = a_model.m_indices;
}
Model::~Model()
{
    if (--(*m_ref) <= 0)
    {
        delete m_ref;

        glDeleteBuffers(2, m_buffers);
        delete[] m_buffers;

        glDeleteVertexArrays(1, &m_vao);
    }
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

void Model::SetIndicesCount(unsigned int a_indicies)
{
    m_indices = a_indicies;
}
unsigned int Model::GetIndicesCount() const
{
    return m_indices;
}

e_ModelType Model::GetModelType() const
{
    return e_ModelType::Base;
}