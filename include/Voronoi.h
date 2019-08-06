#pragma once

#define GLM_SWIZZLE
#include <glm/glm.hpp>

class Voronoi
{
private:
    unsigned int m_xRes;
    unsigned int m_yRes;

    int*         m_diagram;
protected:

public:
    Voronoi(const glm::vec2* a_verts, unsigned int a_count, unsigned int a_xRex, unsigned int a_yRes);
    ~Voronoi();

    inline unsigned int GetXResolution() const;
    inline unsigned int GetYResolution() const;

    int GetIndex(unsigned int a_x, unsigned int a_y) const;
};