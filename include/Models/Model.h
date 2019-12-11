#pragma once

#define GLM_SWIZZLE
#include <glm/glm.hpp>

enum class e_ModelType
{
    Base,
    Skinned
};

struct ModelVertex
{
    glm::vec4 Position;
    glm::vec2 TexCoord;
};

class Model
{
private:
    const static int IBO = 0;
    const static int VBO = 1;

    unsigned int* m_buffers;
    unsigned int  m_vao;

    unsigned int  m_indicies;
    unsigned int  m_verticies;
protected:

public:
    Model();
    ~Model();

    unsigned int GetVAO() const;
    unsigned int GetVBO() const;
    unsigned int GetIBO() const;

    void SetVerticiesCount(unsigned int a_verticies);
    unsigned int GetVerticiesCount() const;

    void SetIndiciesCount(unsigned int a_indicies);
    unsigned int GetIndiciesCount() const;
};