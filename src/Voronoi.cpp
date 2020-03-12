#include "Voronoi.h"

#include "Models/Model.h"

Voronoi::Voronoi(const ModelVertex* a_verts, unsigned int a_count, int a_xOffset, int a_yOffset, unsigned int a_xRes, unsigned int a_yRes)
{
    const float INFI = std::numeric_limits<float>::infinity();
    
    m_xRes = a_xRes;
    m_yRes = a_yRes;
    m_diagram = new int[a_xRes * a_yRes];

    for (unsigned int x = 0; x < m_xRes; ++x)
    {
        for (unsigned int y = 0; y < m_yRes; ++y)
        {
            int closest = -1;
            float closeDist = INFI;

            for (unsigned int i = 0; i < a_count; ++i)
            {
                const glm::vec4 aPos = a_verts[i].Position;

                const glm::vec2 vertPos = glm::vec2(aPos.x * m_xRes - a_xOffset, aPos.y * m_yRes - a_yOffset);
                const glm::vec2 pos = glm::vec2(x, y);

                const glm::vec2 diff = vertPos - pos;

                const float dist = glm::length(diff);

                if (dist < closeDist)
                {
                    closest = i;
                    closeDist = dist;
                }
            }

            m_diagram[x + y * m_xRes] = closest;
        }
    }
}
Voronoi::~Voronoi()
{
    delete[] m_diagram;
}

unsigned int Voronoi::GetXResolution() const
{
    return m_xRes;
}
unsigned int Voronoi::GetYResolution() const
{
    return m_yRes;
}

int Voronoi::GetIndex(unsigned int a_x, unsigned int a_y) const
{
    return m_diagram[a_x + a_y * m_xRes];
}