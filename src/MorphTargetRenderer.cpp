#include "Components/MorphTargetRenderer.h"

#include <glm/gtc/matrix_transform.hpp>

#include "AnimControl.h"
#include "Camera.h"
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

    m_renderMode = e_MorphRenderMode::Null;
    m_selectedMode = ITEMS[0];

    m_animValuesDisplayed = false;
}
MorphTargetRenderer::~MorphTargetRenderer()
{
    delete m_morphTargetDisplay;

    delete m_lerp;
}

void MorphTargetRenderer::Init()
{
    InitValues();

    AnimControl* animControl = GetAnimControl();
    const Object* object = GetObject();

    const std::string baseName = std::string("[") + object->GetName() + "] [" + this->ComponentName() + "] ";

    m_lerp = new AnimValue<Vec2KeyValue>((baseName + "Lerp").c_str(), animControl);
}
void MorphTargetRenderer::Draw(bool a_preview, double a_delta, Camera* a_camera)
{
    const Object* object = GetObject();

    const Transform* transform = object->GetTransform();

    glm::mat4 transformMat;
    glm::vec3 anchor;
    const char* modelName;

    glm::vec2 lerp = glm::vec2(0);

    if (a_preview)
    {
        transformMat = transform->GetBaseWorldMatrix();

        anchor = -GetBaseAnchor();
        modelName = GetBaseModelName();

        Vec2KeyValue* lerpValue = m_lerp->GetValue();

        if (lerpValue != nullptr)
        {
            lerp = lerpValue->GetBaseValue();
        }
    }
    else
    {
        transformMat = transform->GetWorldMatrix();  
        
        anchor = -GetAnchor();  
        modelName = GetModelName();

        Vec2KeyValue* lerpValue = m_lerp->GetAnimValue();

        if (lerpValue != nullptr)
        {
            lerp = lerpValue->GetValue();
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

    switch (m_renderMode)
    {
    case e_MorphRenderMode::Point5:
    {
        m_morphTargetDisplay->Draw(finalTransform, lerp);

        break;
    }
    case e_MorphRenderMode::Point3:
    {
        m_morphTargetDisplay->Draw(finalTransform, glm::vec2(lerp.x, 0));

        break;
    }
    default:
    {
        m_morphTargetDisplay->Draw(finalTransform, glm::vec2(0));

        break;
    }
    }
}

void MorphTargetRenderer::Update(double a_delta, Camera* a_camera)
{
    Draw(false, a_delta, a_camera);
}
void MorphTargetRenderer::UpdatePreview(double a_delta, Camera* a_camera)
{
    Draw(true, a_delta, a_camera);
}
void MorphTargetRenderer::UpdateGUI()
{
    UpdateRendererGUI();

    ImGui::Spacing();

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
        m_renderMode = e_MorphRenderMode::Point3;
    }
    else if (strcmp(m_selectedMode, "5 Point") == 0)
    {
        m_renderMode = e_MorphRenderMode::Point5;
    }
    else if (strcmp(m_selectedMode, "9 Point") == 0)
    {
        m_renderMode = e_MorphRenderMode::Point9;
    }
    else
    {
        m_renderMode = e_MorphRenderMode::Null;
    }

    DisplayValues(m_animValuesDisplayed);

    switch (m_renderMode)
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
}

void MorphTargetRenderer::DisplayValues(bool a_value)
{
    m_animValuesDisplayed = a_value;

    DisplayRendererValues(a_value);

    switch (m_renderMode)
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

void MorphTargetRenderer::Load(PropertyFileProperty* a_property, AnimControl* a_animControl)
{
    const std::list<PropertyFileValue> values = a_property->Values();

    int renderMode = -1;

    for (auto iter = values.begin(); iter != values.end(); ++iter)
    {
        IFSETTOATTVALI(iter->Name, "renderMode", renderMode, iter->Value)
    }

    if (renderMode != -1)
    {
        m_renderMode = (e_MorphRenderMode)renderMode;

        m_selectedMode = ITEMS[renderMode];
    }
}
void MorphTargetRenderer::Save(PropertyFile* a_propertyFile, PropertyFileProperty* a_parent) const
{
    PropertyFileProperty* property = SaveValues(a_propertyFile, a_parent);

    property->EmplaceValue("renderMode", std::to_string((int)m_renderMode).c_str());
}