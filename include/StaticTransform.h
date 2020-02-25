#pragma once

#include <glm/gtc/quaternion.hpp>
#include <glm/glm.hpp>

class StaticTransform
{
private:
    glm::vec3                   m_translation;
    glm::vec3                   m_scale;

    glm::quat                   m_rotation;
protected:

public:
    StaticTransform();
    ~StaticTransform();

    glm::vec3 GetTranslation() const;
    glm::fquat GetRotation() const;
    glm::vec3 GetScale() const;

    void SetTranslation(const glm::vec3& a_translation);
    void SetRotation(const glm::fquat& a_rotation);
    void SetScale(const glm::vec3& a_scale);

    glm::mat4 GetRotationMatrix() const;

    glm::mat4 ToMatrix() const;
};