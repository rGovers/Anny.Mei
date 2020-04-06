#include "Transform.h"

#include <glm/common.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <string>
#include <string.h>

#include "AnimControl.h"
#include "FileUtils.h"
#include "Object.h"

Transform::Transform(AnimControl* a_animControl, Object* a_object) 
{
    m_animControl = a_animControl;

    m_parent = nullptr;

    m_object = a_object;

    std::string baseName = std::string("[") + m_object->GetName() + "] ";

    m_translation = new AnimValue<Vec3KeyValue>((baseName + "Translation").c_str(), a_animControl);
    m_translation->SelectKeyFrame(0);
    m_translation->GetValue()->SetBaseValue(glm::vec3(0));

    m_rotation = new AnimValue<QuatKeyValue>((baseName + "Rotation").c_str(), a_animControl);

    m_scale = new AnimValue<Vec3KeyValue>((baseName + "Scale").c_str(), a_animControl);
    m_scale->SelectKeyFrame(0);
    m_scale->GetValue()->SetBaseValue(glm::vec3(1));
}
Transform::~Transform()
{
    delete m_translation;
    delete m_rotation;
    delete m_scale;

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

glm::vec3 Transform::GetTranslation() const
{
    const Vec3KeyValue* value = m_translation->GetAnimValue();

    if (value != nullptr)
    {
        return value->GetValue();
    }

    return glm::vec3(0);
}
glm::fquat Transform::GetRotation() const
{
    const QuatKeyValue* value = m_rotation->GetAnimValue();

    if (value != nullptr)
    {
        return value->GetValue();
    }

    return glm::quat();
}
glm::vec3 Transform::GetScale() const
{
    const Vec3KeyValue* value = m_scale->GetAnimValue();

    if (value != nullptr)
    {
        return value->GetValue();
    }

    return glm::vec3(1);
}

glm::vec3 Transform::GetBaseTranslation() const
{
    const Vec3KeyValue* value = m_translation->GetValue();

    if (value != nullptr)
    {
        return value->GetBaseValue();
    }

    return glm::vec3(0);
}
glm::quat Transform::GetBaseRotation() const
{
    const QuatKeyValue* value = m_rotation->GetValue();

    if (value != nullptr)
    {
        return value->GetBaseValue();
    }

    return glm::quat();
}
glm::vec3 Transform::GetBaseScale() const
{
    const Vec3KeyValue* value = m_scale->GetValue();

    if (value != nullptr)
    {
        return value->GetBaseValue();
    }

    return glm::vec3(1);
}

glm::vec3 Transform::GetWorldPosition() const
{
    glm::mat4 matrix = GetWorldMatrix();

    glm::vec3 translation;
    glm::quat rotation;
    glm::vec3 scale;
    glm::vec3 skew;
    glm::vec4 perspective;

    glm::decompose(matrix, scale, rotation, translation, skew, perspective);

    return translation;
}
glm::quat Transform::GetWorldRotation() const
{
    glm::mat4 matrix = GetWorldMatrix();

    glm::vec3 translation;
    glm::quat rotation;
    glm::vec3 scale;
    glm::vec3 skew;
    glm::vec4 perspective;

    glm::decompose(matrix, scale, rotation, translation, skew, perspective);

    return glm::fquat(rotation);
}
glm::vec3 Transform::GetWorldScale() const
{
    glm::mat4 matrix = GetWorldMatrix();

    glm::vec3 translation;
    glm::quat rotation;
    glm::vec3 scale;
    glm::vec3 skew;
    glm::vec4 perspective;

    glm::decompose(matrix, scale, rotation, translation, skew, perspective);

    return glm::vec3(scale);
}

glm::vec3 Transform::GetBaseWorldPosition() const
{
    glm::mat4 matrix = GetBaseWorldMatrix();

    glm::vec3 translation;
    glm::quat rotation;
    glm::vec3 scale;
    glm::vec3 skew;
    glm::vec4 perspective;

    glm::decompose(matrix, scale, rotation, translation, skew, perspective);

    return translation;
}
glm::fquat Transform::GetBaseWorldRotation() const
{
    glm::mat4 matrix = GetBaseWorldMatrix();

    glm::vec3 translation;
    glm::quat rotation;
    glm::vec3 scale;
    glm::vec3 skew;
    glm::vec4 perspective;

    glm::decompose(matrix, scale, rotation, translation, skew, perspective);

    return glm::fquat(rotation);
}
glm::vec3 Transform::GetBaseWorldScale() const
{
    glm::mat4 matrix = GetBaseWorldMatrix();

    glm::vec3 translation;
    glm::quat rotation;
    glm::vec3 scale;
    glm::vec3 skew;
    glm::vec4 perspective;

    glm::decompose(matrix, scale, rotation, translation, skew, perspective);

    return glm::vec3(scale);
}

void Transform::SetTranslation(const glm::vec3& a_translation)
{
    Vec3KeyValue* value = m_translation->GetValue();

    if (value != nullptr)
    {
        value->SetBaseValue(a_translation);
    }
}
void Transform::SetRotation(const glm::fquat& a_rotation)
{
    QuatKeyValue* value = m_rotation->GetValue();

    if (value != nullptr)
    {
        value->SetBaseValue(a_rotation);
    }
}
void Transform::SetScale(const glm::vec3& a_scale)
{
    Vec3KeyValue* value = m_scale->GetValue();

    if (value != nullptr)
    {
        value->SetBaseValue(a_scale);
    }
}

glm::mat4 Transform::GetRotationMatrix() const
{
    return glm::toMat4(GetRotation());
}
glm::mat4 Transform::GetBaseRotationMatrix() const
{
    return glm::toMat4(GetBaseRotation());
}

glm::mat4 Transform::ToMatrix() const
{
    const glm::mat4 iden = glm::identity<glm::mat4>();

    const glm::mat4 translation = glm::translate(iden, GetTranslation());
    const glm::mat4 scale = glm::scale(iden, GetScale());
    const glm::mat4 rotation = glm::toMat4(GetRotation());

    return translation * rotation * scale;
}
glm::mat4 Transform::GetWorldMatrix() const
{
    if (m_parent != nullptr)
    {
        return m_parent->GetWorldMatrix() * ToMatrix();
    }

    return ToMatrix();
}

glm::mat4 Transform::ToBaseMatrix() const
{
    const glm::mat4 iden = glm::identity<glm::mat4>();

    const glm::mat4 translation = glm::translate(iden, GetBaseTranslation());
    const glm::mat4 scale = glm::scale(iden, GetBaseScale());
    const glm::mat4 rotation = glm::toMat4(GetBaseRotation());

    return translation * rotation * scale;
}
glm::mat4 Transform::GetBaseWorldMatrix() const
{
    if (m_parent != nullptr)
    {
        return m_parent->GetBaseWorldMatrix() * ToBaseMatrix();
    }

    return ToBaseMatrix();
}

void Transform::RefreshValues(double a_time)
{
    m_translation->UpdateAnimValue(a_time);
    m_rotation->UpdateAnimValue(a_time);
    m_scale->UpdateAnimValue(a_time);
}

void Transform::UpdateGUI()
{
    Vec3KeyValue* translationValue = m_translation->GetValue();
    if (translationValue != nullptr)
    {
        translationValue->UpdateGUI("Translation");
    }

    QuatKeyValue* rotationValue = m_rotation->GetValue();
    if (rotationValue != nullptr)
    {
        rotationValue->UpdateGUI("Rotation");
    }

    Vec3KeyValue* scaleValue = m_scale->GetValue();
    if (scaleValue != nullptr)
    {
        scaleValue->UpdateGUI("Scale");
    }
}

void Transform::DisplayValues(bool a_state)
{
    m_translation->SetDisplayState(a_state);
    m_rotation->SetDisplayState(a_state);
    m_scale->SetDisplayState(a_state);
}

void Transform::ObjectRenamed()
{
    std::string baseName = std::string("[") + m_object->GetName() + "] ";

    m_translation->Rename((baseName + "Translation").c_str());
    m_rotation->Rename((baseName + "Rotation").c_str());
    m_scale->Rename((baseName + "Scale").c_str());
}