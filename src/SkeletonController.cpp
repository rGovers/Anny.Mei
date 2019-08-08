#include "SkeletonController.h"

#include "imgui.h"
#include "Models/Model.h"
#include "Object.h"

SkeletonController::SkeletonController() :
    m_baseObject(new Object()),
    m_selectedObject(nullptr)
{
    m_baseObject->SetName("Root Object");
}

SkeletonController::~SkeletonController()
{
    delete m_baseObject;
}

void SkeletonController::ListObjects(Object* a_object, float a_offset)
{
    ImGui::SetCursorPosX(a_offset);

    const char* name = a_object->GetName();

    if (name == nullptr || name[0] == 0)
    {
        name = "NULL";
    }

    if (ImGui::Button(name))
    {
        m_selectedObject = a_object;
    }

    const std::list<Object*> children = a_object->GetChildren();

    for (auto iter = children.begin(); iter != children.end(); ++iter)
    {
        ListObjects(*iter, a_offset + 10.0f);
    }
}

void SkeletonController::Update(double a_delta)
{
    if (m_selectedObject != nullptr)
    {
        ImGui::SetNextWindowSize({ 200, 400 }, ImGuiCond_Appearing);
        if (ImGui::Begin("Object Properties"))
        {
            static const int BUFFER_SIZE = 1024;

            char* buffer = new char[BUFFER_SIZE];
            buffer[0] = 0;
            
            const char* name = m_selectedObject->GetName();

            if (name != nullptr)
            {
                strcpy(buffer, m_selectedObject->GetName());
            }

            ImGui::InputText("Name", buffer, BUFFER_SIZE);

            m_selectedObject->SetName(buffer);

            if (ImGui::Button("Add Child Object"))
            {
                Object* object = new Object();
                object->SetParent(m_selectedObject);
            }

            if (m_selectedObject != m_baseObject)
            {
                if (ImGui::Button("Remove Object"))
                {
                    delete m_selectedObject;

                    m_selectedObject = nullptr;
                }
            }

            ImGui::End();
        }
    }

    ImGui::SetNextWindowSize({ 200, 600 }, ImGuiCond_Appearing);
    if (ImGui::Begin("Skeleton Hierarchy"))
    {
        ListObjects(m_baseObject, 0.0f);

        ImGui::End();
    }
}

void SkeletonController::SetModel(int a_index, Model* a_model)
{
    auto iter = m_models.find(a_index);

    if (iter != m_models.end())
    {
        iter->second = a_model;
    }
    else
    {
        m_models.emplace(a_index, a_model);
    }
}

SkeletonController* SkeletonController::Load(ZipArchive::Ptr a_archive)
{
    SkeletonController* skeletonController = new SkeletonController();

    return skeletonController;
}