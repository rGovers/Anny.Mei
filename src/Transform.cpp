#include "Transform.h"

#include <glm/common.hpp>
#include <glm/gtx/quaternion.hpp>
#include <string>
#include <string.h>

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

float StringToFloat(const char* a_stringStart, const char* a_stringEnd)
{
    int diff = a_stringEnd - a_stringStart;

    char* tmp = new char[diff];
    memcpy(tmp, a_stringStart + 1, diff);
    tmp[diff - 1] = 0;

    float val = std::stof(tmp);

    delete[] tmp;

    return val;
}

void Transform::Parse(const char* a_string)
{
    // Got lazy so I am just going along finding characters and using pointer
    // offsets
    char* str = (char*)a_string;
    str = strchr(str, '{');
    str = strchr(str + 1, '{');

    char* endStr = strchr(str, ',');
    
    m_translation.x = StringToFloat(str, endStr);
    
    str = endStr;
    endStr = strchr(str + 1, ',');    
    m_translation.y = StringToFloat(str, endStr);

    str = endStr;
    endStr = strchr(str + 1, '}');
    m_translation.z = StringToFloat(str, endStr);

    str = strchr(endStr + 1, '{');
    endStr = strchr(str + 1, ',');
    m_rotation.x = StringToFloat(str, endStr);

    str = endStr;
    endStr = strchr(str + 1, ',');
    m_rotation.y = StringToFloat(str, endStr);

    str = endStr;
    endStr = strchr(str + 1, ',');
    m_rotation.z = StringToFloat(str, endStr);

    str = endStr;
    endStr = strchr(str + 1, '}');
    m_rotation.w = StringToFloat(str, endStr);

    str = strchr(endStr + 1, '{');
    endStr = strchr(str + 1, ',');
    m_scale.x = StringToFloat(str, endStr);

    str = endStr;
    endStr = strchr(str + 1, ',');
    m_scale.y = StringToFloat(str, endStr);

    str = endStr;
    endStr = strchr(str + 1, '}');
    m_scale.z = StringToFloat(str, endStr);
}
char* Transform::ToString() const
{
    std::string str;

    str += std::string("{ { ");
    str += std::to_string(m_translation.x) + ", ";
    str += std::to_string(m_translation.y) + ", ";
    str += std::to_string(m_translation.z) + " }";

    str += " { ";
    str += std::to_string(m_rotation.x) + ", ";
    str += std::to_string(m_rotation.y) + ", ";
    str += std::to_string(m_rotation.z) + ", ";
    str += std::to_string(m_rotation.w) + " }";
 
    str += " { ";
    str += std::to_string(m_scale.x) + ", ";
    str += std::to_string(m_scale.y) + ", ";
    str += std::to_string(m_scale.z) + " } }";

    const size_t len = str.length();
    
    char* cStr = new char[len];
    memset(cStr, 0, len);
    strcpy(cStr, str.c_str());

    return cStr;
}