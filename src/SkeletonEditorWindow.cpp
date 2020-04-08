#include "WindowControls/SkeletonEditorWindow.h"

#include "Camera.h"
#include "imgui.h"
#include "Object.h"
#include "SkeletonEditor.h"
#include "StaticTransform.h"
#include "Texture.h"
#include "Transform.h"
#include "Workspace.h"

static const int BUFFER_SIZE = 1024;
static char* BUFFER = new char[BUFFER_SIZE];

const static int IMAGE_SIZE = 4096;

const static float MAX_ZOOM = 2.5f;
const static float MOUSE_SENSITIVITY = 0.005f;
const static float MOUSE_WHEEL_SENSITIVITY = 0.1f;

SkeletonEditorWindow::SkeletonEditorWindow(SkeletonEditor* a_skeletonEditor)
{
    m_skeletonEditor = a_skeletonEditor;

    m_selectedObject = nullptr;

    m_camera = new Camera();

    m_lastMousePos = glm::vec2(0);

    m_zoom = 1;
}
SkeletonEditorWindow::~SkeletonEditorWindow()
{
    delete m_camera;
}

void SkeletonEditorWindow::ListObjects(Object* a_object, int& a_node)
{
    const char* name = a_object->GetName();

    if (name == nullptr || name[0] == 0)
    {
        name = "NULL";
    }

    const std::list<Object*> children = a_object->GetChildren();

    bool open = false;
    bool is_selected = (m_selectedObject == a_object);

    if (children.size() > 0)
    {
        open = ImGui::TreeNode((void*)a_node, "");
        ImGui::SameLine();
    }
    else
    {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetTreeNodeToLabelSpacing());
    }
    
    if (ImGui::Selectable(name, &is_selected))
    {
        if (m_selectedObject != nullptr)
        {
            m_selectedObject->DisplayValues(false);
        }

        m_selectedObject = a_object;

        if (m_selectedObject != nullptr)
        {
            m_selectedObject->DisplayValues(true);
        }

        m_skeletonEditor->GetWorkspace()->SelectWorkspace(m_skeletonEditor);
    }

    if (is_selected)
    {
        ImGui::SetItemDefaultFocus();
    }

    if (ImGui::BeginPopupContextItem())
    {
        Namer* namer = m_skeletonEditor->GetNamer();
        AnimControl* animControl = m_skeletonEditor->GetAnimControl();
        Object* baseObject = m_skeletonEditor->GetBaseObject();

        if (ImGui::MenuItem("Add Object"))
        {
            Object* object = new Object(namer, animControl);
            object->SetParent(a_object);
        }

        if (a_object != baseObject)
        {
            if (ImGui::MenuItem("Remove Object"))
            {
                if (a_object == m_selectedObject)
                {
                    m_selectedObject = nullptr;
                }

                delete a_object;

                ImGui::EndPopup();

                if (open)
                {
                    ImGui::TreePop();
                }

                return;
            }

            Object* parent = a_object->GetParent();

            if (parent != nullptr)
            {
                const std::list<Object*> children = parent->GetChildren();

                if (children.size() > 1)
                {
                    ImGui::Separator();

                    if (a_object != *children.begin() && ImGui::MenuItem("Move Up"))
                    {
                        parent->MoveChildUp(a_object);
                    }

                    if (a_object != *--children.end() && ImGui::MenuItem("Move Down"))
                    {
                        parent->MoveChildDown(a_object);
                    }
                }
            }
        }
        
        ImGui::EndPopup();
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

Camera* SkeletonEditorWindow::GetCamera() const
{
    return m_camera;
}

Object* SkeletonEditorWindow::GetSelectedObject() const
{
    return m_selectedObject;
}

void SkeletonEditorWindow::Update()
{
    Object* baseObject = m_skeletonEditor->GetBaseObject();

    if (baseObject != nullptr)
    {
        if (ImGui::Begin("Skeleton Hierarchy"))
        {
            int node = 0;

            ListObjects(baseObject, node);
        }
        ImGui::End();
    }
}

void SkeletonEditorWindow::UpdatePropertiesWindow(Object* a_selectedObject)
{
    if (a_selectedObject != nullptr)
    {
        KeyValue::ResetGUI();

        const Object* baseObject = m_skeletonEditor->GetBaseObject();

        const char* name = a_selectedObject->GetTrueName();

        if (name != nullptr)
        {
            const size_t strLen = strlen(name);

            strcpy(BUFFER, name);
        }
        else
        {
            memset(BUFFER, 0, BUFFER_SIZE - 1);
        }

        if (a_selectedObject != baseObject)
        {
            ImGui::InputText("Name", BUFFER, BUFFER_SIZE);

            ImGui::Separator();
        }

        Transform* transform = a_selectedObject->GetTransform();

        transform->UpdateGUI();

        if (strcmp(BUFFER, name) != 0)
        {
            if (BUFFER[0] != 0)
            {
                a_selectedObject->SetTrueName(BUFFER);
            }
            else
            {
                a_selectedObject->SetTrueName("NULL");
            }
        }

        ImGui::Separator();

        a_selectedObject->UpdateComponentUI(m_skeletonEditor->GetWorkspace());
    }
}
void SkeletonEditorWindow::UpdateEditorWindow()
{
    const ImVec2 size = ImGui::GetWindowSize();
    const glm::vec2 useSize = { size.x - 20, size.y - 60 };

    const float scalar = m_zoom / MAX_ZOOM;

    if (ImGui::IsWindowFocused())
    {
        const glm::vec2 halfSize = useSize * 0.5f;
        const glm::vec2 trueSize = { useSize.x / IMAGE_SIZE, useSize.y / IMAGE_SIZE };
        const glm::vec2 halfTrue = trueSize * 0.5f;

        m_camera->SetProjection(glm::orthoRH(-halfTrue.x * m_zoom, halfTrue.x * m_zoom, -halfTrue.y * m_zoom, halfTrue.y * m_zoom, -1.0f, 1.0f));

        StaticTransform* transform = m_camera->GetTransform();

        glm::vec3 translation = transform->GetTranslation();

        const float mouseWheel = ImGui::GetIO().MouseWheel;
        const float wheelDelta = mouseWheel * MOUSE_WHEEL_SENSITIVITY;
        m_zoom = glm::clamp(m_zoom - wheelDelta, 0.01f, MAX_ZOOM);

        if (ImGui::IsMouseDown(2))
        {
            const ImVec2 tPos = ImGui::GetMousePos();
            const glm::vec2 mousePos = { tPos.x, tPos.y };

            if (m_lastMousePos.x >= 0 && m_lastMousePos.y >= 0)
            {
                glm::vec2 mov = m_lastMousePos - mousePos;
                translation += glm::vec3(mov.x, mov.y, 0.0f) * MOUSE_SENSITIVITY * scalar;
            }

            m_lastMousePos = mousePos;
        }
        else
        {
            m_lastMousePos = glm::vec2(-1);
        }

        transform->SetTranslation(translation);
    }

    const Texture* texture = m_skeletonEditor->DrawEditor();
    
    ImGui::Image((ImTextureID)texture->GetHandle(), { useSize.x, useSize.y });
}