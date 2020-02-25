#include "StaticTransform.h"

#include <glm/gtx/quaternion.hpp>

StaticTransform::StaticTransform()
{
    m_translation = glm::vec3(0);
    m_scale = glm::vec3(1);
    m_rotation = glm::identity<glm::quat>();
}
StaticTransform::~StaticTransform()
{

}

glm::vec3 StaticTransform::GetTranslation() const
{
    return m_translation;
}
glm::fquat StaticTransform::GetRotation() const
{
    return m_rotation;
}  
glm::vec3 StaticTransform::GetScale() const
{
    return m_scale;
}

void StaticTransform::SetTranslation(const glm::vec3& a_translation)
{
    m_translation = a_translation;
}
void StaticTransform::SetRotation(const glm::fquat& a_rotation)
{
    m_rotation = a_rotation;
}
void StaticTransform::SetScale(const glm::vec3& a_scale)
{
    m_scale = a_scale;
}

glm::mat4 StaticTransform::GetRotationMatrix() const
{
    return glm::toMat4(m_rotation);
}

glm::mat4 StaticTransform::ToMatrix() const
{
    const glm::mat4 iden = glm::identity<glm::mat4>();

    const glm::mat4 translation = glm::translate(iden, m_translation);
    const glm::mat4 scale = glm::scale(iden, m_scale);
    const glm::mat4 rotation = glm::toMat4(m_rotation);

    return translation * rotation * scale;
}