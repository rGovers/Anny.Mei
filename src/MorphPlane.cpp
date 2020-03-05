#include "MorphPlane.h"

#include <glad/glad.h>

#include "FileUtils.h"
#include "Texture.h"

MorphPlane::MorphPlane()
{
    m_dimensions = 0;
    m_morphPos = nullptr;
}
MorphPlane::MorphPlane(unsigned int a_dimensions)
{
    m_dimensions = a_dimensions + 1;

    const unsigned int size = m_dimensions * m_dimensions;

    m_morphPos = new glm::vec2[size];

    for (unsigned int x = 0; x < m_dimensions; ++x)
    {
        for (unsigned int y = 0; y < m_dimensions; ++y)
        {
            const unsigned int index = x + y * m_dimensions;

            m_morphPos[index] = { x / (float)a_dimensions, y / (float)a_dimensions };
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
glm::vec2* MorphPlane::GetMorphPositions() const
{
    return m_morphPos;
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
    const unsigned int trueSize = a_newSize + 1;

    glm::vec2* newMorphPos = new glm::vec2[trueSize * trueSize];

    const unsigned int min = glm::min(trueSize, m_dimensions);

    for (unsigned int x = 0; x < min; ++x)
    {
        for (unsigned int y = 0; y < min; ++y)
        {
            const unsigned int index = x + y * trueSize;
            const unsigned int oldIndex = x + y * m_dimensions;
            
            newMorphPos[index] = m_morphPos[oldIndex];
        }
    }

    for (unsigned int x = min; x < trueSize; ++x)
    {
        for (unsigned int y = min; y < trueSize; ++y)
        {
            const unsigned int index = x + y * trueSize;

            newMorphPos[index] = { x / (float)a_newSize, y / (float)a_newSize };
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, size, 1, 0, GL_RG, GL_FLOAT, m_morphPos);

    return tex;
}

MorphPlane* MorphPlane::Load(const char* a_fileName, mz_zip_archive& a_archive, unsigned int a_size)
{
    MorphPlane* morphPlane = new MorphPlane();

    morphPlane->m_dimensions = a_size + 1;
    morphPlane->m_morphPos = (glm::vec2*)ExtractFileFromArchive(a_fileName, a_archive);

    if (morphPlane->m_morphPos == nullptr)
    {
        morphPlane->m_morphPos = new glm::vec2[morphPlane->m_dimensions * morphPlane->m_dimensions];
    }

    return morphPlane;
}

MorphPlane* MorphPlane::Lerp(float a_lerp, const MorphPlane& a_left, const MorphPlane& a_right) const
{
    MorphPlane* morphPlane = new MorphPlane();
    morphPlane->m_dimensions = m_dimensions;

    const unsigned int size = m_dimensions * m_dimensions;

    morphPlane->m_morphPos = new glm::vec2[size];

    if (a_lerp < 0)
    {
        for (unsigned int x = 0; x < m_dimensions; ++x)
        {
            for (unsigned int y = 0; y < m_dimensions; ++y)
            {
                const unsigned int index = x + y * m_dimensions;

                morphPlane->m_morphPos[index] = glm::mix(m_morphPos[index], a_left.m_morphPos[index], -a_lerp);
            }
        }
    }
    else
    {
        for (unsigned int x = 0; x < m_dimensions; ++x)
        {
            for (unsigned int y = 0; y < m_dimensions; ++y)
            {
                const unsigned int index = x + y * m_dimensions;

                morphPlane->m_morphPos[index] = glm::mix(m_morphPos[index], a_right.m_morphPos[index], a_lerp);
            }
        }
    }

    return morphPlane;
}
MorphPlane* MorphPlane::Lerp5(const glm::vec2& a_lerp, const MorphPlane& a_left, const MorphPlane& a_right, const MorphPlane& a_up, const MorphPlane& a_down) const
{
    MorphPlane* morphPlane = new MorphPlane();

    morphPlane->m_dimensions = m_dimensions;

    const unsigned int size = m_dimensions * m_dimensions;

    morphPlane->m_morphPos = new glm::vec2[size];

    if (a_lerp.x < 0)
    {
        if (a_lerp.y < 0)
        {
            for (unsigned int x = 0; x < m_dimensions; ++x)
            {
                for (unsigned int y = 0; y < m_dimensions; ++y)
                {
                    const unsigned int index = x + y * m_dimensions;

                    const glm::vec2 posA = m_morphPos[index];
                    const glm::vec2 posB = a_left.m_morphPos[index];
                    const glm::vec2 posC = a_down.m_morphPos[index];

                    glm::vec2 dirA = posC - posA;
                    glm::vec2 dirB = posB - posA;

                    const float lenA = glm::length(dirA);
                    const float lenB = glm::length(dirB);

                    if (lenA != 0)
                    {
                        dirA /= lenA;
                    }
                    if (lenB != 0)
                    {
                        dirB /= lenB;
                    }

                    morphPlane->m_morphPos[index] = posA + (dirA * lenA * -a_lerp.y) + (dirB * lenB * -a_lerp.x);
                }
            }
        }
        else
        {
            for (unsigned int x = 0; x < m_dimensions; ++x)
            {
                for (unsigned int y = 0; y < m_dimensions; ++y)
                {
                    const unsigned int index = x + y * m_dimensions;

                    const glm::vec2 posA = m_morphPos[index];
                    const glm::vec2 posB = a_left.m_morphPos[index];
                    const glm::vec2 posC = a_up.m_morphPos[index];

                    glm::vec2 dirA = posC - posA;
                    glm::vec2 dirB = posB - posA;

                    const float lenA = glm::length(dirA);
                    const float lenB = glm::length(dirB);

                    if (lenA != 0)
                    {
                        dirA /= lenA;
                    }
                    if (lenB != 0)
                    {
                        dirB /= lenB;
                    }

                    morphPlane->m_morphPos[index] = posA + (dirA * lenA * a_lerp.y) + (dirB * lenB * -a_lerp.x);
                }
            }
        }
    }
    else
    {
        if (a_lerp.y < 0)
        {
            for (unsigned int x = 0; x < m_dimensions; ++x)
            {
                for (unsigned int y = 0; y < m_dimensions; ++y)
                {
                    const unsigned int index = x + y * m_dimensions;

                    const glm::vec2 posA = m_morphPos[index];
                    const glm::vec2 posB = a_right.m_morphPos[index];
                    const glm::vec2 posC = a_down.m_morphPos[index];

                    glm::vec2 dirA = posC - posA;
                    glm::vec2 dirB = posB - posA;

                    const float lenA = glm::length(dirA);
                    const float lenB = glm::length(dirB);

                    if (lenA != 0)
                    {
                        dirA /= lenA;
                    }
                    if (lenB != 0)
                    {
                        dirB /= lenB;
                    }

                    morphPlane->m_morphPos[index] = posA + (dirA * lenA * -a_lerp.y) + (dirB * lenB * a_lerp.x);
                }
            }
        }
        else
        {
            for (unsigned int x = 0; x < m_dimensions; ++x)
            {
                for (unsigned int y = 0; y < m_dimensions; ++y)
                {
                    const unsigned int index = x + y * m_dimensions;

                    const glm::vec2 posA = m_morphPos[index];
                    const glm::vec2 posB = a_right.m_morphPos[index];
                    const glm::vec2 posC = a_up.m_morphPos[index];

                    glm::vec2 dirA = posC - posA;
                    glm::vec2 dirB = posB - posA;

                    const float lenA = glm::length(dirA);
                    const float lenB = glm::length(dirB);

                    if (lenA != 0)
                    {
                        dirA /= lenA;
                    }
                    if (lenB != 0)
                    {
                        dirB /= lenB;
                    }

                    morphPlane->m_morphPos[index] = posA + (dirA * lenA * a_lerp.y) + (dirB * lenB * a_lerp.x);
                }
            }
        }
    }

    return morphPlane;
}
MorphPlane* MorphPlane::Lerp9(const glm::vec2& a_lerp, const MorphPlane& a_left, const MorphPlane& a_right, const MorphPlane& a_up, const MorphPlane& a_down,
    const MorphPlane& a_upLeft, const MorphPlane& a_downLeft, const MorphPlane& a_downRight, const MorphPlane& a_upRight) const
{
    MorphPlane* morphPlane = new MorphPlane();

    morphPlane->m_dimensions = m_dimensions;

    const unsigned int size = m_dimensions * m_dimensions;

    morphPlane->m_morphPos = new glm::vec2[size];

    if (a_lerp.x < 0)
    {
        if (a_lerp.y < 0)
        {
            for (unsigned int x = 0; x < m_dimensions; ++x)
            {
                for (unsigned int y = 0; y < m_dimensions; ++y)
                {
                    const unsigned int index = x + y * m_dimensions;

                    const glm::vec2 posA = m_morphPos[index];
                    const glm::vec2 posB = a_left.m_morphPos[index];
                    const glm::vec2 posC = a_down.m_morphPos[index];
                    const glm::vec2 posD = a_downLeft.m_morphPos[index];

                    glm::vec2 dirA = posD - posA;
                    glm::vec2 dirB = posC - posA;
                    glm::vec2 dirC = posB - posA;

                    const float lenA = glm::length(dirA);
                    const float lenB = glm::length(dirB);
                    const float lenC = glm::length(dirC);

                    if (lenA != 0)
                    {
                        dirA /= lenA;
                    }
                    if (lenB != 0)
                    {
                        dirB /= lenB;
                    }
                    if (lenC != 0)
                    {
                        dirC /= lenC;
                    }

                    const float lerpA = glm::max(0.0f, 1 - glm::length(glm::vec2(-1, 0) - a_lerp));
                    const float lerpB = glm::max(0.0f, 1 - glm::length(glm::vec2(0, -1) - a_lerp));
                    const float lerpC = glm::max(0.0f, 1 - glm::length(glm::vec2(-1, -1) - a_lerp));

                    morphPlane->m_morphPos[index] = posA + (dirA * lenA * lerpC) + (dirB * lenB * lerpB) + (dirC * lenC * lerpA);
                }
            }
        }
        else
        {
            for (unsigned int x = 0; x < m_dimensions; ++x)
            {
                for (unsigned int y = 0; y < m_dimensions; ++y)
                {
                    const unsigned int index = x + y * m_dimensions;

                    const glm::vec2 posA = m_morphPos[index];
                    const glm::vec2 posB = a_left.m_morphPos[index];
                    const glm::vec2 posC = a_up.m_morphPos[index];
                    const glm::vec2 posD = a_upLeft.m_morphPos[index];

                    glm::vec2 dirA = posD - posA;
                    glm::vec2 dirB = posC - posA;
                    glm::vec2 dirC = posB - posA;

                    const float lenA = glm::length(dirA);
                    const float lenB = glm::length(dirB);
                    const float lenC = glm::length(dirC);

                    if (lenA != 0)
                    {
                        dirA /= lenA;
                    }
                    if (lenB != 0)
                    {
                        dirB /= lenB;
                    }
                    if (lenC != 0)
                    {
                        dirC /= lenC;
                    }

                    const float lerpA = glm::max(0.0f, 1 - glm::length(glm::vec2(-1, 0) - a_lerp));
                    const float lerpB = glm::max(0.0f, 1 - glm::length(glm::vec2(0, 1) - a_lerp));
                    const float lerpC = glm::max(0.0f, 1 - glm::length(glm::vec2(-1, 1) - a_lerp));

                    morphPlane->m_morphPos[index] = posA + (dirA * lenA * lerpC) + (dirB * lenB * lerpB) + (dirC * lenC * lerpA);
                }
            }
        }
    }
    else
    {
        if (a_lerp.y < 0)
        {
            for (unsigned int x = 0; x < m_dimensions; ++x)
            {
                for (unsigned int y = 0; y < m_dimensions; ++y)
                {
                    const unsigned int index = x + y * m_dimensions;

                    const glm::vec2 posA = m_morphPos[index];
                    const glm::vec2 posB = a_right.m_morphPos[index];
                    const glm::vec2 posC = a_down.m_morphPos[index];
                    const glm::vec2 posD = a_downRight.m_morphPos[index];

                    glm::vec2 dirA = posD - posA;
                    glm::vec2 dirB = posC - posA;
                    glm::vec2 dirC = posB - posA;

                    const float lenA = glm::length(dirA);
                    const float lenB = glm::length(dirB);
                    const float lenC = glm::length(dirC);

                    if (lenA != 0)
                    {
                        dirA /= lenA;
                    }
                    if (lenB != 0)
                    {
                        dirB /= lenB;
                    }
                    if (lenC != 0)
                    {
                        dirC /= lenC;
                    }

                    const float lerpA = glm::max(0.0f, 1 - glm::length(glm::vec2(1, 0) - a_lerp));
                    const float lerpB = glm::max(0.0f, 1 - glm::length(glm::vec2(0, -1) - a_lerp));
                    const float lerpC = glm::max(0.0f, 1 - glm::length(glm::vec2(1, -1) - a_lerp));

                    morphPlane->m_morphPos[index] = posA + (dirA * lenA * lerpC) + (dirB * lenB * lerpB) + (dirC * lenC * lerpA);
                }
            }
        }
        else
        {
            for (unsigned int x = 0; x < m_dimensions; ++x)
            {
                for (unsigned int y = 0; y < m_dimensions; ++y)
                {
                    const unsigned int index = x + y * m_dimensions;

                    const glm::vec2 posA = m_morphPos[index];
                    const glm::vec2 posB = a_right.m_morphPos[index];
                    const glm::vec2 posC = a_up.m_morphPos[index];
                    const glm::vec2 posD = a_upRight.m_morphPos[index];

                    glm::vec2 dirA = posD - posA;
                    glm::vec2 dirB = posC - posA;
                    glm::vec2 dirC = posB - posA;

                    const float lenA = glm::length(dirA);
                    const float lenB = glm::length(dirB);
                    const float lenC = glm::length(dirC);

                    if (lenA != 0)
                    {
                        dirA /= lenA;
                    }
                    if (lenB != 0)
                    {
                        dirB /= lenB;
                    }
                    if (lenC != 0)
                    {
                        dirC /= lenC;
                    }

                    const float lerpA = glm::max(0.0f, 1 - glm::length(glm::vec2(1, 0) - a_lerp));
                    const float lerpB = glm::max(0.0f, 1 - glm::length(glm::vec2(0, 1) - a_lerp));
                    const float lerpC = glm::max(0.0f, 1 - glm::length(glm::vec2(1, 1) - a_lerp));

                    morphPlane->m_morphPos[index] = posA + (dirA * lenA * lerpC) + (dirB * lenB * lerpB) + (dirC * lenC * lerpA);
                }
            }
        }
    }

    return morphPlane;
}