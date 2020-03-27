#include "Components/MorphTargetRenderer.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include "AnimControl.h"
#include "Camera.h"
#include "DataStore.h"
#include "imgui.h"
#include "Object.h"
#include "Renderers/MorphTargetDisplay.h"
#include "StaticTransform.h"
#include "Transform.h"

const static int BUFFER_SIZE = 1024;
const char* MorphTargetRenderer::COMPONENT_NAME = "MorphTargetRenderer";

const char* MorphTargetRenderer::ITEMS[] = { "Null", "Lerp", "5 Point", "9 Point" };

MorphTargetRenderer::MorphTargetRenderer(Object* a_object, AnimControl* a_animControl) : 
    Renderer(a_object, a_animControl)
{
    m_morphTargetDisplay = new MorphTargetDisplay();

    m_selectedMode = ITEMS[(int)e_MorphRenderMode::Null];

    m_animValuesDisplayed = false;
}
MorphTargetRenderer::~MorphTargetRenderer()
{
    delete m_morphTargetDisplay;

    delete m_lerp;
    delete m_renderMode;
}

void MorphTargetRenderer::MorphTargetDraw(bool a_preview, double a_delta, Camera* a_camera)
{
    DataStore* store = DataStore::GetInstance();

    const Object* object = GetObject();

    const Transform* transform = object->GetTransform();

    glm::mat4 transformMat;
    glm::vec3 anchor;
    const char* modelName = nullptr;
    bool depthTest;
    const char* useMask = nullptr;

    glm::vec2 lerp = glm::vec2(0);

    e_MorphRenderMode renderMode = e_MorphRenderMode::Null;

    DepthRenderTexture* mask = nullptr;

    if (a_preview)
    {
        transformMat = transform->GetBaseWorldMatrix();

        anchor = -GetBaseAnchor();
        modelName = GetBaseModelName();
        depthTest = GetBaseDepthTest();
        useMask = GetBaseMaskName();

        Vec2KeyValue* lerpValue = m_lerp->GetValue();
        if (lerpValue != nullptr)
        {
            lerp = lerpValue->GetBaseValue();
        }

        SetIntKeyValue* renderValue = m_renderMode->GetValue();
        if (renderValue != nullptr)
        {
            renderMode = (e_MorphRenderMode)renderValue->GetBaseInt();
        }

        if (useMask != nullptr)
        {
            mask = store->GetPreviewMask(useMask);
        }
    }
    else
    {
        transformMat = transform->GetWorldMatrix();  
        
        anchor = -GetAnchor();  
        modelName = GetModelName();
        depthTest = GetDepthTest();
        useMask = GetMaskName();

        Vec2KeyValue* lerpValue = m_lerp->GetAnimValue();
        if (lerpValue != nullptr)
        {
            lerp = lerpValue->GetValue();
        }

        SetIntKeyValue* renderValue = m_renderMode->GetAnimValue();
        if (renderValue != nullptr)
        {
            renderMode = (e_MorphRenderMode)renderValue->GetInt();
        }

        if (useMask != nullptr)
        {
            mask = store->GetMask(useMask);
        }
    }

    m_morphTargetDisplay->SetModelName(modelName);

    const glm::mat4 shift = transformMat * glm::translate(glm::mat4(1), anchor);

    glm::mat4 view = glm::mat4(1);
    glm::mat4 proj = glm::orthoRH(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);

    if (a_camera != nullptr)
    {
        view = glm::inverse(a_camera->GetTransform()->ToMatrix());
        proj = a_camera->GetProjection();
    }

    const glm::mat4 finalTransform = view * proj * shift;

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
    case e_MorphRenderMode::Point9:
    {
        if (useMask == nullptr || useMask[0] == 0)
        {
            m_morphTargetDisplay->Draw9Point(finalTransform, lerp);
        }
        else
        {
            m_morphTargetDisplay->Draw9PointMasked(finalTransform, mask, lerp);
        }
        
        break;
    }
    case e_MorphRenderMode::Point5:
    {
        if (useMask == nullptr || useMask[0] == 0)
        {
            m_morphTargetDisplay->Draw(finalTransform, lerp);
        }
        else
        {
            m_morphTargetDisplay->DrawMasked(finalTransform, mask, lerp);
        }

        break;
    }
    case e_MorphRenderMode::Point3:
    {
        if (useMask == nullptr || useMask[0] == 0)
        {
            m_morphTargetDisplay->Draw(finalTransform, glm::vec2(lerp.x, 0));
        }
        else
        {
            m_morphTargetDisplay->DrawMasked(finalTransform, mask, glm::vec2(lerp.x, 0));
        }

        break;
    }
    default:
    {
        if (useMask == nullptr || useMask[0] == 0)
        {
            m_morphTargetDisplay->Draw(finalTransform);
        }
        else
        {
            m_morphTargetDisplay->DrawMasked(finalTransform, mask);
        }

        break;
    }
    }
}
void MorphTargetRenderer::MorphTargetUpdateGUI()
{
    UpdateRendererGUI();

    ImGui::Spacing();

    SetIntKeyValue* renderModeValue = m_renderMode->GetValue();
    
    if (renderModeValue != nullptr)
    {
        m_selectedMode = ITEMS[renderModeValue->GetBaseInt()];
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
            renderModeValue->SetInt((int)e_MorphRenderMode::Point3);
        }
        else if (strcmp(m_selectedMode, "5 Point") == 0)
        {
            renderModeValue->SetInt((int)e_MorphRenderMode::Point5);
        }
        else if (strcmp(m_selectedMode, "9 Point") == 0)
        {
            renderModeValue->SetInt((int)e_MorphRenderMode::Point9);
        }
        else
        {
            renderModeValue->SetInt((int)e_MorphRenderMode::Null);
        }

        DisplayValues(m_animValuesDisplayed);

        switch ((e_MorphRenderMode)renderModeValue->GetBaseInt())
        {
        case e_MorphRenderMode::Point3:
        {
            Vec2KeyValue* vecValue = m_lerp->GetValue();
            if (vecValue != nullptr)
            {
                glm::vec2 val = vecValue->GetBaseValue();

                ImGui::DragFloat("Lerp", &val.x, 0.01f);

                val.x = glm::clamp(val.x, -1.0f, 1.0f);

                vecValue->SetBaseValue(val);
            }

            break;
        }
        case e_MorphRenderMode::Point5:
        case e_MorphRenderMode::Point9:
        {
            Vec2KeyValue* vecValue = m_lerp->GetValue();
            if (vecValue != nullptr)
            {
                glm::vec2 val = vecValue->GetBaseValue();

                ImGui::DragFloat2("Lerp", (float*)&val, 0.01f);

                val.x = glm::clamp(val.x, -1.0f, 1.0f);
                val.y = glm::clamp(val.y, -1.0f, 1.0f);

                vecValue->SetBaseValue(val);
            }

            break;
        }
        }
    }
}

void MorphTargetRenderer::Init()
{
    InitValues();

    AnimControl* animControl = GetAnimControl();
    const Object* object = GetObject();

    const std::string baseName = std::string("[") + object->GetName() + "] [" + this->ComponentName() + "] ";

    m_lerp = new AnimValue<Vec2KeyValue>((baseName + "Lerp").c_str(), animControl);

    m_renderMode = new AnimValue<SetIntKeyValue>((baseName + "Render Mode").c_str(), animControl);
}

void MorphTargetRenderer::Update(double a_delta, Camera* a_camera)
{
    MorphTargetDraw(false, a_delta, a_camera);
}
void MorphTargetRenderer::UpdatePreview(double a_delta, Camera* a_camera)
{
    MorphTargetDraw(true, a_delta, a_camera);
}
void MorphTargetRenderer::UpdateGUI()
{
    MorphTargetUpdateGUI();
}

const char* MorphTargetRenderer::ComponentName() const
{
    return COMPONENT_NAME;
}

void MorphTargetRenderer::ObjectRenamed()
{
    RenameValues();

    const Object* object = GetObject();

    const std::string baseName = std::string("[") + object->GetName() + "] [" + this->ComponentName() + "] ";

    m_lerp->Rename((baseName + "Lerp").c_str());

    m_renderMode->Rename((baseName + "Render Mode").c_str());
}

void MorphTargetRenderer::DisplayValues(bool a_value)
{
    m_animValuesDisplayed = a_value;

    DisplayRendererValues(a_value);

    m_renderMode->SetDisplayState(a_value);

    e_MorphRenderMode renderMode = e_MorphRenderMode::Null;

    SetIntKeyValue* renderModeKeyValue = m_renderMode->GetValue();
    if (renderModeKeyValue != nullptr)
    {
        renderMode = (e_MorphRenderMode)renderModeKeyValue->GetBaseInt();
    }

    switch (renderMode)
    {
    case e_MorphRenderMode::Point3:
    case e_MorphRenderMode::Point5:
    case e_MorphRenderMode::Point9:
    {
        m_lerp->SetDisplayState(a_value);
        
        break;
    }
    default:
    {
        m_lerp->SetDisplayState(false);

        break;
    }
    }
}
