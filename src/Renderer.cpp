#include "Components/Renderer.h"

#include "imgui.h"

Renderer::Renderer(Object* a_object) :
    Component(a_object)
{

}

Renderer::~Renderer()
{

}

void Renderer::Update()
{

}
void Renderer::UpdateGUI()
{
    ImGui::Text("Test");
}

const char* Renderer::ComponentName() const
{
    return "Renderer";
}