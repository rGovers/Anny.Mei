#pragma once

#include <glm/gtc/quaternion.hpp>
#include <glm/glm.hpp>
#include <list>

#include "AnimValue.h"
#include "KeyValues/QuatKeyValue.h"
#include "KeyValues/Vec3KeyValue.h"

class AnimControl;
class Object;

class Transform
{
private:
    AnimControl*             m_animControl;
 
    Object*                  m_object;

    AnimValue<Vec3KeyValue>* m_translation;
    AnimValue<Vec3KeyValue>* m_scale;

    AnimValue<QuatKeyValue>* m_rotation;
             
    Transform*               m_parent;
    std::list<Transform*>    m_children;
protected:

public:
    Transform(AnimControl* a_animControl, Object* a_object);
    ~Transform();

    void SetParent(Transform* a_parent);
    Transform* GetParent() const;

    std::list<Transform*> GetChildren() const;

    glm::vec3 GetTranslation() const;
    glm::fquat GetRotation() const;
    glm::vec3 GetScale() const;

    glm::vec3 GetBaseTranslation() const;
    glm::fquat GetBaseRotation() const;
    glm::vec3 GetBaseScale() const;

    glm::vec3 GetWorldPosition() const;
    glm::fquat GetWorldRotation() const;
    glm::vec3 GetWorldScale() const;

    glm::vec3 GetBaseWorldPosition() const;
    glm::fquat GetBaseWorldRotation() const;
    glm::vec3 GetBaseWorldScale() const;

    void SetTranslation(const glm::vec3& a_translation);
    void SetRotation(const glm::fquat& a_rotation);
    void SetScale(const glm::vec3& a_scale);

    glm::mat4 GetRotationMatrix() const;
    glm::mat4 GetBaseRotationMatrix() const;

    glm::mat4 ToMatrix() const;
    glm::mat4 GetWorldMatrix() const;

    glm::mat4 ToBaseMatrix() const;
    glm::mat4 GetBaseWorldMatrix() const;

    void RefreshValues(double a_time);

    void UpdateGUI();

    void DisplayValues(bool a_state);

    void ObjectRenamed();
};