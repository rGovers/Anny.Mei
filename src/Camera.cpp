#include "Camera.h"

#include "StaticTransform.h"

Camera::Camera()
{
    m_transform = new StaticTransform();

    m_projection = glm::mat4(1);
}
Camera::~Camera()
{  
    delete m_transform;
}

StaticTransform* Camera::GetTransform() const
{
    return m_transform;
}

void Camera::SetProjection(const glm::mat4& a_matrix)
{
    m_projection = a_matrix;
}
glm::mat4 Camera::GetProjection() const
{
    return m_projection;
}