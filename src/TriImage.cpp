#include "TriImage.h"

#include <algorithm>
#include <iostream>
#include <list>
#include <vector>

#include "Voronoi.h"

void TriImage::WindTriangle(unsigned int& a_a, unsigned int& a_b, unsigned int& a_c) const
{
    const glm::vec2 mid = (m_verts[a_a] + m_verts[a_b] + m_verts[a_c]) * 0.33f;

    const glm::vec2 normA = glm::normalize(m_verts[a_a] - mid);
    const glm::vec2 normB = glm::normalize(m_verts[a_b] - mid);
    const glm::vec2 normC = glm::normalize(m_verts[a_c] - mid);

    float angleA = atan2(normA.x, normA.y);
    float angleB = atan2(normB.x, normB.y);
    float angleC = atan2(normC.x, normC.y);

    if (angleB > angleC)
    {
        const unsigned int temp = a_b;
        a_b = a_c;
        a_c = temp;

        const float tempAngle = angleB;
        angleB = angleC;
        angleC = tempAngle;
    }
    if (angleA > angleB)
    {
        if (angleA > angleC)
        {
            const unsigned int temp = a_c;
            a_c = a_a;
            a_a = temp;
        }

        const unsigned int temp = a_b;
        a_b = a_a;
        a_a = temp;
    }
}
void TriImage::WindQuad(unsigned int& a_a, unsigned int& a_b, unsigned int& a_c, unsigned int& a_d) const
{
    const glm::vec2 mid = (m_verts[a_a] + m_verts[a_b] + m_verts[a_c] + m_verts[a_d]) * 0.25f;

    const glm::vec2 normA = glm::normalize(m_verts[a_a] - mid);
    const glm::vec2 normB = glm::normalize(m_verts[a_b] - mid);
    const glm::vec2 normC = glm::normalize(m_verts[a_c] - mid);
    const glm::vec2 normD = glm::normalize(m_verts[a_d] - mid);

    float angleA = atan2(normA.x, normA.y);
    float angleB = atan2(normB.x, normB.y);
    float angleC = atan2(normC.x, normC.y);
    float angleD = atan2(normD.x, normD.y);

    if (angleC > angleD)
    {
        const unsigned int tmp = a_c;
        a_c = a_d;
        a_d = tmp;

        const float tmpAngle = angleC;
        angleC = angleD;
        angleD = tmpAngle;
    }

    if (angleB > angleC)
    {
        if (angleB > angleD)
        {
            const unsigned int tmp = a_b;
            a_b = a_d;
            a_d = tmp;

            const float tmpAngle = angleB;
            angleB = angleD;
            angleD = tmpAngle;
        }

        const unsigned int tmp = a_b;
        a_b = a_c;
        a_c = tmp;

        const float tmpAngle = angleB;
        angleB = angleC;
        angleC = tmpAngle;
    }

    if (angleA > angleB)
    {
        if (angleA > angleC)
        {
            if (angleA > angleD)
            {
                const unsigned int tmp = a_a;
                a_a = a_d;
                a_d = tmp;
            }

            const unsigned int tmp = a_a;
            a_a = a_c;
            a_c = tmp;
        }

        const unsigned int tmp = a_a;
        a_a = a_b;
        a_b = tmp;
    }
}

void TriImage::VoronoiTriangulation(int a_vWidth, int a_vHeight)
{
    std::list<TriImageTriangle> triangles;

    std::vector<int> altIndex;

    Voronoi* voronoi = new Voronoi(m_verts, m_vertCount, a_vWidth, a_vHeight);

    for (int x = 0; x < a_vWidth; ++x)
    {
        for (int y = 0; y < a_vHeight; ++y)
        {
            altIndex.clear();

            altIndex.emplace_back(voronoi->GetIndex(x, y));

            for (int xS = -1; xS <= 1; ++xS)
            {
                for (int yS = -1; yS <= 1; ++yS)
                {
                    if (xS == 0 && yS == 0)
                    {
                        continue;
                    }

                    const int nX = x + xS;
                    const int nY = y + yS;

                    if (nX >= 0 && nX < a_vWidth &&
                        nY >= 0 && nY < a_vHeight)
                    {
                        const int nIndex = voronoi->GetIndex(nX, nY);

                        auto iter = std::find(altIndex.begin(), altIndex.end(), nIndex);
                        if (iter == altIndex.end())
                        {
                            altIndex.emplace_back(nIndex);
                        }
                    }
                }
            }

            const size_t count = altIndex.size();

            if (count == 3)
            {
                TriImageTriangle tri = 
                {
                    altIndex[0],
                    altIndex[1],
                    altIndex[2]
                };

                if (tri.A == tri.B || tri.B == tri.C || tri.C == tri.A)
                {
                    continue;
                }

                WindTriangle(tri.A, tri.B, tri.C);

                auto iter = std::find(triangles.begin(), triangles.end(), tri);
                if (iter == triangles.end())
                {
                    triangles.emplace_back(tri);
                }
            }
            else if (count == 4)
            {
                unsigned int indA = altIndex[0];
                unsigned int indB = altIndex[1];
                unsigned int indC = altIndex[2];
                unsigned int indD = altIndex[3];

                WindQuad(indA, indB, indC, indD);

                TriImageTriangle triA = 
                {
                    indA,
                    indB,
                    indC
                };
                TriImageTriangle triB = 
                {
                    indA,
                    indC,
                    indD
                };

                std::list<TriImageTriangle>::iterator iter;
                iter = std::find(triangles.begin(), triangles.end(), triA);
                if (iter == triangles.end())
                {
                    triangles.emplace_back(triA);
                }

                iter = std::find(triangles.begin(), triangles.end(), triB);
                if (iter == triangles.end())
                {
                    triangles.emplace_back(triB);
                }
            }
        }
    }
    m_indexCount = triangles.size() * 3;
    if (m_indicies != nullptr)
    {
        delete[] m_indicies;
    }
    m_indicies = new unsigned int[m_indexCount];

    std::copy(triangles.begin(), triangles.end(), (TriImageTriangle*)m_indicies);

    delete voronoi;
}

TriImage::TriImage(const unsigned char* a_textureData, int a_width, int a_height)
{
    m_textureData = a_textureData;

    m_verts = nullptr;
    m_indicies = nullptr;

    m_width = a_width;
    m_height = a_height;
}
TriImage::~TriImage()
{
    delete[] m_verts;
    delete[] m_indicies;
}

void TriImage::AlphaTriangulation(int a_texXStep, int a_texYStep, float a_alphaThreshold, int a_vWidth, int a_vHeight)
{
    std::list<glm::vec2> verticies;

    const unsigned int scaledAlpha = a_alphaThreshold * 255;

    for (int x = 0; x < m_width; x += a_texXStep)
    {
        for (int y = 0; y < m_height; y += a_texYStep)
        {
            const int index = (x + y * m_width) * 4 + 3;

            if (m_textureData[index] > scaledAlpha)
            {
                int blankNum = 0;

                glm::vec2 dir = glm::vec2(0);

                for (int xS = -a_texXStep; xS <= a_texXStep; xS += a_texXStep)
                {
                    for (int yS = -a_texYStep; yS <= a_texYStep; yS += a_texYStep)
                    {
                        if (xS == 0 && yS == 0)
                        {
                            continue;
                        }

                        const int nX = x + xS;
                        const int nY = y + yS;

                        if (nX < 0 || nX >= m_width ||
                            nY < 0 || nY >= m_height)
                        {
                            continue;
                        }

                        const int nIndex = (nX + nY * m_width) * 4 + 3;

                        if (m_textureData[nIndex] <= scaledAlpha)
                        {
                            ++blankNum;

                            dir += glm::vec2(xS, yS);
                        }
                    }
                }

                if (blankNum > 3)
                {
                    if (dir.x != 0 || dir.y != 0)
                    {
                        dir = glm::normalize(dir);
                    }

                    const glm::vec2 pos = glm::vec2(x / (float)m_width, y / (float)m_height) + glm::vec2((dir.x * a_texXStep) / m_width, (dir.y * a_texYStep) / m_height);

                    verticies.emplace_back(pos);
                }
            }
        }
    }

    m_vertCount = verticies.size();

    if (m_verts != nullptr)
    {
        delete[] m_verts;
    }
    m_verts = new glm::vec2[m_vertCount];
    std::copy(verticies.begin(), verticies.end(), m_verts);

    VoronoiTriangulation(a_vWidth, a_vHeight);    
}
void TriImage::QuadTriangulation(int a_stepX, int a_stepY, int a_texXStep, int a_texYStep, float a_alphaThreshold, int a_vWidth, int a_vHeight)
{
    std::list<glm::vec2> verticies;

    const unsigned int scaledAlpha = a_alphaThreshold * 255;

    for (int x = 0; x < m_width; x += a_texXStep)
    {
        for (int y = 0; y < m_height; y += a_texYStep)
        {
            const int index = (x + y * m_width) * 4 + 3;

            if (m_textureData[index] > scaledAlpha)
            {
                if ((x % a_stepX == 0 && y % a_stepY == 0))
                {
                    const glm::vec2 pos = { x / (float)m_width, y / (float)m_height };

                    verticies.emplace_back(pos);

                    continue;
                }

                int blankNum = 0;

                glm::vec2 dir = glm::vec2(0);

                for (int xS = -a_texXStep; xS <= a_texXStep; xS += a_texXStep)
                {
                    for (int yS = -a_texYStep; yS <= a_texYStep; yS += a_texYStep)
                    {
                        if (xS == 0 && yS == 0)
                        {
                            continue;
                        }

                        const int nX = x + xS;
                        const int nY = y + yS;

                        if (nX < 0 || nX >= m_width ||
                            nY < 0 || nY >= m_height)
                        {
                            continue;
                        }

                        const int nIndex = (nX + nY * m_width) * 4 + 3;

                        if (m_textureData[nIndex] <= scaledAlpha)
                        {
                            ++blankNum;

                            dir += glm::vec2(xS, yS);
                        }
                    }
                }

                if (blankNum > 3)
                {
                    if (dir.x != 0 || dir.y != 0)
                    {
                        dir = glm::normalize(dir);
                    }

                    const glm::vec2 pos = glm::vec2(x / (float)m_width, y / (float)m_height) + glm::vec2((dir.x * a_texXStep) / m_width, (dir.y * a_texYStep) / m_height);

                    verticies.emplace_back(pos);
                }
            }
        }
    }

    m_vertCount = verticies.size();

    if (m_verts != nullptr)
    {
        delete[] m_verts;
    }
    m_verts = new glm::vec2[m_vertCount];
    std::copy(verticies.begin(), verticies.end(), m_verts);

    VoronoiTriangulation(a_vWidth, a_vHeight);    
}
void TriImage::OutlineTriangulation(float a_channelDiff, int a_texXStep, int a_texYStep, float a_alphaThreshold, int a_vWidth, int a_vHeight)
{
    std::list<glm::vec2> verticies;

    const unsigned int scaledAlpha = a_alphaThreshold * 255;

    for (int x = 0; x < m_width; x += a_texXStep)
    {
        for (int y = 0; y < m_height; y += a_texYStep)
        {
            const int index = (x + y * m_width) * 4;

            float cVal = 0;

            if (m_textureData[index + 3] > scaledAlpha)
            {
                int blankNum = 0;
                
                glm::vec2 dir = glm::vec2(0);

                for (int xS = -a_texXStep; xS <= a_texXStep; xS += a_texXStep)
                {
                    for (int yS = -a_texYStep; yS <= a_texYStep; yS += a_texYStep)
                    {
                        if (xS == 0 && yS == 0)
                        {
                            continue;
                        }

                        const int nX = x + xS;
                        const int nY = y + yS;

                        if (nX < 0 || nX >= m_width ||
                            nY < 0 || nY >= m_height)
                        {
                            continue;
                        }

                        const int nIndex = (nX + nY * m_width) * 4;

                        cVal += ((m_textureData[nIndex + 0] / 255.0f) + (m_textureData[nIndex + 1] / 255.0f) + (m_textureData[nIndex + 2] / 255.0f)) / 3;

                        if (m_textureData[nIndex + 3] <= scaledAlpha)
                        {
                            ++blankNum;

                            dir += glm::vec2(xS, yS);
                        }
                    }
                }

                if (blankNum > 3)
                {
                    if (dir.x != 0 || dir.y != 0)
                    {
                        dir = glm::normalize(dir);
                    }

                    const glm::vec2 pos = glm::vec2(x / (float)m_width, y / (float)m_height) + glm::vec2((dir.x * a_texXStep) / m_width, (dir.y * a_texYStep) / m_height);

                    verticies.emplace_back(pos);

                    continue;
                }

                cVal *= 0.125f;

                const float val = ((m_textureData[index + 0] / 255.0f) + (m_textureData[index + 1] / 255.0f) + (m_textureData[index + 2] / 255.0f)) / 3;

                const float fVal = val - cVal;

                if (glm::abs(fVal) > a_channelDiff)
                {
                    const glm::vec2 pos = { x / (float)m_width, y / (float)m_height };

                    verticies.emplace_back(pos);
                }
            }
        }
    }

    m_vertCount = verticies.size();

    if (m_verts != nullptr)
    {
        delete[] m_verts;
    }
    m_verts = new glm::vec2[m_vertCount];
    std::copy(verticies.begin(), verticies.end(), m_verts);

    VoronoiTriangulation(a_vWidth, a_vHeight);   
}

unsigned int TriImage::GetIndexCount() const
{
    return m_indexCount;
}
unsigned int* TriImage::GetIndices() const
{
    return m_indicies;
}

unsigned int TriImage::GetVertexCount() const
{
    return m_vertCount;
}

ModelVertex* TriImage::ToModelVertices() const
{
    ModelVertex* verts = new ModelVertex[m_vertCount];

    for (int i = 0; i < m_vertCount; ++i)
    {
        const glm::vec2 pos = m_verts[i];

        verts[i].Position = glm::vec4(pos, 0, 1);
        verts[i].TexCoord = pos;
    }

    return verts;
}

