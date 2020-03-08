#include "WindowControls/ModelEditorWindow.h"

#include <limits>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include "DataStore.h"
#include "imgui.h"
#include "ModelEditor.h"
#include "Name.h"
#include "StaticTransform.h"
#include "Texture.h"

const static int BUFFER_SIZE = 1024;

const static int IMAGE_SIZE = 2048;

const static float MOUSE_SENSITIVITY = 0.001f;
const static float MOUSE_WHEEL_SENSITIVITY = 0.1f;
const static float MAX_ZOOM = 2.5f;

ModelEditorWindow::ModelEditorWindow(ModelEditor* a_modelEditor)
{
    m_modelEditor = a_modelEditor;

    m_toolMode = e_ToolMode::Select;
    m_axis = e_Axis::Null;

    m_camera = new Camera();

    m_zoom = 1;

    m_solid = true;
    m_wireframe = false;
    m_alpha = false;

    ResetTools();
}
ModelEditorWindow::~ModelEditorWindow()
{
    delete m_camera;
}

void ModelEditorWindow::SetSelectTool()
{
    m_toolMode = e_ToolMode::Select;

    m_startDragPos = glm::vec2(-std::numeric_limits<float>::infinity());
}
void ModelEditorWindow::SetMoveTool()
{
    m_toolMode = e_ToolMode::Move;

    m_dragging = false;
    m_axis = e_Axis::Null;
}

void ModelEditorWindow::ResetTools()
{
    m_startDragPos = glm::vec2(-std::numeric_limits<float>::infinity());

    m_axis = e_Axis::Null;

    m_dragging = false;
}

void ModelEditorWindow::SetSelectionPoint(const glm::vec2& a_point)
{
    m_selectionPoint = a_point;
}

Camera* ModelEditorWindow::GetCamera() const
{
    return m_camera;
}

e_ToolMode ModelEditorWindow::GetToolMode() const
{
    return m_toolMode;
}
e_Axis ModelEditorWindow::GetAxis() const
{
    return m_axis;
}

bool ModelEditorWindow::GetAlphaMode() const
{
    return m_alpha;
}
bool ModelEditorWindow::GetSolidMode() const
{
    return m_solid;
}
bool ModelEditorWindow::GetWireFrameMode() const
{
    return m_wireframe;
}

void ModelEditorWindow::Update()
{
    if (ImGui::Begin("Model List"))
    {
        m_modelEditor->DrawModelList();
    }
    ImGui::End();

    if (m_modelEditor->IsModelSelected())
    {
        ImGui::SetNextWindowSize({ 200, 100 }, ImGuiCond_Appearing);
        if (ImGui::Begin("Model Tools"))
        {
            if (ImGui::Button("Select"))
            {
                SetSelectTool();
            }
            if (ImGui::Button("Move"))
            {
                SetMoveTool();
            }
        }
        ImGui::End();
    }
}
void ModelEditorWindow::UpdatePropertiesWindow(const ModelData* a_modelData)
{
    if (a_modelData != nullptr)
    {
        const char* name = a_modelData->ModelName->GetTrueName();

        size_t len = strlen(name);

        char* buff = new char[len + 1];
        strcpy(buff, name);

        ImGui::InputText("Model Name", buff, BUFFER_SIZE);

        if (strcmp(name, buff) != 0)
        {
            m_modelEditor->RenameModel(buff);
        }

        delete[] buff;

        name = a_modelData->TextureName;

        len = strlen(name);

        buff = new char[len + 1];
        strcpy(buff, name);

        ImGui::InputText("Texture Name", buff, BUFFER_SIZE);

        if (strcmp(name, buff) != 0)
        {
            m_modelEditor->SetTextureName(buff);
        }
        delete[] buff;

        if (ImGui::TreeNode("Morph Planes"))
        {
            const std::list<MorphPlaneData*> morphPlanes = a_modelData->MorphPlanes;

            int newSize = a_modelData->MorphPlaneSize;

            ImGui::InputInt("Morph Plane Size", &newSize);

            newSize = glm::clamp(newSize, 1, 100);
            if (a_modelData->MorphPlaneSize != newSize && newSize >= 0)
            {
                m_modelEditor->ResizeMorphPlane(newSize);
            }

            if (ImGui::Button("Add Morph Plane"))
            {
                m_modelEditor->AddMorphPlaneClicked();
            }

            if (a_modelData->MorphPlanes.size() > 0)
            {
                ImGui::Separator();

                for (auto iter = morphPlanes.begin(); iter != morphPlanes.end(); ++iter)
                {
                    bool is_selected = m_modelEditor->IsMorphPlaneSelected(*iter);

                    if (ImGui::Selectable((*iter)->MorphPlaneName->GetName(), &is_selected))
                    {
                        m_modelEditor->MorphPlaneSelected(*iter);
                    }

                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
            }

            ImGui::TreePop();
        }
    }
}
void ModelEditorWindow::UpdateEditorWindow()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("View"))
        {
            ImGui::MenuItem("Solid", nullptr, &m_solid);
            ImGui::MenuItem("Wireframe", nullptr, &m_wireframe);
            ImGui::MenuItem("Alpha", nullptr, &m_alpha);

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

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
        
        const glm::mat4 transformMatrix = transform->ToMatrix();
        const glm::mat4 viewMatirx = glm::inverse(transformMatrix);
        const glm::mat4 projectionMatrix = m_camera->GetProjection();
        const glm::mat4 projectionInverse = glm::inverse(projectionMatrix);

        const glm::mat4 viewProj = projectionMatrix * viewMatirx;

        const bool mouseDown = ImGui::IsMouseDown(0);

        if (mouseDown)
        {
            const ImVec2 tPos = ImGui::GetMousePos();
            const ImVec2 tWPos = ImGui::GetWindowPos();

            const glm::vec2 mousePos = { tPos.x, tPos.y };
            const glm::vec2 winPos = { tWPos.x + 10, tWPos.y + 40 };
            const glm::vec2 relPos = mousePos - winPos;

            if (relPos.x > 0 && relPos.y > 0 && relPos.x <= useSize.x && relPos.y <= useSize.y)
            {
                const glm::vec4 scaledMousePos = glm::vec4((relPos.x / halfSize.x) - 1.0f, (relPos.y / halfSize.y) - 1.0f, 0.0f, 1.0f);
                glm::vec4 worldPos = transformMatrix * projectionInverse * scaledMousePos;
                
                const float delta = 1 / worldPos.w;

                worldPos.x *= delta;
                worldPos.y *= delta;
                worldPos.z *= delta;
                worldPos.w = 1;

                switch (m_toolMode)
                {
                case e_ToolMode::Select:
                {
                    if (m_startDragPos.x == -std::numeric_limits<float>::infinity() || m_startDragPos.y == -std::numeric_limits<float>::infinity())
                    {
                        m_startDragPos = { worldPos.x, worldPos.y };
                    }
                    m_endDragPos = { worldPos.x, worldPos.y };

                    m_modelEditor->DrawSelectionBox(m_startDragPos, m_endDragPos);

                    break;
                }
                case e_ToolMode::Move:
                {
                    const glm::vec2 world2 = { worldPos.x, worldPos.y };

                    if (!m_dragging)
                    {
                        const glm::vec2 diff = m_selectionPoint - world2;
                        const glm::vec2 aDiff = { glm::abs(diff.x), glm::abs(diff.y) };

                        if (aDiff.x <= 0.01f && aDiff.y <= 0.5f)
                        {
                            m_axis = e_Axis::Y;
                        }
                        else if (aDiff.x <= 0.5f && aDiff.y <= 0.01f)
                        {
                            m_axis = e_Axis::X;
                        }
                        else
                        {
                            m_axis = e_Axis::Null;
                        }

                        m_dragging = true;
                    }
                    else
                    {
                        const glm::vec2 mov = world2 - m_lastPos;

                        switch (m_axis)
                        {
                        case e_Axis::X:
                        {
                            m_modelEditor->DragValue({ mov.x, 0.0f });

                            break;
                        }
                        case e_Axis::Y:
                        {
                            m_modelEditor->DragValue({ 0.0f, mov.y });

                            break;
                        }
                        }

                    }

                    m_lastPos = world2;
                }
                }

            }   
        }
        else if (m_mouseLastDown)
        {
            switch (m_toolMode)
            {
            case e_ToolMode::Select:
            {
                m_modelEditor->SelectMouseUp(m_startDragPos, m_endDragPos);

                break;
            }
            }

            ResetTools();
        }

        m_mouseLastDown = mouseDown;
    }
    else
    {
        ResetTools();
    }

    const Texture* texture = m_modelEditor->DrawEditor();

    ImGui::Image((ImTextureID)texture->GetHandle(), { useSize.x, useSize.y });
}