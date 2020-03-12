#pragma once

#include <glm/glm.hpp>
#include <list>

struct LayerMeta;
struct ModelVertex;

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
    ModelVertex*         m_verts;
    
    unsigned int*        m_indicies;
    unsigned int         m_indexCount;

    const unsigned char* m_textureData;

    glm::ivec2           m_size;
    glm::ivec2           m_offset;

    glm::ivec2           m_imageSize;
                 
    void WindTriangle(unsigned int& a_a, unsigned int& a_b, unsigned int& a_c) const;
    void WindQuad(unsigned int& a_a, unsigned int& a_b, unsigned int& a_c, unsigned int& a_d) const;

    void VoronoiTriangulation(int a_vWidth, int a_vHeight);

    bool PlaceAlphaVertex(std::list<ModelVertex>* a_verts, int a_x, int a_y, int a_texXStep, int a_texYStep, float a_alpha) const;
protected:

public:
    TriImage() = delete;
    TriImage(const unsigned char* a_textureData, const LayerMeta* a_meta);
    ~TriImage();

    void AlphaTriangulation(int a_texXStep, int a_texYStep, float a_alphaThreshold, int a_vWidth, int a_vHeight);
    void QuadTriangulation(int a_stepX, int a_stepY, int a_texXStep, int a_texYStep, float a_alphaThreshold, int a_vWidth, int a_vHeight);
    void OutlineTriangulation(float a_channelDiff, int a_texXStep, int a_texYStep, float a_alphaThreshold, int a_vWidth, int a_vHeight);

    unsigned int GetIndexCount() const;
    unsigned int* GetIndices() const;

    unsigned int GetVertexCount() const;

    ModelVertex* GetVertices() const;
};