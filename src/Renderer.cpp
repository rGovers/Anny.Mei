#include "Components/Renderer.h"

Renderer::Renderer(Object* a_object) :
    Component(a_object)
{
}

Renderer::~Renderer()
{
}

const char* Renderer::ComponentName() const
{
    return "Renderer";
}