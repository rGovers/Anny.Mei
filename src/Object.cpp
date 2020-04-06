#include "Object.h"

#include <algorithm>
#include <string.h>

#include "AnimControl.h"
#include "Components/ImageMask.h"
#include "Components/ImageRenderer.h"
#include "Components/MorphPlaneMask.h"
#include "Components/MorphPlaneRenderer.h"
#include "Components/MorphTargetMask.h"
#include "Components/MorphTargetRenderer.h"
#include "Components/MouseScale.h"
#include "Components/MouseTranslation.h"
#include "FileLoaders/PropertyFile.h"
#include "imgui.h"
#include "Name.h"
#include "Transform.h"

#define ISCREATECOMPONENT(Comp, Obj, Name, Construct, AControl) if (strcmp(Name, Construct::COMPONENT_NAME) == 0) { Comp = new Construct(Obj, AControl); Comp->Init(); }

Object::Object(Namer* a_namer, AnimControl* a_animControl) 
{
    m_animControl = a_animControl;

    m_windowOpen = false;

    m_parent = nullptr;

    m_name = new Name("Object", a_namer);
    
    m_transform = new Transform(m_animControl, this);
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

    m_transform->ObjectRenamed();

    for (auto iter = m_components.begin(); iter != m_components.end(); ++iter)
    {
        (*iter)->ObjectRenamed();
    }
}
const char* Object::GetTrueName() const
{
    return m_name->GetTrueName();
}
void Object::SetName(const char* a_name)
{
    m_name->SetName(a_name);

    m_transform->ObjectRenamed();

    for (auto iter = m_components.begin(); iter != m_components.end(); ++iter)
    {
        (*iter)->ObjectRenamed();
    }
}
const char* Object::GetName() const
{
    return m_name->GetName();
}

void Object::DisplayValues(bool a_state)
{
    m_transform->DisplayValues(a_state);

    for (auto iter = m_components.begin(); iter != m_components.end(); ++iter)
    {
        (*iter)->DisplayValues(a_state);
    }
}

void Object::MoveChildUp(Object* a_child)
{
    for (auto iter = m_children.begin(); iter != m_children.end(); ++iter)
    {
        if (*iter == a_child)
        {
            auto curItem = iter;

            --iter;

            m_children.erase(curItem);
            m_children.emplace(iter, a_child);

            return;
        }
    }
}
void Object::MoveChildDown(Object* a_child)
{
    for (auto iter = m_children.begin(); iter != m_children.end(); ++iter)
    {
        if (*iter == a_child)
        {
            auto curItem = iter;

            ++++iter;

            m_children.erase(curItem);
            m_children.emplace(iter, a_child);

            return;
        }
    }
}

void Object::LoadComponent(PropertyFileProperty* a_property)
{
    Component* comp = nullptr;

    const char* propName = a_property->GetName();

    ISCREATECOMPONENT(comp, this, propName, ImageMask, m_animControl)
    else ISCREATECOMPONENT(comp, this, propName, ImageRenderer, m_animControl)
    else ISCREATECOMPONENT(comp, this, propName, MorphPlaneMask, m_animControl)
    else ISCREATECOMPONENT(comp, this, propName, MorphPlaneRenderer, m_animControl)
    else ISCREATECOMPONENT(comp, this, propName, MorphTargetMask, m_animControl)
    else ISCREATECOMPONENT(comp, this, propName, MorphTargetRenderer, m_animControl)
    else ISCREATECOMPONENT(comp, this, propName, MouseScale, m_animControl)
    else ISCREATECOMPONENT(comp, this, propName, MouseTranslation, m_animControl)

    if (comp != nullptr)
    {
        comp->Load(a_property, m_animControl);

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

        bool createImageMask = true;
        bool createImageRenderer = true;
        bool createMorphPlaneMask = true;
        bool createMorphPlaneRenderer = true;
        bool cerateMorphTargetMask = true;
        bool createMorphTargetRenderer = true;
        bool createMouseScale = true;
        bool createMouseTranslation = true;

        for (auto iter = m_components.begin(); iter != m_components.end(); ++iter)
        {
            const char* componentName = (*iter)->ComponentName();

            if (strcmp(componentName, ImageMask::COMPONENT_NAME) == 0)
            {
                createImageMask = false;
            }
            else if (strcmp(componentName, ImageRenderer::COMPONENT_NAME) == 0)
            {
                createImageRenderer = false;
            }
            else if (strcmp(componentName, MorphPlaneMask::COMPONENT_NAME) == 0)
            {
                createMorphPlaneMask = false;
            }
            else if (strcmp(componentName, MorphPlaneRenderer::COMPONENT_NAME) == 0)
            {
                createMorphPlaneRenderer = false;
            }
            else if (strcmp(componentName, MorphTargetMask::COMPONENT_NAME) == 0)
            {
                cerateMorphTargetMask = false;
            }
            else if (strcmp(componentName, MorphTargetRenderer::COMPONENT_NAME) == 0)
            {
                createMorphTargetRenderer = false;
            }
            else if (strcmp(componentName, MouseScale::COMPONENT_NAME) == 0)
            {
                createMouseScale = false;
            }
            else if (strcmp(componentName, MouseTranslation::COMPONENT_NAME) == 0)
            {
                createMouseTranslation = false;
            }
        }

        if (createImageMask && ImGui::Selectable(ImageMask::COMPONENT_NAME))
        {
            component = new ImageMask(this, m_animControl);
        }
        if (createImageRenderer && ImGui::Selectable(ImageRenderer::COMPONENT_NAME))
        {
            component = new ImageRenderer(this, m_animControl);
        }
        if (createMorphPlaneMask && ImGui::Selectable(MorphPlaneMask::COMPONENT_NAME))
        {
            component = new MorphPlaneMask(this, m_animControl);
        }
        if (createMorphPlaneRenderer && ImGui::Selectable(MorphPlaneRenderer::COMPONENT_NAME))
        {
            component = new MorphPlaneRenderer(this, m_animControl);
        }
        if (cerateMorphTargetMask && ImGui::Selectable(MorphTargetMask::COMPONENT_NAME))
        {
            component = new MorphTargetMask(this, m_animControl);
        }
        if (createMorphTargetRenderer && ImGui::Selectable(MorphTargetRenderer::COMPONENT_NAME))
        {
            component = new MorphTargetRenderer(this, m_animControl);
        }
        if (createMouseScale && ImGui::Selectable(MouseScale::COMPONENT_NAME))
        {
            component = new MouseScale(this, m_animControl);
        }
        if (createMouseTranslation && ImGui::Selectable(MouseTranslation::COMPONENT_NAME))
        {
            component = new MouseTranslation(this, m_animControl);
        }

        if (component != nullptr)
        {
            component->Init();

            component->DisplayValues(true);

            m_windowOpen = false;

            m_components.emplace_back(component);
        }

        ImGui::ListBoxFooter();
    }

    auto removeIter = m_components.end();

    bool gen = false;

    int index = 0;
    for (auto iter = m_components.begin(); iter != m_components.end(); ++iter)
    {
        if (ImGui::TreeNode((void*)index, (*iter)->ComponentName()))
        {
            if (!gen && ImGui::BeginPopupContextItem("NYAH HAHA REMOVE ME FOR NO ID CRASHES!!!"))
            {
                gen = true;

                if (ImGui::MenuItem("Remove Component"))
                {
                    removeIter = iter;
                }

                ImGui::EndPopup();
            }

            (*iter)->UpdateGUI();

            ImGui::TreePop();
        }
        else
        {
            // Seems to be a 50/50 that if I remove the string that it will either assert or combo boxes will stop working
            // I have no idea
            // Tried push id does not work
            // Dont need the string elsewhere
            // Cant find exact cause to report
            if (!gen && ImGui::BeginPopupContextItem("NYAH HAHA REMOVE ME FOR NO ID CRASHES!!!"))
            {
                // Stop Duplicate Buttons
                gen = true;

                if (ImGui::MenuItem("Remove Component"))
                {
                    removeIter = iter;
                }

                ImGui::EndPopup();
            }
        }

        ++index;
    }

    if (removeIter != m_components.end())
    {
        delete *removeIter;

        m_components.erase(removeIter);
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