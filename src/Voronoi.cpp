#include "Voronoi.h"

Voronoi::Voronoi(const glm::vec2* a_verts, unsigned int a_count, unsigned int a_xRes, unsigned int a_yRes) :
    m_xRes(a_xRes),
    m_yRes(a_yRes),
    m_diagram(new int[a_xRes * a_yRes])
{
    const float INFI = std::numeric_limits<float>::infinity();

    for (unsigned int x = 0; x < m_xRes; ++x)
    {
        for (unsigned int y = 0; y < m_yRes; ++y)
        {
            int closest = -1;
            float closeDist = INFI;

            for (unsigned int i = 0; i < a_count; ++i)
            {
                const glm::vec2 vertPos = glm::vec2(a_verts[i].x * m_xRes, a_verts[i].y * m_yRes);
                const glm::vec2 pos = glm::vec2(x, y);

                const glm::vec2 diff = vertPos - pos;

                const float dist = glm::dot(diff, diff);

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