#include "Components/MorphPlaneRenderer.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string.h>

#include "Camera.h"
#include "DataStore.h"
#include "FileLoaders/PropertyFile.h"
#include "FileUtils.h"
#include "imgui.h"
#include "ModelEditor.h"
#include "Models/Model.h"
#include "MorphPlane.h"
#include "Object.h"
#include "Renderers/MorphPlaneDisplay.h"
#include "StaticTransform.h"
#include "Texture.h"
#include "Transform.h"
#include "Workspace.h"

#ifdef WIN32
#undef GetObject
#endif

const char* MorphPlaneRenderer::COMPONENT_NAME = "MorphPlaneRenderer";

const char* MorphPlaneRenderer::ITEMS[] = { "Null", "Lerp", "5 Point", "9 Point" };

MorphPlaneRenderer::MorphPlaneRenderer(Object* a_object, AnimControl* a_animControl) : 
    Renderer(a_object, a_animControl)
{
    m_morphPlaneDisplay = new MorphPlaneDisplay();

    m_selectedMode = ITEMS[(int)e_MorphRenderMode::Null];

    m_animValuesDisplayed = false;

    m_editingMorphPlane = false;
}
MorphPlaneRenderer::~MorphPlaneRenderer()
{
    delete m_morphPlaneDisplay;

    delete m_morphPlaneName;

    delete m_lerp;

    delete m_renderMode;

    delete m_northPlaneName;
    delete m_southPlaneName;
    delete m_eastPlaneName;
    delete m_westPlaneName;

    delete m_northEastPlaneName;
    delete m_southEastPlaneName;
    delete m_southWestPlaneName;
    delete m_northWestPlaneName;
}

void MorphPlaneRenderer::MorphPlaneInit()
{
    InitValues();

    AnimControl* animControl = GetAnimControl();
    const Object* object = GetObject();

    const std::string baseName = std::string("[") + object->GetName() + "] [" + this->ComponentName() + "] ";

    m_morphPlaneName = new AnimValue<StringKeyValue>((baseName + "Morph Plane Name").c_str(), animControl);

    m_renderMode = new AnimValue<IntKeyValue>((baseName + "Morph Mode").c_str(), animControl);

    m_lerp = new AnimValue<Vec2KeyValue>((baseName + "Lerp").c_str(), animControl);

    m_northPlaneName = new AnimValue<StringKeyValue>((baseName + "North Plane Name").c_str(), animControl);
    m_southPlaneName = new AnimValue<StringKeyValue>((baseName + "South Plane Name").c_str(), animControl);
    m_eastPlaneName = new AnimValue<StringKeyValue>((baseName + "East Plane Name").c_str(), animControl);
    m_westPlaneName = new AnimValue<StringKeyValue>((baseName + "West Plane Name").c_str(), animControl);

    m_northEastPlaneName = new AnimValue<StringKeyValue>((baseName + "North East Plane Name").c_str(), animControl);
    m_southEastPlaneName = new AnimValue<StringKeyValue>((baseName + "South East Plane Name").c_str(), animControl);
    m_southWestPlaneName = new AnimValue<StringKeyValue>((baseName + "South West Plane Name").c_str(), animControl);
    m_northWestPlaneName = new AnimValue<StringKeyValue>((baseName + "North West Plane Name").c_str(), animControl);
}
void MorphPlaneRenderer::MorphPlaneObjectRenamed()
{
    RenameValues();

    const Object* object = GetObject();

    const std::string baseName = std::string("[") + object->GetName() + "] [" + this->ComponentName() + "] ";

    m_morphPlaneName->Rename((baseName + "Morph Plane Name").c_str());

    m_lerp->Rename((baseName + "Lerp").c_str());

    m_renderMode->Rename((baseName + "Morph Mode").c_str());

    m_northPlaneName->Rename((baseName + "North Plane Name").c_str());
    m_southPlaneName->Rename((baseName + "South Plane Name").c_str());
    m_eastPlaneName->Rename((baseName + "East Plane Name").c_str());
    m_westPlaneName->Rename((baseName + "West Plane Name").c_str());

    m_northEastPlaneName->Rename((baseName + "North East Plane Name").c_str());
    m_southEastPlaneName->Rename((baseName + "South East Plane Name").c_str());
    m_southWestPlaneName->Rename((baseName + "South West Plane Name").c_str());
    m_northWestPlaneName->Rename((baseName + "North West Plane Name").c_str());
}

void MorphPlaneRenderer::MorphPlaneDisplayValues(bool a_value)
{
    m_animValuesDisplayed = a_value;

    DisplayRendererValues(a_value);

    m_morphPlaneName->SetDisplayState(a_value);
    m_renderMode->SetDisplayState(a_value);

    e_MorphRenderMode renderMode = e_MorphRenderMode::Null;

    IntKeyValue* renderModeKeyValue = m_renderMode->GetValue();
    if (renderModeKeyValue != nullptr)
    {
        renderMode = (e_MorphRenderMode)renderModeKeyValue->GetBaseValue();
    }

    switch (renderMode)
    {
    case e_MorphRenderMode::Point3:
    {
        m_lerp->SetDisplayState(a_value);
        
        m_northPlaneName->SetDisplayState(false);
        m_southPlaneName->SetDisplayState(false);
        m_eastPlaneName->SetDisplayState(a_value);
        m_westPlaneName->SetDisplayState(a_value);

        m_northEastPlaneName->SetDisplayState(false);
        m_southEastPlaneName->SetDisplayState(false);
        m_southWestPlaneName->SetDisplayState(false);
        m_northWestPlaneName->SetDisplayState(false);

        break;
    }
    case e_MorphRenderMode::Point5:
    {  
        m_lerp->SetDisplayState(a_value);

        m_northPlaneName->SetDisplayState(a_value);
        m_southPlaneName->SetDisplayState(a_value);
        m_eastPlaneName->SetDisplayState(a_value);
        m_westPlaneName->SetDisplayState(a_value);

        m_northEastPlaneName->SetDisplayState(false);
        m_southEastPlaneName->SetDisplayState(false);
        m_southWestPlaneName->SetDisplayState(false);
        m_northWestPlaneName->SetDisplayState(false);

        break;
    }
    case e_MorphRenderMode::Point9:
    {
        m_lerp->SetDisplayState(a_value);

        m_northPlaneName->SetDisplayState(a_value);
        m_southPlaneName->SetDisplayState(a_value);
        m_eastPlaneName->SetDisplayState(a_value);
        m_westPlaneName->SetDisplayState(a_value);

        m_northEastPlaneName->SetDisplayState(a_value);
        m_southEastPlaneName->SetDisplayState(a_value);
        m_southWestPlaneName->SetDisplayState(a_value);
        m_northWestPlaneName->SetDisplayState(a_value);

        break;
    }
    default:
    {
        m_lerp->SetDisplayState(false);
        
        m_northPlaneName->SetDisplayState(false);
        m_southPlaneName->SetDisplayState(false);
        m_eastPlaneName->SetDisplayState(false);
        m_westPlaneName->SetDisplayState(false);

        m_northEastPlaneName->SetDisplayState(false);
        m_southEastPlaneName->SetDisplayState(false);
        m_southWestPlaneName->SetDisplayState(false);
        m_northWestPlaneName->SetDisplayState(false);

        break;
    }
    }
}

void MorphPlaneRenderer::UpdateMorphPlaneGUI(const char* a_label, AnimValue<StringKeyValue>* a_morphPlane)
{
    StringKeyValue* planeValue = a_morphPlane->GetValue();
    if (planeValue != nullptr)
    {
        planeValue->UpdateGUI(a_label);
    }
}

void MorphPlaneRenderer::MorphPlaneUpdateGUI(Workspace* a_workspace)
{
    DataStore* store = DataStore::GetInstance();

    UpdateRendererGUI();

    StringKeyValue* value = m_morphPlaneName->GetValue();

    if (value != nullptr)
    {
        value->UpdateGUI("Morph Plane Name");

        if (value != nullptr)
        {
            const char* modelName = GetBaseModelName();

            if (modelName != nullptr)
            {
                Model* model = store->GetModel(modelName, e_ModelType::MorphPlane);
                MorphPlane* morphPlane = store->GetMorphPlane(value->GetBaseString());

                if (model != nullptr && morphPlane != nullptr)
                {
                    if (m_editingMorphPlane)
                    {
                        if (ImGui::Button("Cancel"))
                        {
                            m_editingMorphPlane = false;

                            a_workspace->GetModelEditor()->ToolsDisplayOverride(false);
                        }
                    }
                    else
                    {
                        if (ImGui::Button("Edit Morph Plane"))
                        {
                            m_editingMorphPlane = true;

                            m_workSpace = a_workspace;

                            a_workspace->GetModelEditor()->ToolsDisplayOverride(true);
                        }
                    }
                }
            }
        }

        ImGui::Spacing();

        IntKeyValue* renderModeValue = m_renderMode->GetValue();

        if (renderModeValue != nullptr)
        {
            renderModeValue->SetLerpMode(e_LerpMode::Set);

            m_selectedMode = ITEMS[renderModeValue->GetBaseValue()];
            if (ImGui::BeginCombo("Morph Mode", m_selectedMode))
            {
                for (int i = 0; i < IM_ARRAYSIZE(ITEMS); ++i)
                {
                    bool is_selected = (m_selectedMode == ITEMS[i]); 
                    if (ImGui::Selectable(ITEMS[i], is_selected))
                    {
                        m_selectedMode = ITEMS[i];
                    }
                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }

                ImGui::EndCombo();
            }
        
            if (strcmp(m_selectedMode, "Lerp") == 0)
            {
                renderModeValue->SetBaseValue((int)e_MorphRenderMode::Point3);
            }
            else if (strcmp(m_selectedMode, "5 Point") == 0)
            {
                renderModeValue->SetBaseValue((int)e_MorphRenderMode::Point5);
            }
            else if (strcmp(m_selectedMode, "9 Point") == 0)
            {
                renderModeValue->SetBaseValue((int)e_MorphRenderMode::Point9);
            }
            else
            {
                renderModeValue->SetBaseValue((int)e_MorphRenderMode::Null);
            }

            ImGui::Spacing();

            DisplayValues(m_animValuesDisplayed);

            switch ((e_MorphRenderMode)renderModeValue->GetBaseValue())
            {
            case e_MorphRenderMode::Point9:
            {
                Vec2KeyValue* vecValue = m_lerp->GetValue();
                if (vecValue != nullptr)
                {
                    vecValue->SetMinValue(glm::vec2(-1.0f));
                    vecValue->SetMaxValue(glm::vec2(1.0f));

                    vecValue->UpdateGUI("Lerp");
                }

                UpdateMorphPlaneGUI("North Morph Plane", m_northPlaneName);
                UpdateMorphPlaneGUI("South Morph Plane", m_southPlaneName);
                UpdateMorphPlaneGUI("East Morph Plane", m_eastPlaneName);
                UpdateMorphPlaneGUI("West Morph Plane", m_westPlaneName);

                UpdateMorphPlaneGUI("North East Plane", m_northEastPlaneName);
                UpdateMorphPlaneGUI("South East Plane", m_southEastPlaneName);
                UpdateMorphPlaneGUI("South West Plane", m_southWestPlaneName);
                UpdateMorphPlaneGUI("North West Plane", m_northWestPlaneName);

                break;
            }
            case e_MorphRenderMode::Point5:
            {
                Vec2KeyValue* vecValue = m_lerp->GetValue();
                if (vecValue != nullptr)
                {
                    vecValue->SetMinValue(glm::vec2(-1.0f));
                    vecValue->SetMaxValue(glm::vec2(1.0f));

                    vecValue->UpdateGUI("Lerp");
                }

                UpdateMorphPlaneGUI("North Morph Plane", m_northPlaneName);
                UpdateMorphPlaneGUI("South Morph Plane", m_southPlaneName);
                UpdateMorphPlaneGUI("East Morph Plane", m_eastPlaneName);
                UpdateMorphPlaneGUI("West Morph Plane", m_westPlaneName);

                break;
            }
            case e_MorphRenderMode::Point3:
            {
                Vec2KeyValue* vecValue = m_lerp->GetValue();
                if (vecValue != nullptr)
                {
                    glm::vec2 val = vecValue->GetBaseValue();

                    ImGui::DragFloat("Lerp", &val.x, 0.01f);
                    ImGui::SameLine();
                    vecValue->UpdateLerpGUI();

                    val.x = glm::clamp(val.x, -1.0f, 1.0f);

                    vecValue->SetBaseValue(val);
                }

                UpdateMorphPlaneGUI("East Morph Plane", m_eastPlaneName);
                UpdateMorphPlaneGUI("West Morph Plane", m_westPlaneName);

                break;
            }
            }
        }
    }  
}

void MorphPlaneRenderer::MorphPlaneDraw(bool a_preview, double a_delta, Camera* a_camera)
{
    const DataStore* store = DataStore::GetInstance();

    const Object* object = GetObject();

    Transform* transform = object->GetTransform();

    e_MorphRenderMode renderMode = e_MorphRenderMode::Null;

    glm::mat4 transformMat;
    glm::vec3 anchor;
    bool depthTest;
    const char* modelName = nullptr;

    glm::vec2 lerp;

    const char* morphPlaneName = nullptr;

    const char* eastPlaneName = nullptr;
    const char* westPlaneName = nullptr;
    const char* northPlaneName = nullptr;
    const char* southPlaneName = nullptr;

    const char* northEastPlaneName = nullptr;
    const char* southEastPlaneName = nullptr;
    const char* southWestPlaneName = nullptr;
    const char* northWestPlaneName = nullptr;

    const char* useMask = nullptr;

    const DepthRenderTexture* mask = nullptr;

    if (a_preview)
    {
        transformMat = transform->GetBaseWorldMatrix();

        anchor = -GetBaseAnchor();
        modelName = GetBaseModelName();
        depthTest = GetBaseDepthTest();
        useMask = GetBaseMaskName();

        if (useMask != nullptr)
        {
            mask = store->GetPreviewMask(useMask);
        }

        const Vec2KeyValue* lerpValue = m_lerp->GetValue();
        if (lerpValue != nullptr)
        {
            lerp = lerpValue->GetBaseValue();
        }

        const StringKeyValue* morphValue = m_morphPlaneName->GetValue();
        if (morphValue != nullptr)
        {
            morphPlaneName = morphValue->GetBaseString();
        }

        const StringKeyValue* eastMorphValue = m_eastPlaneName->GetValue();
        if (eastMorphValue != nullptr)
        {
            eastPlaneName = eastMorphValue->GetBaseString();
        }
        const StringKeyValue* westMorphValue = m_westPlaneName->GetValue();
        if (westMorphValue != nullptr)
        {
            westPlaneName = westMorphValue->GetBaseString();
        }
        const StringKeyValue* northMorphValue = m_northPlaneName->GetValue();
        if (northMorphValue != nullptr)
        {
            northPlaneName = northMorphValue->GetBaseString();
        }
        const StringKeyValue* southMorphValue = m_southPlaneName->GetValue();
        if (southMorphValue != nullptr)
        {
            southPlaneName = southMorphValue->GetBaseString();
        }

        const StringKeyValue* northEastValue = m_northEastPlaneName->GetValue();
        if (northEastValue != nullptr)
        {
            northEastPlaneName = northEastValue->GetBaseString();
        }
        const StringKeyValue* southEastValue = m_southEastPlaneName->GetValue();
        if (southEastValue != nullptr)
        {
            southEastPlaneName = southEastValue->GetBaseString();
        }
        const StringKeyValue* southWestValue = m_southWestPlaneName->GetValue();
        if (southWestValue != nullptr)
        {
            southWestPlaneName = southWestValue->GetBaseString();
        }
        const StringKeyValue* northWestValue = m_northWestPlaneName->GetValue();
        if (northWestValue != nullptr)
        {
            northWestPlaneName = northWestValue->GetBaseString();
        }

        const IntKeyValue* renderModeValue = m_renderMode->GetValue();
        if (renderModeValue != nullptr)
        {
            renderMode = (e_MorphRenderMode)renderModeValue->GetBaseValue(); 
        }
    }
    else
    {
        transformMat = transform->GetWorldMatrix();  
        
        anchor = -GetAnchor();  
        modelName = GetModelName();
        depthTest = GetDepthTest();
        useMask = GetMaskName();

        if (useMask != nullptr)
        {
            mask = store->GetMask(useMask);
        }

        const Vec2KeyValue* lerpValue = m_lerp->GetAnimValue();
        if (lerpValue != nullptr)
        {
            lerp = lerpValue->GetValue();
        }

        const StringKeyValue* morphValue = m_morphPlaneName->GetAnimValue();
        if (morphValue != nullptr)
        {
            morphPlaneName = morphValue->GetString();
        }
        
        const StringKeyValue* eastMorphValue = m_eastPlaneName->GetAnimValue();
        if (eastMorphValue != nullptr)
        {
            eastPlaneName = eastMorphValue->GetString();
        }
        const StringKeyValue* westMorphValue = m_westPlaneName->GetAnimValue();
        if (westMorphValue != nullptr)
        {
            westPlaneName = westMorphValue->GetString();
        }
        const StringKeyValue* northMorphValue = m_northPlaneName->GetAnimValue();
        if (northMorphValue != nullptr)
        {
            northPlaneName = northMorphValue->GetString();
        }
        const StringKeyValue* southMorphValue = m_southPlaneName->GetAnimValue();
        if (southMorphValue != nullptr)
        {
            southPlaneName = southMorphValue->GetString();
        }

        const StringKeyValue* northEastValue = m_northEastPlaneName->GetAnimValue();
        if (northEastValue != nullptr)
        {
            northEastPlaneName = northEastValue->GetString();
        }
        const StringKeyValue* southEastValue = m_southEastPlaneName->GetAnimValue();
        if (southEastValue != nullptr)
        {
            southEastPlaneName = southEastValue->GetString();
        }
        const StringKeyValue* southWestValue = m_southWestPlaneName->GetAnimValue();
        if (southWestValue != nullptr)
        {
            southWestPlaneName = southWestValue->GetString();
        }
        const StringKeyValue* northWestValue = m_northWestPlaneName->GetAnimValue();
        if (northWestValue != nullptr)
        {
            northWestPlaneName = northWestValue->GetString();
        }

        const IntKeyValue* renderModeValue = m_renderMode->GetAnimValue();
        if (renderModeValue != nullptr)
        {
            renderMode = (e_MorphRenderMode)renderModeValue->GetValue(); 
        }
    }
    
    const glm::mat4 shift = transformMat * glm::translate(glm::mat4(1), anchor);

    glm::mat4 view = glm::mat4(1);
    glm::mat4 proj = glm::orthoRH(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);

    if (a_camera != nullptr)
    {
        view = glm::inverse(a_camera->GetTransform()->ToMatrix());
        proj = a_camera->GetProjection();
    }

    const glm::mat4 finalTransform = view * proj * shift;

    m_morphPlaneDisplay->SetModelName(modelName);
    m_morphPlaneDisplay->SetMorphPlaneName(morphPlaneName);

    if (depthTest)
    {
        glDepthFunc(GL_LESS);
    }
    else
    {
        glDepthFunc(GL_ALWAYS);
    }

    switch (renderMode)
    {
    case e_MorphRenderMode::Point3:
    {
        if (morphPlaneName != nullptr && eastPlaneName != nullptr && westPlaneName != nullptr)
        {
            const MorphPlane* centreMorphPlane = store->GetMorphPlane(morphPlaneName);

            const MorphPlane* eastMorphPlane = store->GetMorphPlane(eastPlaneName);
            const MorphPlane* westMorphPlane = store->GetMorphPlane(westPlaneName);

            if (centreMorphPlane != nullptr && eastMorphPlane != nullptr && westMorphPlane != nullptr)
            {
                MorphPlane* morphPlane = centreMorphPlane->Lerp(lerp.x, *eastMorphPlane, *westMorphPlane);

                if (useMask == nullptr || useMask[0] == 0)
                {
                    m_morphPlaneDisplay->Draw(morphPlane, finalTransform);
                }
                else
                {
                    m_morphPlaneDisplay->DrawMasked(morphPlane, finalTransform, mask);
                }

                delete morphPlane;
            }
        }

        break;
    }
    case e_MorphRenderMode::Point5:
    {
        if (morphPlaneName != nullptr && eastPlaneName != nullptr && westPlaneName != nullptr && northPlaneName != nullptr && southPlaneName != nullptr)
        {
            const MorphPlane* centreMorphPlane = store->GetMorphPlane(morphPlaneName);

            const MorphPlane* eastMorphPlane = store->GetMorphPlane(eastPlaneName);
            const MorphPlane* westMorphPlane = store->GetMorphPlane(westPlaneName);
            const MorphPlane* northMorphPlane = store->GetMorphPlane(northPlaneName);
            const MorphPlane* southMorphPlane = store->GetMorphPlane(southPlaneName);

            if (centreMorphPlane != nullptr && eastMorphPlane != nullptr && westMorphPlane != nullptr && northMorphPlane != nullptr && southMorphPlane != nullptr)
            {
                MorphPlane* morphPlane = centreMorphPlane->Lerp5(lerp, *westMorphPlane, *eastMorphPlane, *northMorphPlane, *southMorphPlane);

                if (useMask == nullptr || useMask[0] == 0)
                {
                    m_morphPlaneDisplay->Draw(morphPlane, finalTransform);
                }
                else
                {
                    m_morphPlaneDisplay->DrawMasked(morphPlane, finalTransform, mask);
                }

                delete morphPlane;
            }
        }

        break;
    }
    case e_MorphRenderMode::Point9:
    {
        if (morphPlaneName != nullptr && eastPlaneName != nullptr && westPlaneName != nullptr && northPlaneName != nullptr && southPlaneName != nullptr
            && northEastPlaneName != nullptr && southEastPlaneName != nullptr && southWestPlaneName != nullptr && northWestPlaneName != nullptr)
        {
            const MorphPlane* centreMorphPlane = store->GetMorphPlane(morphPlaneName);

            const MorphPlane* eastMorphPlane = store->GetMorphPlane(eastPlaneName);
            const MorphPlane* westMorphPlane = store->GetMorphPlane(westPlaneName);
            const MorphPlane* northMorphPlane = store->GetMorphPlane(northPlaneName);
            const MorphPlane* southMorphPlane = store->GetMorphPlane(southPlaneName);

            const MorphPlane* northEastMorphPlane = store->GetMorphPlane(northEastPlaneName);
            const MorphPlane* southEastMorphPlane = store->GetMorphPlane(southEastPlaneName);
            const MorphPlane* southWestMorphPlane = store->GetMorphPlane(southWestPlaneName);
            const MorphPlane* northWestMorphPlane = store->GetMorphPlane(northWestPlaneName);

            if (centreMorphPlane != nullptr && eastMorphPlane != nullptr && westMorphPlane != nullptr && northMorphPlane != nullptr && southMorphPlane != nullptr &&
                northEastMorphPlane != nullptr && southEastMorphPlane != nullptr && southWestMorphPlane != nullptr && northWestMorphPlane != nullptr)
            {
                MorphPlane* morphPlane = centreMorphPlane->Lerp9(lerp, *westMorphPlane, *eastMorphPlane, *northMorphPlane, *southMorphPlane, 
                *northWestMorphPlane, *southWestMorphPlane, *southEastMorphPlane, *northEastMorphPlane);

                if (useMask == nullptr || useMask[0] == 0)
                {
                    m_morphPlaneDisplay->Draw(morphPlane, finalTransform);
                }
                else
                {
                    m_morphPlaneDisplay->DrawMasked(morphPlane, finalTransform, mask);
                }
                
                delete morphPlane;
            }
        }

        break;
    }
    default:
    {
        if (useMask == nullptr || useMask[0] == 0)
        {
            m_morphPlaneDisplay->Draw(finalTransform);
        }
        else
        {
            m_morphPlaneDisplay->DrawMasked(finalTransform, mask);
        }

        break;
    }
    }

    if (a_preview && m_editingMorphPlane)
    {
        ModelEditor* modelEditor = m_workSpace->GetModelEditor();

        modelEditor->ResetIMRenderer();

        const ImVec2 winSize = ImGui::GetWindowSize();
        const glm::vec2 scaledWinSize = { winSize.x - 20, winSize.y - 60 };

        glm::vec2 scalar = glm::vec2(1);

        if (scaledWinSize.x > scaledWinSize.y)
        {
            scalar.x = scaledWinSize.y / scaledWinSize.x;
        }
        else
        {
            scalar.y = scaledWinSize.x / scaledWinSize.y;
        }

        modelEditor->DrawMorphPlaneEditor(finalTransform, modelName, morphPlaneName, finalTransform[3].z, scalar);

        modelEditor->DrawIMRenderer();
    }
}

void MorphPlaneRenderer::Init()
{
    MorphPlaneInit();
}

void MorphPlaneRenderer::Update(double a_delta, Camera* a_camera)
{
    MorphPlaneDraw(false, a_delta, a_camera);
}
void MorphPlaneRenderer::UpdatePreview(double a_delta, Camera* a_camera)
{
    MorphPlaneDraw(true, a_delta, a_camera);
}
void MorphPlaneRenderer::UpdateGUI(Workspace* a_workspace)
{
    MorphPlaneUpdateGUI(a_workspace);
}

void MorphPlaneRenderer::ObjectRenamed()
{
    MorphPlaneObjectRenamed();
}

const char* MorphPlaneRenderer::ComponentName() const
{
    return COMPONENT_NAME;
}

void MorphPlaneRenderer::DisplayValues(bool a_value)
{
    MorphPlaneDisplayValues(a_value);
}
