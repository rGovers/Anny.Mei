#include "Components/Component.h"

#include "Object.h"

Component::Component(Object* a_object) :
    m_object(a_object)
{
    
}
Component::~Component()
{

}

Object* Component::GetObject() const
{
    return m_object;
}