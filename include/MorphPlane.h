#pragma once

#include <glm/glm.hpp>

#include "miniz.h"

class Texture;

class MorphPlane
{
private:
    unsigned int m_dimensions;

    glm::vec2*   m_morphPos;
protected:
    MorphPlane();

public:
    MorphPlane(unsigned int a_dimensions);
    ~MorphPlane();

    glm::vec2 GetMorphPosition(unsigned int a_x, unsigned int a_y) const;
    void SetMorphPosition(const glm::vec2 a_value, unsigned int a_x, unsigned int a_y);

    glm::vec2* GetMorphPositions() const;
    
    unsigned int GetSize() const;

    void Resize(unsigned int a_newSize);

    Texture* ToTexture() const;

    static MorphPlane* Load(const char* a_fileName, mz_zip_archive& a_archive, unsigned int a_size);

    MorphPlane* Lerp(float a_lerp, const MorphPlane& a_left, const MorphPlane& a_right) const;
    MorphPlane* Lerp5(const glm::vec2& a_lerp, const MorphPlane& a_left, const MorphPlane& a_right, const MorphPlane& a_up, const MorphPlane& a_down) const;
};