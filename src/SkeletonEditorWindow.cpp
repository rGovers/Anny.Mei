#include "WindowControls/SkeletonEditorWindow.h"

#include "Camera.h"
#include "imgui.h"
#include "Object.h"
#include "SkeletonEditor.h"
#include "StaticTransform.h"
#include "Texture.h"
#include "Transform.h"

static const int BUFFER_SIZE = 1024;

const static int IMAGE_SIZE = 4096;

const static float MAX_ZOOM = 2.5f;
const static float MOUSE_SENSITIVITY = 0.001f;
const static float MOUSE_WHEEL_SENSITIVITY = 0.1f;

SkeletonEditorWindow::SkeletonEditorWindow(SkeletonEditor* a_skeletonEditor)
{
    m_skeletonEditor = a_skeletonEditor;

    m_camera = new Camera();

    m_zoom = 1;
}
SkeletonEditorWindow::~SkeletonEditorWindow()
{
    delete m_camera;
}

Camera* SkeletonEditorWindow::GetCamera() const
{
    return m_camera;
}

void SkeletonEditorWindow::Update()
{
    Object* baseObject = m_skeletonEditor->GetBaseObject();

    if (baseObject != nullptr)
    {
        if (ImGui::Begin("Skeleton Hierarchy"))
        {
            int node = 0;

            m_skeletonEditor->ListObjects(baseObject, node);
        }
        ImGui::End();
    }
}

void SkeletonEditorWindow::UpdatePropertiesWindow(Object* a_selectedObject)
{
    if (a_selectedObject != nullptr)
    {
        const Object* baseObject = m_skeletonEditor->GetBaseObject();

        const char* name = a_selectedObject->GetTrueName();

        char* buffer;
        if (name != nullptr)
        {
            const size_t strLen = strlen(name);

            buffer = new char[strLen + 2];

            strcpy(buffer, name);
        }
        else
        {
            buffer = new char[3] { 0 };
        }

        if (a_selectedObject != baseObject)
        {
            ImGui::InputText("Name", buffer, BUFFER_SIZE);

            ImGui::Separator();
        }

        Transform* transform = a_selectedObject->GetTransform();

        glm::vec3 translation = transform->GetBaseTranslation();
        ImGui::InputFloat3("Translation", (float*)&translation, 4);
        transform->SetTranslation(translation);

        // Need to a some point implement euler angles for easy user control
        glm::fquat quat = transform->GetBaseRotation();
        ImGui::InputFloat4("Rotation", (float*)&quat, 4);
        transform->SetRotation(glm::normalize(quat));

        glm::vec3 scale = transform->GetBaseScale();
        ImGui::InputFloat3("Scale", (float*)&scale, 4);
        transform->SetScale(scale);

        if (strcmp(buffer, name) != 0)
        {
            if (buffer[0] != 0)
            {
                a_selectedObject->SetTrueName(buffer);
            }
            else
            {
                a_selectedObject->SetTrueName("NULL");
            }
        }

        delete[] buffer;

        ImGui::Separator();

        a_selectedObject->UpdateComponentUI();
    }
}
void SkeletonEditorWindow::UpdateEditorWindow()
{
    const ImVec2 size = ImGui::GetWindowSize();
    const glm::vec2 useSize = { size.x - 20, size.y - 60 };
    if (ImGui::IsWindowFocused())
    {
        const glm::vec2 halfSize = useSize * 0.5f;
        const glm::vec2 trueSize = { useSize.x / IMAGE_SIZE, useSize.y / IMAGE_SIZE };

        m_camera->SetProjection(glm::orthoRH(0.0f, trueSize.x * m_zoom * 5, 0.0f, trueSize.y * m_zoom * 5, -1.0f, 1.0f));

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
                translation += glm::vec3(mov.x, mov.y, 0.0f) * MOUSE_SENSITIVITY;
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