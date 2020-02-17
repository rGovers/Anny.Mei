#include "MorphPlane.h"

#include <glad/glad.h>

#include "Texture.h"

MorphPlane::MorphPlane(unsigned int a_dimensions)
{
    m_dimensions = a_dimensions;

    const unsigned int size = m_dimensions * m_dimensions;

    m_morphPos = new glm::vec2[size];

    for (unsigned int x = 0; x < m_dimensions; ++x)
    {
        for (unsigned int y = 0; y < m_dimensions; ++y)
        {
            const unsigned int index = x + y * m_dimensions;

            m_morphPos[index] = { x / (float)m_dimensions, y / (float)m_dimensions };
        }
    }
}
MorphPlane::~MorphPlane()
{
    delete[] m_morphPos;
}

glm::vec2 MorphPlane::GetMorphPosition(unsigned int a_x, unsigned int a_y) const
{
    return m_morphPos[a_x + a_y * m_dimensions];
}

void MorphPlane::SetMorphPosition(const glm::vec2 a_value, unsigned int a_x, unsigned int a_y)
{
    m_morphPos[a_x + a_y * m_dimensions] = a_value;
}

unsigned int MorphPlane::GetSize() const
{
    return m_dimensions;
}

void MorphPlane::Resize(unsigned int a_newSize)
{
    glm::vec2* newMorphPos = new glm::vec2[a_newSize * a_newSize];

    const unsigned int min = glm::min(a_newSize, m_dimensions);

    for (unsigned int x = 0; x < min; ++x)
    {
        for (unsigned int y = 0; y < min; ++y)
        {
            const unsigned int index = x + y * a_newSize;
            const unsigned int oldIndex = x + y * m_dimensions;
            
            newMorphPos[index] = m_morphPos[oldIndex];
        }
    }

    for (unsigned int x = min; x < a_newSize; ++x)
    {
        for (unsigned int y = min; y < a_newSize; ++y)
        {
            const unsigned int index = x + y * a_newSize;

            newMorphPos[index] = { x / (float)m_dimensions, y / (float)m_dimensions };
        }
    }

    delete[] m_morphPos;
    m_morphPos = newMorphPos;

    m_dimensions = a_newSize;
}

Texture* MorphPlane::ToTexture() const
{
    const unsigned int size = m_dimensions * m_dimensions;

    Texture* tex = new Texture(size, 1, GL_RG);

    const int handle = tex->GetHandle();
    
    glBindTexture(GL_TEXTURE_2D, handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size, 1, 0, GL_RG, GL_FLOAT, m_morphPos);

    return tex;
}