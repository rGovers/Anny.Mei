#include "Transform.h"

#include <glm/common.hpp>
#include <glm/gtx/quaternion.hpp>

Transform::Transform() : 
    m_translation(glm::vec3(0)),
    m_scale(glm::vec3(1)),
    m_rotation(glm::identity<glm::quat>()),
    m_parent(nullptr)
{

}
Transform::~Transform()
{
    if (m_parent != nullptr)
    {
        m_parent->m_children.remove(this);
    }

    for (auto iter = m_children.begin(); iter != m_children.end(); ++iter)
    {
        (*iter)->m_parent = nullptr;
    }
}

void Transform::SetParent(Transform* a_parent)
{
    if (m_parent != nullptr)
    {
        m_parent->m_children.remove(this);
    }

    m_parent = a_parent;

    if (m_parent != nullptr)
    {
        m_parent->m_children.emplace_back(this);
    }
}
Transform* Transform::GetParent() const
{
    return m_parent;
}

std::list<Transform*> Transform::GetChildren() const
{
    return m_children;
}

glm::vec3& Transform::Translation()
{
    return m_translation;
}
glm::fquat& Transform::Rotation()
{
    return m_rotation;
}
glm::vec3& Transform::Scale()
{
    return m_scale;
}

void Transform::SetTranslation(const glm::vec3& a_translation)
{
    m_translation = a_translation;
}
void Transform::SetRotation(const glm::fquat& a_rotation)
{
    m_rotation = a_rotation;
}
void Transform::SetScale(const glm::vec3& a_scale)
{
    m_scale = a_scale;
}

glm::mat4 Transform::GetRotationMatrix() const
{
    return glm::toMat4(m_rotation);
}

glm::mat4 Transform::ToMatrix() const
{
    const glm::mat4 iden = glm::identity<glm::mat4>();

    const glm::mat4 translation = glm::translate(iden, m_translation);
    const glm::mat4 scale = glm::scale(iden, m_scale);
    const glm::mat4 rotation = glm::toMat4(m_rotation);

    return scale * rotation* translation;
}