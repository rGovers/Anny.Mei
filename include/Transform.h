#pragma once

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

    inline glm::vec3& Translation();
    inline glm::vec3& Scale();
    inline glm::quat& Rotation();

    inline glm::mat4 GetRotationMatrix() const;

    glm::mat4 ToMatrix() const;
};