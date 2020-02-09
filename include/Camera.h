#pragma once

#define GLM_SWIZZLE
#include <glm/glm.hpp>

class Transform;

class Camera
{
private:
    glm::mat4  m_projection;
    Transform* m_transform;
protected:

public:
    Camera();
    ~Camera();

    Transform* GetTransform() const;

    void SetProjection(const glm::mat4& a_matrix);
    glm::mat4 GetProjection() const;
};
