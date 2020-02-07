#include "Object.h"

#include <algorithm>
#include <string.h>

#include "Components/ImageRenderer.h"
#include "imgui.h"
#include "PropertyFile.h"
#include "Transform.h"

#define ISCREATECOMPONENT(Comp, Obj, Name, Construct) { if (strcmp(Name, Construct::COMPONENT_NAME) == 0) { Comp = new Construct(Obj); }}

std::map<std::string, Object::ID>* Object::ObjectNames = nullptr;
std::list<const char*>* Object::NameList = nullptr;

Object::Object() : 
    m_name(nullptr),
    m_trueName(nullptr),
    m_parent(nullptr),
    m_transform(new Transform()),
    m_windowOpen(false)
{
    if (ObjectNames == nullptr)
    {
        ObjectNames = new std::map<std::string, ID>();
    }
    if (NameList == nullptr)
    {
        NameList = new std::list<const char*>();
    }

    SetTrueName("Object");
}
Object::~Object()
{
    delete m_transform;

    auto iter = ObjectNames->find(m_trueName);

    if (iter != ObjectNames->end())
    {
        if (--iter->second.Objects <= 0)
        {
            ObjectNames->erase(iter);
        }
    }

    NameList->remove(m_name);

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

void Object::CreateName(ID& a_id)
{
    const std::string str = std::to_string(a_id.ID);

    const size_t len = strlen(m_trueName);
    const size_t sLen = str.length();

    size_t cLen = len + sLen;

    m_name = new char[cLen];
    strcpy(m_name, m_trueName);
    strcpy(m_name + len, str.c_str());

    a_id.ID++;
    
    bool found = false;

    for (auto iter = NameList->begin(); iter != NameList->end(); ++iter)
    {
        if (strcmp(m_name, *iter) == 0)
        {
            found = true;

            break;
        }
    }

    if (found)
    {
        delete[] m_name;

        CreateName(a_id);
    }
}

void Object::SetTrueName(const char* a_trueName)
{
    if (m_trueName != nullptr)
    {
        if (strcmp(a_trueName, m_trueName) == 0)
        {
            return;
        }

        auto iter = ObjectNames->find(m_trueName);

        if (iter != ObjectNames->end())
        {
            if (--iter->second.Objects <= 0)
            {
                ObjectNames->erase(iter);
            }
        }

        delete[] m_trueName;
        m_trueName = nullptr;
    }
    if (m_name != nullptr)
    {
        NameList->remove(m_name);
        
        delete[] m_name;
        m_name = nullptr;
    }

    if (a_trueName != nullptr)
    {
        const size_t len = strlen(a_trueName);

        if (len > 0)
        {
            m_trueName = new char[len];
            strcpy(m_trueName, a_trueName);
        
            auto iter = ObjectNames->find(m_trueName);

            if (iter == ObjectNames->end())
            {
                m_name = new char[len];
                strcpy(m_name, m_trueName);

                ObjectNames->emplace(m_trueName, ID{ 1, 1 });
            }
            else
            {
                CreateName(iter->second);

                ++iter->second.Objects;
            }

            NameList->emplace_back(m_name);
        }
    }
}
const char* Object::GetTrueName() const
{
    return m_trueName;
}
void Object::SetName(const char* a_name)
{
    if (m_name != nullptr)
    {
        if (strcmp(a_name, m_name) == 0)
        {
            return;
        }

        NameList->remove(m_name);

        delete[] m_name;
        m_name = nullptr;
    }

    if (a_name != nullptr)
    {
        m_name = new char[strlen(a_name)];

        strcpy(m_name, a_name);

        NameList->emplace_back(m_name);
    }
}
const char* Object::GetName() const
{
    return m_name;
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

void Object::UpdateComponents(bool a_preview, double a_delta)
{
    if (!a_preview)
    {
        for (auto iter = m_components.begin(); iter != m_components.end(); ++iter)
        {
            (*iter)->Update(a_delta);
        }
    }
    else
    {
        for (auto iter = m_components.begin(); iter != m_components.end(); ++iter)
        {
            (*iter)->UpdatePreview(a_delta);
        }
    }
    
}