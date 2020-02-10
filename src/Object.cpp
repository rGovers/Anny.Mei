#include "Object.h"

#include <algorithm>
#include <string.h>

#include "Components/ImageRenderer.h"
#include "imgui.h"
#include "Name.h"
#include "PropertyFile.h"
#include "Transform.h"

#define ISCREATECOMPONENT(Comp, Obj, Name, Construct) { if (strcmp(Name, Construct::COMPONENT_NAME) == 0) { Comp = new Construct(Obj); }}

Object::Object(Namer* a_namer) : 
    m_parent(nullptr),
    m_transform(new Transform()),
    m_windowOpen(false)
{
    m_name = new Name("Object", a_namer);
}
Object::~Object()
{
    delete m_transform;

    delete m_name;

    if (m_parent != nullptr)
    {
        m_parent->m_children.remove(this);
    }

    for (auto iter = m_children.begin(); iter != m_children.end(); ++iter)
    {
        (*iter)->m_parent = nullptr;

        delete *iter;
    }

    for (auto iter = m_components.begin(); iter != m_components.end(); ++iter)
    {
        delete *iter;
    }
}

Transform* Object::GetTransform() const
{
    return m_transform;
}

Object* Object::GetParent() const
{
    return m_parent;
}
void Object::SetParent(Object* a_parent)
{
    if (m_parent == a_parent)
    {
        return;
    }

    if (m_parent != nullptr)
    {
        m_parent->m_children.remove(this);
    }

    m_transform->SetParent(a_parent->m_transform);

    m_parent = a_parent;

    a_parent->m_children.emplace_back(this);
}

std::list<Object*> Object::GetChildren() const
{
    return m_children;
}

void Object::SetTrueName(const char* a_trueName)
{
    m_name->SetTrueName(a_trueName);
}
const char* Object::GetTrueName() const
{
    return m_name->GetTrueName();
}
void Object::SetName(const char* a_name)
{
    m_name->SetName(a_name);
}
const char* Object::GetName() const
{
    return m_name->GetName();
}

void Object::LoadComponent(PropertyFileProperty* a_property)
{
    Component* comp = nullptr;

    ISCREATECOMPONENT(comp, this, a_property->GetName(), ImageRenderer)

    if (comp != nullptr)
    {
        comp->Load(a_property);

        m_components.emplace_back(comp);
    }
}
void Object::SaveComponents(PropertyFile* a_propertyFile, PropertyFileProperty* a_parent) const
{
    for (auto iter = m_components.begin(); iter != m_components.end(); ++iter)
    {
        (*iter)->Save(a_propertyFile, a_parent);
    }
}

void Object::UpdateComponentUI()
{
    if (ImGui::Button("Add Component"))
    {
        m_windowOpen = !m_windowOpen;
    }

    if (m_windowOpen)
    {
        ImGui::ListBoxHeader("");

        Component* component = nullptr;

        if (ImGui::Selectable("ImageRenderer"))
        {
            component = new ImageRenderer(this);
        }

        if (component != nullptr)
        {
            m_windowOpen = false;

            m_components.emplace_back(component);
        }

        ImGui::ListBoxFooter();
    }

    int index = 0;

    for (auto iter = m_components.begin(); iter != m_components.end(); ++iter)
    {
        if (ImGui::TreeNode((void*)index, (*iter)->ComponentName()))
        {
            (*iter)->UpdateGUI();

            ImGui::TreePop();
        }

        ++index;
    }
}

void Object::UpdateComponents(bool a_preview, Camera* a_camera, double a_delta)
{
    if (!a_preview)
    {
        for (auto iter = m_components.begin(); iter != m_components.end(); ++iter)
        {
            (*iter)->Update(a_delta, a_camera);
        }
    }
    else
    {
        for (auto iter = m_components.begin(); iter != m_components.end(); ++iter)
        {
            (*iter)->UpdatePreview(a_delta, a_camera);
        }
    }
    
}