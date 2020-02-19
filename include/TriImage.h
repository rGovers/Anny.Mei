#pragma once

#include "Models/Model.h"
#include "Models/SkinnedModel.h"

class Texture;

struct TriImageTriangle
{
    unsigned int A, B, C;

    bool operator ==(const TriImageTriangle& a_triB) const
    {
        return A == a_triB.A && B == a_triB.B && C == a_triB.C;
    }
};

class TriImage
{
private:
    unsigned int         m_vertCount;
    glm::vec2*           m_verts;
    
    unsigned int*        m_indicies;
    unsigned int         m_indexCount;

    const unsigned char* m_textureData;
    int                  m_width;
    int                  m_height;

    void WindTriangle(unsigned int& a_a, unsigned int& a_b, unsigned int& a_c) const;
    void WindQuad(unsigned int& a_a, unsigned int& a_b, unsigned int& a_c, unsigned int& a_d) const;

    void VoronoiTriangulation(int a_vWidth, int a_vHeight);
protected:

public:
    TriImage() = delete;
    TriImage(const unsigned char* a_textureData, int a_width, int a_height);
    ~TriImage();

    void AlphaTriangulation(int a_texXStep, int a_texYStep, float a_alphaThreshold, int a_vWidth, int a_vHeight);
    void QuadTriangulation(int a_stepX, int a_stepY, int a_texXStep, int a_texYStep, float a_alphaThreshold, int a_vWidth, int a_vHeight);
    void OutlineTriangulation(float a_channelDiff, int a_texXStep, int a_texYStep, float a_alphaThreshold, int a_vWidth, int a_vHeight);

    unsigned int GetIndexCount() const;
    unsigned int* GetIndices() const;

    unsigned int GetVertexCount() const;

    ModelVertex* ToModelVertices() const;
};