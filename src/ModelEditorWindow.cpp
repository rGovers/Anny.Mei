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
static char* BUFFER = new char[BUFFER_SIZE];

const static int IMAGE_SIZE = 4096;

const static float MOUSE_SENSITIVITY = 0.001f;
const static float MOUSE_WHEEL_SENSITIVITY = 0.1f;
const static float MAX_ZOOM = 2.5f;

ModelEditorWindow::ModelEditorWindow(ModelEditor* a_modelEditor)
{
    m_modelEditor = a_modelEditor;

    m_toolMode = e_ToolMode::Select;
    m_axis = e_Axis::Null;

    m_camera = new Camera();

    StaticTransform* transform = m_camera->GetTransform();

    transform->SetTranslation({ 0.5f, 0.5f, 0.0f });

    m_zoom = 1;

    m_solid = true;
    m_wireframe = true;
    m_alpha = false;

    m_displayOverride = false;

    ResetTools();
}
ModelEditorWindow::~ModelEditorWindow()
{
    delete m_camera;
}

void ModelEditorWindow::ToolDisplayOverride(bool a_state)
{
    m_displayOverride = a_state;
}

void ModelEditorWindow::SetSelectTool()
{
    m_toolMode = e_ToolMode::Select;

    ResetTools();
}
void ModelEditorWindow::SetMoveTool()
{
    m_toolMode = e_ToolMode::Move;

    ResetTools();
}

void ModelEditorWindow::MorphTargetDisplay(const char* a_name, glm::vec4* a_morphTarget)
{
    bool is_selected = m_modelEditor->IsMorphTargetSelected(a_morphTarget);

    if (ImGui::Selectable(a_name, &is_selected))
    {
        m_modelEditor->MorphTargetSelected(a_morphTarget);
    }

    if (is_selected)
    {
        ImGui::SetItemDefaultFocus();
    }
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

float ModelEditorWindow::GetZoom() const
{
    return m_zoom;
}
float ModelEditorWindow::GetMaxZoom() const
{
    return MAX_ZOOM;
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

    if (m_modelEditor->IsModelSelected() || m_displayOverride)
    {
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
        strcpy(BUFFER, name);

        ImGui::InputText("Model Name", BUFFER, BUFFER_SIZE);

        if (strcmp(name, BUFFER) != 0)
        {
            m_modelEditor->RenameModel(BUFFER);
        }

        name = a_modelData->TextureName;
        strcpy(BUFFER, name);

        ImGui::InputText("Texture Name", BUFFER, BUFFER_SIZE);

        if (strcmp(name, BUFFER) != 0)
        {
            m_modelEditor->SetTextureName(BUFFER);
        }

        ImGui::Separator();

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

                    if (ImGui::BeginPopupContextItem())
                    {
                        if (ImGui::Selectable("Delete Morph Plane"))
                        {
                            const auto cur = iter;

                            --iter;

                            m_modelEditor->RemoveMorphPlane(*cur);
                        }

                        ImGui::EndPopup();
                    }
                }
            }

            ImGui::TreePop();
        }

        ImGui::Separator();

        if (a_modelData->TargetModel != nullptr)
        {
            if (ImGui::TreeNode("Morph Targets"))
            {
                bool skip = false;

                if (ImGui::BeginPopupContextItem())
                {
                    if (ImGui::Selectable("Delete Morph Targets"))
                    {
                        skip = true;

                        m_modelEditor->RemoveMorphTargets();
                    }

                    ImGui::EndPopup();
                }   

                if (!skip)
                {
                    MorphTargetDisplay("North Morph Target", a_modelData->MorphTargetData[0]);
                    MorphTargetDisplay("South Morph Target", a_modelData->MorphTargetData[1]);
                    MorphTargetDisplay("East Morph Target", a_modelData->MorphTargetData[2]);
                    MorphTargetDisplay("West Morph Target", a_modelData->MorphTargetData[3]);

                    MorphTargetDisplay("North East Morph Target", a_modelData->MorphTargetData[4]);
                    MorphTargetDisplay("South East Morph Target", a_modelData->MorphTargetData[5]);
                    MorphTargetDisplay("South West Morph Target", a_modelData->MorphTargetData[6]);
                    MorphTargetDisplay("North West Morph Target", a_modelData->MorphTargetData[7]);
                }

                ImGui::TreePop();   
            }
            else
            {
                if (ImGui::BeginPopupContextItem())
                {
                    if (ImGui::Selectable("Delete Morph Targets"))
                    {
                        m_modelEditor->RemoveMorphTargets();
                    }

                    ImGui::EndPopup();
                }   
            }
        }
        else
        {
            if (ImGui::Button("Add Morph Targets"))
            {
                m_modelEditor->AddMorphTargetsClicked();
            }
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

    const glm::vec2 halfSize = useSize * 0.5f;
    const glm::vec2 trueSize = { useSize.x / IMAGE_SIZE, useSize.y / IMAGE_SIZE };
    const glm::vec2 halfTrue = trueSize * 0.5f;

    const float scalar = m_zoom / MAX_ZOOM;

    m_camera->SetProjection(glm::orthoRH(-halfTrue.x * m_zoom, halfTrue.x * m_zoom, -halfTrue.y * m_zoom, halfTrue.y * m_zoom, -1.0f, 1.0f));

    if (ImGui::IsWindowFocused())
    {
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

                        if (aDiff.x <= scalar * 0.01f && aDiff.y <= scalar * 0.5f)
                        {
                            m_axis = e_Axis::Y;
                        }
                        else if (aDiff.x <= scalar * 0.5f && aDiff.y <= scalar * 0.01f)
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