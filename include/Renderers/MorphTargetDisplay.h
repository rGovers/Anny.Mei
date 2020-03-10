#pragma once

#include <glm/glm.hpp>

class ShaderProgram;

class MorphTargetDisplay
{
private:
    static unsigned int Ref;
    static ShaderProgram* BaseShaderProgram;
    static ShaderProgram* WireShaderProgram;

    static ShaderProgram* Point9ShaderProgram;
    static ShaderProgram* Point9WireShaderProgram;

    char* m_modelName;
protected:

public:
    MorphTargetDisplay();
    ~MorphTargetDisplay();

    const char* GetModelName() const;
    void SetModelName(const char* a_name);

    void Draw(const glm::mat4& a_transform, const glm::vec2& a_lerp, bool a_alpha = true, bool a_solid = true, bool a_wireframe = false) const;
    void Draw9Point(const glm::mat4& a_transform, const glm::vec2& a_lerp, bool a_alpha = true, bool a_solid = true, bool a_wireframe = false) const;
};