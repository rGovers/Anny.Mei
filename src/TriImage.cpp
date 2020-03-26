#include "TriImage.h"

#include <algorithm>
#include <iostream>
#include <vector>

#include "FileLoaders/ImageLoader.h"
#include "Models/Model.h"
#include "Voronoi.h"

void TriImage::WindTriangle(unsigned int& a_a, unsigned int& a_b, unsigned int& a_c) const
{
    const glm::vec2 posA = m_verts[a_a].Position;
    const glm::vec2 posB = m_verts[a_b].Position;
    const glm::vec2 posC = m_verts[a_c].Position;

    const glm::vec2 mid = (posA + posB + posC) * 0.33f;

    const glm::vec2 normA = glm::normalize(posA - mid);
    const glm::vec2 normB = glm::normalize(posB - mid);
    const glm::vec2 normC = glm::normalize(posC - mid);

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
    const glm::vec2 posA = m_verts[a_a].Position;
    const glm::vec2 posB = m_verts[a_b].Position;
    const glm::vec2 posC = m_verts[a_c].Position;
    const glm::vec2 posD = m_verts[a_d].Position;

    const glm::vec2 mid = (posA + posB + posC + posD) * 0.25f;

    const glm::vec2 normA = glm::normalize(posA - mid);
    const glm::vec2 normB = glm::normalize(posB - mid);
    const glm::vec2 normC = glm::normalize(posC - mid);
    const glm::vec2 normD = glm::normalize(posD - mid);

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
void TriImage::WindPent(unsigned int& a_a, unsigned int& a_b, unsigned int& a_c, unsigned int& a_d, unsigned int& a_e) const
{
    const glm::vec2 posA = m_verts[a_a].Position;
    const glm::vec2 posB = m_verts[a_b].Position;
    const glm::vec2 posC = m_verts[a_c].Position;
    const glm::vec2 posD = m_verts[a_d].Position;
    const glm::vec2 posE = m_verts[a_e].Position;

    const glm::vec2 mid = (posA + posB + posC + posD + posE) * 0.20f;

    const glm::vec2 normA = glm::normalize(posA - mid);
    const glm::vec2 normB = glm::normalize(posB - mid);
    const glm::vec2 normC = glm::normalize(posC - mid);
    const glm::vec2 normD = glm::normalize(posD - mid);
    const glm::vec2 normE = glm::normalize(posE - mid);

    float angleA = atan2(normA.x, normA.y);
    float angleB = atan2(normB.x, normB.y);
    float angleC = atan2(normC.x, normC.y);
    float angleD = atan2(normD.x, normD.y);
    float angleE = atan2(normE.x, normE.y);

    if (angleD > angleE)
    {
        const unsigned int tmp = a_d;
        a_d = a_e;
        a_e = tmp;

        const float tmpAngle = angleD;
        angleD = angleE;
        angleE = tmpAngle;
    }

    if (angleC > angleD)
    {
        if (angleC > angleE)
        {
            const unsigned int tmp = a_c;
            a_c = a_e;
            a_e = tmp;

            const float tmpAngle = angleC;
            angleC = angleE;
            angleE = tmpAngle;
        }

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
            if (angleB > angleE)
            {
                const unsigned int tmp = a_b;
                a_b = a_e;
                a_e = tmp;

                const float tmpAngle = angleB;
                angleB = angleE;
                angleE = tmpAngle;
            }

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
                if (angleA > angleE)
                {
                    const unsigned int tmp = a_a;
                    a_a = a_e;
                    a_e = tmp;

                    const float tmpAngle = angleA;
                    angleA = angleE;
                    angleE = tmpAngle;
                }

                const unsigned int tmp = a_a;
                a_a = a_d;
                a_d = tmp;

                const float tmpAngle = angleA;
                angleA = angleD;
                angleD = tmpAngle;
            }

            const unsigned int tmp = a_a;
            a_a = a_c;
            a_c = tmp;

            const float tmpAngle = angleA;
            angleA = angleC;
            angleC = tmpAngle;
        }

        const unsigned int tmp = a_a;
        a_a = a_b;
        a_b = tmp;

        const float tmpAngle = angleA;
        angleA = angleB;
        angleB = tmpAngle;
    }
}

void TriImage::VoronoiTriangulation(int a_vWidth, int a_vHeight)
{
    std::list<TriImageTriangle> triangles;

    std::vector<int> altIndex;

    glm::vec2 offsetScale = glm::vec2(a_vWidth / (float)m_imageSize.x, a_vHeight / (float)m_imageSize.y);
    glm::ivec2 offset = glm::ivec2(offsetScale.x * m_offset.x, offsetScale.y * m_offset.y);

    Voronoi* voronoi = new Voronoi(m_verts, m_vertCount, offset.x, offset.y, a_vWidth, a_vHeight);

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

            switch (count)
            {
            case 3:
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

                break;
            }
            case 4:
            {
                unsigned int indA = altIndex[0];
                unsigned int indB = altIndex[1];
                unsigned int indC = altIndex[2];
                unsigned int indD = altIndex[3];

                if (indA == indB || indA == indC || indA == indD ||
                    indB == indC || indB == indD ||
                    indC == indD)
                {
                    continue;    
                }

                WindQuad(indA, indB, indC, indD);

                const TriImageTriangle triA = 
                {
                    indA,
                    indB,
                    indC
                };
                const TriImageTriangle triB = 
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

                break;
            }
            // Very rare but apparently can occur 
            // Need to look out for situations that more can occur
            case 5:
            {
                unsigned int indA = altIndex[0];
                unsigned int indB = altIndex[1];
                unsigned int indC = altIndex[2];
                unsigned int indD = altIndex[3];
                unsigned int indE = altIndex[4];

                if (indA == indB || indA == indC || indA == indD || indA == indE ||
                    indB == indC || indB == indD || indB == indE || 
                    indC == indD || indC == indE ||
                    indD == indE)
                {
                    continue;    
                }

                WindPent(indA, indB, indC, indD, indE);

                const TriImageTriangle triA = 
                {
                    indA,
                    indD,
                    indE
                };

                const TriImageTriangle triB = 
                {
                    indA,
                    indC,
                    indD
                };

                const TriImageTriangle triC = 
                {
                    indA,
                    indB,
                    indC
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

                iter = std::find(triangles.begin(), triangles.end(), triC);
                if (iter == triangles.end())
                {
                    triangles.emplace_back(triC);
                }

                break;
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

TriImage::TriImage(const unsigned char* a_textureData, const LayerMeta* a_meta)
{
    m_textureData = a_textureData;

    m_verts = nullptr;
    m_indicies = nullptr;

    m_size.x = a_meta->Width;
    m_size.y = a_meta->Height;

    m_offset.x = a_meta->xOffset;
    m_offset.y = a_meta->yOffset;

    m_imageSize.x = a_meta->ImageWidth;
    m_imageSize.y = a_meta->ImageHeight;

    m_min = glm::vec2(std::numeric_limits<float>::infinity());
    m_max = glm::vec2(-std::numeric_limits<float>::infinity());
}
TriImage::~TriImage()
{
    delete[] m_verts;
    delete[] m_indicies;
}

bool TriImage::PlaceAlphaVertex(std::list<ModelVertex>* a_verts, int a_x, int a_y, int a_texXStep, int a_texYStep, float a_alpha)
{
    const int index = (a_x + a_y * m_size.x) * 4 + 3;

    if (m_textureData[index] > a_alpha)
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

                const int nX = a_x + xS;
                const int nY = a_y + yS;

                if (nX < 0 || nX >= m_size.x ||
                    nY < 0 || nY >= m_size.y)
                {
                    continue;
                }

                const int nIndex = (nX + nY * m_size.x) * 4 + 3;

                if (m_textureData[nIndex] <= a_alpha)
                {
                    ++blankNum;

                    dir += glm::vec2(xS, yS);
                }
            }
        }

        if (blankNum > 3)
        {
            if (dir.x == 0 && dir.y == 0)
            {
                return false;
            }

            dir = glm::normalize(dir);

            const glm::vec2 shift = glm::vec2(dir.x * (a_texXStep - 1), dir.y * (a_texYStep - 1));

            const glm::vec2 shiftPos = glm::vec2(shift.x / m_imageSize.x, shift.y / m_imageSize.y);
            const glm::vec2 shiftTex = glm::vec2(shift.x / m_size.x, shift.y / m_size.y);

            const glm::vec2 offPos = (glm::vec2)m_offset + glm::vec2(a_x, a_y);

            const ModelVertex vert = 
            {
                glm::vec4(offPos.x / m_imageSize.x, offPos.y / m_imageSize.y, 0, 1) + glm::vec4(shiftPos, 0, 0),
                glm::vec2(a_x / (float)m_size.x, a_y / (float)m_size.y) + shiftTex
            };

            m_min.x = glm::min(vert.Position.x - 0.00001f, m_min.x);
            m_min.y = glm::min(vert.Position.y - 0.00001f, m_min.y);

            m_max.x = glm::max(vert.Position.x + 0.00001f, m_max.x);
            m_max.y = glm::max(vert.Position.y + 0.00001f, m_max.y);

            a_verts->emplace_back(vert);

            return true;
        }
    }

    return false;
}

void TriImage::AlphaTriangulation(int a_texXStep, int a_texYStep, float a_alphaThreshold, int a_vWidth, int a_vHeight)
{
    std::list<ModelVertex> verticies;

    const float scaledAlpha = a_alphaThreshold * 255;

    for (int x = 0; x < m_size.x; x += a_texXStep)
    {
        for (int y = 0; y < m_size.y; y += a_texYStep)
        {
            PlaceAlphaVertex(&verticies, x, y, a_texXStep, a_texYStep, scaledAlpha);
        }
    }

    m_vertCount = verticies.size();

    if (m_verts != nullptr)
    {
        delete[] m_verts;
    }
    m_verts = new ModelVertex[m_vertCount];
    std::copy(verticies.begin(), verticies.end(), m_verts);

    VoronoiTriangulation(a_vWidth, a_vHeight);    
}
void TriImage::QuadTriangulation(int a_stepX, int a_stepY, int a_texXStep, int a_texYStep, float a_alphaThreshold, int a_vWidth, int a_vHeight)
{
    std::list<ModelVertex> verticies;

    const float scaledAlpha = a_alphaThreshold * 255;

    for (int x = 0; x < m_size.x; x += a_texXStep)
    {
        for (int y = 0; y < m_size.y; y += a_texYStep)
        {
            if (!PlaceAlphaVertex(&verticies, x, y, a_texXStep, a_texYStep, scaledAlpha))
            {
                if (x % a_stepX == 0 && y % a_stepY == 0)
                {
                    const unsigned int index = (x + y * m_size.x) * 4 + 3;

                    if (m_textureData[index] > scaledAlpha)
                    {
                        const glm::vec2 offPos = (glm::vec2)m_offset + glm::vec2(x, y);

                        ModelVertex vert;
                        vert.Position = glm::vec4(offPos.x / m_imageSize.x, offPos.y / m_imageSize.y, 0, 1);
                        vert.TexCoord = glm::vec2(x / (float)m_size.x, y / (float)m_size.y);

                        verticies.emplace_back(vert);
                    }
                }
            }
        }
    }

    m_vertCount = verticies.size();

    if (m_verts != nullptr)
    {
        delete[] m_verts;
    }
    m_verts = new ModelVertex[m_vertCount];
    std::copy(verticies.begin(), verticies.end(), m_verts);

    VoronoiTriangulation(a_vWidth, a_vHeight);    
}
void TriImage::OutlineTriangulation(float a_channelDiff, int a_texXStep, int a_texYStep, float a_alphaThreshold, int a_vWidth, int a_vHeight)
{
    std::list<ModelVertex> verticies;

    const float scaledAlpha = a_alphaThreshold * 255;

    for (int x = 0; x < m_size.x; x += a_texXStep)
    {
        for (int y = 0; y < m_size.y; y += a_texYStep)
        {
            if (!PlaceAlphaVertex(&verticies, x, y, a_texXStep, a_texYStep, scaledAlpha))
            {
                const unsigned int index = (x + y * m_size.x) * 4 + 3;

                float cVal = 0;
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

                        if (nX < 0 || nX >= m_size.x ||
                            nY < 0 || nY >= m_size.y)
                        {
                            continue;
                        }

                        const int nIndex = (nX + nY * m_size.x) * 4 + 3;

                        cVal += ((m_textureData[nIndex + 0] / 255.0f) + (m_textureData[nIndex + 1] / 255.0f) + (m_textureData[nIndex + 2] / 255.0f)) / 3;
                    }
                }

                cVal *= 0.125f;

                const float val = ((m_textureData[index + 0] / 255.0f) + (m_textureData[index + 1] / 255.0f) + (m_textureData[index + 2] / 255.0f)) / 3;

                const float fVal = val - cVal;

                if (glm::abs(fVal) > a_channelDiff)
                {
                    const glm::vec2 offPos = (glm::vec2)m_offset + glm::vec2(x, y);

                    ModelVertex vert;
                    vert.Position = glm::vec4(offPos.x / m_imageSize.x, offPos.y / m_imageSize.y, 0, 1);
                    vert.TexCoord = glm::vec2(x / (float)m_size.x, y / (float)m_size.y);

                    verticies.emplace_back(vert);
                }
            }
        }
    }

    m_vertCount = verticies.size();

    if (m_verts != nullptr)
    {
        delete[] m_verts;
    }
    m_verts = new ModelVertex[m_vertCount];
    std::copy(verticies.begin(), verticies.end(), m_verts);

    VoronoiTriangulation(a_vWidth, a_vHeight);   
}

glm::vec2 TriImage::GetMinConstraint() const
{
    return m_min;
}
glm::vec2 TriImage::GetMaxConstraint() const
{
    return m_max;
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

ModelVertex* TriImage::GetVertices() const
{
    return m_verts;
}

