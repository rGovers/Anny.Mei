#pragma once

#include <glm/glm.hpp>

#include "Model.h"

struct SkinnedVertex
{
    glm::vec4 Position;
    glm::vec2 TexCoord;
    glm::ivec2 Index;
    glm::vec2 Weight;
};

class SkinnedModel : public Model
{
private:

protected:

public:
    SkinnedModel();


};