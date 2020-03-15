#pragma once

#include <glm/glm.hpp>

class MorphPlane;
class ShaderProgram;

class MorphPlaneDisplay
{
private:
    static unsigned int Ref;
    static ShaderProgram* BaseShaderProgram;
    static ShaderProgram* MaskShaderProgram;
    static ShaderProgram* WireShaderProgram;

    char* m_modelName;
    char* m_morphPlaneName;
    char* m_maskName;
protected:

public:
    MorphPlaneDisplay();
    ~MorphPlaneDisplay();

    const char* GetModelName() const;
    void SetModelName(const char* a_name);

    const char* GetMorphPlaneName() const;
    void SetMorphPlaneName(const char* a_name);

    const char* GetMaskName() const;
    void SetMaskName(const char* a_name);

    void Draw(const glm::mat4& a_transform, bool a_alpha = true, bool a_solid = true, bool a_wireframe = false) const;
    void Draw(const MorphPlane* a_morphPlane, const glm::mat4& a_transform, bool a_alpha = true, bool a_solid = true, bool a_wireframe = false) const;

    void DrawMasked(const glm::mat4& a_transform) const;
    void DrawMasked(const MorphPlane* a_morphPlane, const glm::mat4& a_transform) const;
};