#include "TriImage.h"

#include <algorithm>
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

TriImage::TriImage(const unsigned char* a_textureData, int a_stepX, int a_stepY, int a_width, int a_height, float a_alphaThreshold)
{
    std::list<glm::vec2> verticies;
    std::list<TriImageTriangle> triangles;

    for (int x = 0; x < a_width; ++x)
    {
        for (int y = 0; y < a_height; ++y)
        {
            const int index = (x + y * a_width) * 4;

            if (a_textureData[index] > a_alphaThreshold * 255)
            {
                if (x == 0 || y == 0 || (x % a_stepX == 0 && y % a_stepY == 0))
                {
                    const glm::vec2 pos = { x / (float)a_width, y / (float)a_height };

                    verticies.emplace_back(pos);

                    continue;
                }

                int blankNum = 0;

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

                        const int nIndex = (nX + nY * a_width) * 4;

                        if (a_textureData[nIndex] <= a_alphaThreshold)
                        {
                            ++blankNum;
                        }
                    }
                }

                if (blankNum > 3)
                {
                    glm::vec2 pos = { x / (float)a_width, y / (float)a_height };

                    verticies.emplace_back(pos);
                }
            }
        }
    }

    m_vertCount = verticies.size();
    m_verts = new glm::vec2[m_vertCount];
    std::copy(verticies.begin(), verticies.end(), m_verts);

    const unsigned int xRes = 1024;
    const unsigned int yRes = 1024;

    Voronoi* voronoi = new Voronoi(m_verts, m_vertCount, xRes, yRes);

    for (int x = 0; x < xRes; ++x)
    {
        for (int y = 0; y < yRes; ++y)
        {
            std::vector<int> altIndex;
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

                    if (nX >= 0 && nX < xRes &&
                        nY >= 0 && nY < yRes)
                    {
                        int nIndex = voronoi->GetIndex(nX, nY);

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
            else if (count > 3)
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
    m_indicies = new unsigned int[m_indexCount];

    std::copy(triangles.begin(), triangles.end(), (TriImageTriangle*)m_indicies);

    delete voronoi;
}
TriImage::~TriImage()
{
    delete[] m_verts;
    delete[] m_indicies;
}

unsigned int TriImage::GetIndexCount() const
{
    return m_indexCount;
}
unsigned int* TriImage::GetIndicies() const
{
    return m_indicies;
}

unsigned int TriImage::GetVertexCount() const
{
    return m_vertCount;
}

ModelVertex* TriImage::ToModelVerticies() const
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
SkinnedVertex* TriImage::ToSkinnedVerticies() const
{
    SkinnedVertex* verts = new SkinnedVertex[m_vertCount];

    for (int i = 0; i < m_vertCount; ++i)
    {
        const glm::vec2 pos = m_verts[i];

        verts[i].Position = glm::vec4(pos, 0, 1);
        verts[i].TexCoord = pos;
    }

    return verts;
}