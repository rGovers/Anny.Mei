#pragma once

#include <glm/glm.hpp>

class ShaderProgram;

class ImageDisplay
{
private:
    static unsigned int Ref;
    static ShaderProgram* BaseShaderProgram;
    static ShaderProgram* WireShaderProgram;

    char* m_modelName;
protected:

public:
    ImageDisplay();
    ~ImageDisplay();

    const char* GetModelName() const;
    void SetModelName(const char* a_name);

    void Draw(const glm::mat4& a_transform, bool a_alpha = true, bool a_solid = true, bool a_wireframe = false) const;
};