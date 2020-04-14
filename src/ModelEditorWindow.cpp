#include "WindowControls/ModelEditorWindow.h"

#include <limits>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include "DataStore.h"
#include "imgui.h"
#include "InputControl.h"
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

    m_inputControl = new InputControl();

    ResetTools();
}
ModelEditorWindow::~ModelEditorWindow()
{
    delete m_camera;

    delete m_inputControl;
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
    m_axis = e_Axis::Null;
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

glm::vec4 ModelEditorWindow::ScreenToWorld(const glm::vec2& a_pos, const glm::vec2& a_halfSize, const glm::mat4& a_invProj, const glm::mat4& a_invView) const
{
    const glm::vec4 scaledPos = glm::vec4((a_pos.x / a_halfSize.x) - 1.0f, (a_pos.y / a_halfSize.y) - 1.0f, 0.0f, 1.0f);
    glm::vec4 worldPos = a_invView * a_invProj * scaledPos;
                
    const float delta = 1 / worldPos.w;

    worldPos.x *= delta;
    worldPos.y *= delta;
    worldPos.z *= delta;
    worldPos.w = 1;

    return worldPos;
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

        m_inputControl->Update();

        glm::vec2 startPos;
        glm::vec2 endPos;
        if (m_inputControl->Dragging(&startPos, &endPos))
        {
            const glm::vec4 startWorldPos = ScreenToWorld(startPos, halfSize, projectionInverse, transformMatrix);
            const glm::vec4 endWorldPos = ScreenToWorld(endPos, halfSize, projectionInverse, transformMatrix);

            switch (m_toolMode)
            {
            case e_ToolMode::Select:
                {
                    m_modelEditor->DrawSelectionBox(startWorldPos, endWorldPos);

                    break;
                }
            case e_ToolMode::Move:
                {
                    const glm::vec2 world = { startWorldPos.x, startWorldPos.y };

                    if (m_inputControl->Clicked())
                    {
                        const glm::vec2 diff = m_selectionPoint - world;
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
                    }
                    else
                    {
                        const glm::vec2 mov = m_inputControl->DragMove() * scalar;

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
                    

                    break;
                }
            }
        }
        else if (m_inputControl->DragEnd(&startPos, &endPos))
        {
            const glm::vec4 startWorldPos = ScreenToWorld(startPos, halfSize, projectionInverse, transformMatrix);
            const glm::vec4 endWorldPos = ScreenToWorld(endPos, halfSize, projectionInverse, transformMatrix);

            m_modelEditor->SelectMouseUp(startWorldPos, endWorldPos);
        }
    }
    else
    {
        ResetTools();
    }

    const Texture* texture = m_modelEditor->DrawEditor();

    ImGui::Image((ImTextureID)texture->GetHandle(), { useSize.x, useSize.y });
}