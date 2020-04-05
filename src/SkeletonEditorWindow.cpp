#include "WindowControls/SkeletonEditorWindow.h"

#include "Camera.h"
#include "imgui.h"
#include "Object.h"
#include "SkeletonEditor.h"
#include "StaticTransform.h"
#include "Texture.h"
#include "Transform.h"

static const int BUFFER_SIZE = 1024;
static char* BUFFER = new char[BUFFER_SIZE];

const static int IMAGE_SIZE = 4096;

const static float MAX_ZOOM = 2.5f;
const static float MOUSE_SENSITIVITY = 0.005f;
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

        a_selectedObject->UpdateComponentUI();
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