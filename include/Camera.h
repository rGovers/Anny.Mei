#pragma once

#include <glm/glm.hpp>

class StaticTransform;

class Camera
{
private:
    glm::mat4  m_projection;
    StaticTransform* m_transform;
protected:

public:
    Camera();
    ~Camera();

    StaticTransform* GetTransform() const;

    void SetProjection(const glm::mat4& a_matrix);
    glm::mat4 GetProjection() const;
};
