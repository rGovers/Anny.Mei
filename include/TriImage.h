#pragma once

#include "Models/Model.h"

class Texture;

struct TriImageTriangle
{
    int A, B, C;

    bool operator ==(const TriImageTriangle& a_triB) const
    {
        return A == a_triB.A && B == a_triB.B && C == a_triB.C;
    }
};

class TriImage
{
private:
    unsigned int m_vertCount;
    glm::vec2*   m_verts;
    
    int*         m_indicies;
    unsigned int m_indexCount;

    void WindTriangle(int& a_a, int& a_b, int& a_c) const;
    void WindQuad(int& a_a, int& a_b, int& a_c, int& a_d) const;

protected:

public:
    TriImage() = delete;
    TriImage(const unsigned char* a_textureData, int a_stepX, int a_stepY, int a_width, int a_height, float a_alphaThreshold);
    ~TriImage();
};