#include "Components/Component.h"

#include "AnimControl.h"
#include "Object.h"

Component::Component(Object* a_object, AnimControl* a_animControl)
{
    m_object = a_object;
    m_animControl = a_animControl;   
}
Component::~Component()
{

}

Object* Component::GetObject() const
{
    return m_object;
}

AnimControl* Component::GetAnimControl() const
{
    return m_animControl;
}