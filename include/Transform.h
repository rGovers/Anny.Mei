#pragma once

#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <list>

class Transform
{
private:
    glm::vec3             m_translation;
    glm::vec3             m_scale;

    glm::quat             m_rotation;
        
    Transform*            m_parent;
    std::list<Transform*> m_children;
protected:

public:
    Transform();
    ~Transform();

    void SetParent(Transform* a_parent);
    Transform* GetParent() const;

    std::list<Transform*> GetChildren() const;

    glm::vec3& Translation();
    glm::fquat& Rotation();
    glm::vec3& Scale();

    glm::vec3 GetWorldPosition() const;
    glm::fquat GetWorldRotation() const;
    glm::vec3 GetWorldScale() const;

    void SetTranslation(const glm::vec3& a_translation);
    void SetRotation(const glm::fquat& a_rotation);
    void SetScale(const glm::vec3& a_scale);

    glm::mat4 GetRotationMatrix() const;

    glm::mat4 ToMatrix() const;
    glm::mat4 GetWorldMatrix() const;

    void Parse(const char* a_string);
    char* ToString() const;
};