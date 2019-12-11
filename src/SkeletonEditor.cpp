#include "SkeletonEditor.h"

#include <list>
#include <string>

#include "imgui.h"
#include "MemoryStream.h"
#include "Models/Model.h"
#include "Object.h"
#include "PropertyFile.h"
#include "Transform.h"

SkeletonEditor::SkeletonEditor() :
    m_baseObject(new Object()),
    m_selectedObject(nullptr)
{
    m_baseObject->SetTrueName("Root Object");
}

SkeletonEditor::~SkeletonEditor()
{
    delete m_baseObject;
}

void SkeletonEditor::ListObjects(Object* a_object, int& a_node)
{
    const char* name = a_object->GetName();

    if (name == nullptr || name[0] == 0)
    {
        name = "NULL";
    }

    const std::list<Object*> children = a_object->GetChildren();

    bool open = false;
    bool selected = false;

    if (children.size() > 0)
    {
        open = ImGui::TreeNode((void*)a_node, "");
        ImGui::SameLine();
    }
    else
    {
        ImGui::TreeAdvanceToLabelPos();
    }
    
    ImGui::Selectable(name, &selected);

    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Add Object"))
        {
            Object* object = new Object();
            object->SetParent(a_object);
        }

        if (a_object != m_baseObject)
        {
            if (ImGui::MenuItem("Remove Object"))
            {
                delete a_object;

                ImGui::EndPopup();

                if (open)
                {
                    ImGui::TreePop();
                }

                return;
            }
        }
        
        ImGui::EndPopup();
    }

    if (selected)
    {
        m_selectedObject = a_object;
    }

    ++a_node;

    if (open)
    {
        for (auto iter = children.begin(); iter != children.end(); ++iter)
        {
            ListObjects(*iter, a_node);
        }

        ImGui::TreePop();
    }   
}

void SkeletonEditor::Update(double a_delta)
{
    if (m_selectedObject != nullptr)
    {
        ImGui::SetNextWindowSize({ 200, 400 }, ImGuiCond_Appearing);
        if (ImGui::Begin("Object Properties"))
        {
            static const int BUFFER_SIZE = 1024;

            char* buffer = new char[BUFFER_SIZE];
            buffer[0] = 0;
            
            const char* name = m_selectedObject->GetTrueName();

            if (name != nullptr)
            {
                strcpy(buffer, name);
            }

            ImGui::InputText("Name", buffer, BUFFER_SIZE);

            ImGui::Separator();

            Transform* transform = m_selectedObject->GetTransform();

            ImGui::InputFloat3("Translation", (float*)&transform->Translation(), 4);

            glm::fquat quat = transform->Rotation();
            ImGui::InputFloat4("Rotation", (float*)&quat, 4);
            transform->SetRotation(glm::normalize(quat));

            ImGui::InputFloat3("Scale", (float*)&transform->Scale(), 4);

            m_selectedObject->SetTrueName(buffer);

            ImGui::Separator();

            m_selectedObject->UpdateComponentUI();

            ImGui::End();
        }
    }

    if (m_baseObject != nullptr)
    {
        ImGui::SetNextWindowSize({ 200, 600 }, ImGuiCond_Appearing);
        if (ImGui::Begin("Skeleton Hierarchy"))
        {
            int node = 0;
        
            ListObjects(m_baseObject, node);

            ImGui::End();
        }
    }
}

SkeletonEditor* SkeletonEditor::Load(mz_zip_archive& a_archive)
{
    SkeletonEditor* skeletonController = new SkeletonEditor();

    return skeletonController;
}
void SkeletonEditor::Save(mz_zip_archive& a_archive) const
{
    
}