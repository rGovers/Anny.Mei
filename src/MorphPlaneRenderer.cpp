#include "Components/MorphPlaneRenderer.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string.h>

#include "Camera.h"
#include "DataStore.h"
#include "FileUtils.h"
#include "imgui.h"
#include "Models/Model.h"
#include "MorphPlane.h"
#include "Object.h"
#include "PropertyFile.h"
#include "Renderers/MorphPlaneDisplay.h"
#include "StaticTransform.h"
#include "Texture.h"
#include "Transform.h"

const static int BUFFER_SIZE = 1024;
const char* MorphPlaneRenderer::COMPONENT_NAME = "MorphPlaneRenderer";

const char* MorphPlaneRenderer::ITEMS[] = { "Null", "Lerp", "5 Point", "9 Point" };

void DisplayStringValue(StringKeyValue* a_value, const char* a_name)
{
    const char* nameVal = a_value->GetBaseString();
    char* buff;

    if (nameVal != nullptr)
    {
        const size_t len = strlen(nameVal);

        buff = new char[len + 1];
        strcpy(buff, nameVal);
    }
    else
    {
        buff = new char[2] { 0 };
    }

    ImGui::InputText(a_name, buff, BUFFER_SIZE);

    if (nameVal != nullptr)
    {
        if (strcmp(buff, nameVal) != 0)
        {
            a_value->SetString(buff);
        }
    }
    else
    {
        a_value->SetString(buff);
    }

    delete[] buff;
}

MorphPlaneRenderer::MorphPlaneRenderer(Object* a_object, AnimControl* a_animControl) : 
    Renderer(a_object, a_animControl)
{
    m_morphPlaneDisplay = new MorphPlaneDisplay();

    m_renderMode = e_MorphRenderMode::Null;
    m_selectedMode = ITEMS[0];

    m_animValuesDisplayed = false;
}
MorphPlaneRenderer::~MorphPlaneRenderer()
{
    delete m_morphPlaneDisplay;

    delete m_morphPlaneName;

    delete m_lerp;

    delete m_northPlaneName;
    delete m_southPlaneName;
    delete m_eastPlaneName;
    delete m_westPlaneName;
}

void MorphPlaneRenderer::Init()
{
    InitValues();

    AnimControl* animControl = GetAnimControl();
    const Object* object = GetObject();

    const std::string baseName = std::string("[") + object->GetName() + "] [" + this->ComponentName() + "] ";

    m_morphPlaneName = new AnimValue<StringKeyValue>((baseName + "Morph Plane Name").c_str(), animControl);

    m_lerp = new AnimValue<Vec2KeyValue>((baseName + "Lerp").c_str(), animControl);

    m_northPlaneName = new AnimValue<StringKeyValue>((baseName + "North Plane Name").c_str(), animControl);
    m_southPlaneName = new AnimValue<StringKeyValue>((baseName + "South Plane Name").c_str(), animControl);
    m_eastPlaneName = new AnimValue<StringKeyValue>((baseName + "East Plane Name").c_str(), animControl);
    m_westPlaneName = new AnimValue<StringKeyValue>((baseName + "West Plane Name").c_str(), animControl);
}

void MorphPlaneRenderer::Draw(bool a_preview, double a_delta, Camera* a_camera)
{
    const DataStore* store = DataStore::GetInstance();

    const Object* object = GetObject();

    Transform* transform = object->GetTransform();

    glm::mat4 transformMat;
    glm::vec3 anchor;
    const char* modelName = nullptr;

    glm::vec2 lerp;

    const char* morphPlaneName = nullptr;

    const char* eastPlaneName = nullptr;
    const char* westPlaneName = nullptr;
    const char* northPlaneName = nullptr;
    const char* southPlaneName = nullptr;

    if (a_preview)
    {
        transformMat = transform->GetBaseWorldMatrix();

        anchor = -GetBaseAnchor();
        modelName = GetBaseModelName();

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
    }
    else
    {
        transformMat = transform->GetWorldMatrix();  
        
        anchor = -GetAnchor();  
        modelName = GetModelName();

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

    switch (m_renderMode)
    {
    case e_MorphRenderMode::Point3:
    {
        if (morphPlaneName != nullptr && eastPlaneName != nullptr && westPlaneName != nullptr)
        {
            MorphPlane* centreMorphPlane = store->GetMorphPlane(morphPlaneName);

            MorphPlane* eastMorphPlane = store->GetMorphPlane(eastPlaneName);
            MorphPlane* westMorphPlane = store->GetMorphPlane(westPlaneName);

            if (centreMorphPlane != nullptr && eastMorphPlane != nullptr && westMorphPlane != nullptr)
            {
                MorphPlane* morphPlane = centreMorphPlane->Lerp(lerp.x, *eastMorphPlane, *westMorphPlane);

                m_morphPlaneDisplay->Draw(morphPlane, finalTransform);

                delete morphPlane;
            }
        }

        break;
    }
    case e_MorphRenderMode::Point5:
    {
        if (morphPlaneName != nullptr && eastPlaneName != nullptr && westPlaneName != nullptr && northPlaneName != nullptr && southPlaneName != nullptr)
        {
            MorphPlane* centreMorphPlane = store->GetMorphPlane(morphPlaneName);

            MorphPlane* eastMorphPlane = store->GetMorphPlane(eastPlaneName);
            MorphPlane* westMorphPlane = store->GetMorphPlane(westPlaneName);
            MorphPlane* northMorphPlane = store->GetMorphPlane(northPlaneName);
            MorphPlane* southMorphPlane = store->GetMorphPlane(southPlaneName);

            if (centreMorphPlane != nullptr && eastMorphPlane != nullptr && westMorphPlane != nullptr && northMorphPlane != nullptr && southMorphPlane != nullptr)
            {
                MorphPlane* morphPlane = centreMorphPlane->Lerp5(lerp, *eastMorphPlane, *westMorphPlane, *northMorphPlane, *southMorphPlane);

                m_morphPlaneDisplay->Draw(morphPlane, finalTransform);

                delete morphPlane;
            }
        }

        break;
    }
    default:
    {
        m_morphPlaneDisplay->Draw(finalTransform);

        break;
    }
    }
}

void MorphPlaneRenderer::Update(double a_delta, Camera* a_camera)
{
    Draw(false, a_delta, a_camera);
}
void MorphPlaneRenderer::UpdatePreview(double a_delta, Camera* a_camera)
{
    Draw(true, a_delta, a_camera);
}
void MorphPlaneRenderer::UpdateGUI()
{
    UpdateRendererGUI();

    StringKeyValue* value = m_morphPlaneName->GetValue();

    if (value != nullptr)
    {
        DisplayStringValue(value, "Morph Plane Name");

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

        ImGui::Spacing();

        DisplayValues(m_animValuesDisplayed);

        switch (m_renderMode)
        {
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

            StringKeyValue* northPlaneValue = m_northPlaneName->GetValue();
            if (northPlaneValue != nullptr)
            {
                DisplayStringValue(northPlaneValue, "North Morph Plane");
            }

            StringKeyValue* southPlaneValue = m_southPlaneName->GetValue();
            if (southPlaneValue != nullptr)
            {
                DisplayStringValue(southPlaneValue, "South Morph Plane");
            }

            StringKeyValue* eastPlaneValue = m_eastPlaneName->GetValue();
            if (eastPlaneValue != nullptr)
            {
                DisplayStringValue(eastPlaneValue, "East Morph Plane");
            }

            StringKeyValue* westPlaneValue = m_westPlaneName->GetValue();
            if (westPlaneValue != nullptr)
            {
                DisplayStringValue(westPlaneValue, "West Morph Plane");
            }

            break;
        }
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

            StringKeyValue* eastPlaneValue = m_eastPlaneName->GetValue();
            if (eastPlaneValue != nullptr)
            {
                DisplayStringValue(eastPlaneValue, "East Morph Plane");
            }

            StringKeyValue* westPlaneValue = m_westPlaneName->GetValue();
            if (westPlaneValue != nullptr)
            {
                DisplayStringValue(westPlaneValue, "West Morph Plane");
            }

            break;
        }
        }
    }
}

void MorphPlaneRenderer::ObjectRenamed()
{
    RenameValues();

    const Object* object = GetObject();

    const std::string baseName = std::string("[") + object->GetName() + "] [" + this->ComponentName() + "] ";

    m_morphPlaneName->Rename((baseName + "Morph Plane Name").c_str());
}

const char* MorphPlaneRenderer::ComponentName() const
{
    return COMPONENT_NAME;
}

void MorphPlaneRenderer::DisplayValues(bool a_value)
{
    m_animValuesDisplayed = a_value;

    m_morphPlaneName->SetDisplayState(a_value);

    switch (m_renderMode)
    {
    case e_MorphRenderMode::Point3:
    {
        m_lerp->SetDisplayState(a_value);
        
        m_northPlaneName->SetDisplayState(false);
        m_southPlaneName->SetDisplayState(false);
        m_eastPlaneName->SetDisplayState(a_value);
        m_westPlaneName->SetDisplayState(a_value);

        break;
    }

    case e_MorphRenderMode::Point5:
    case e_MorphRenderMode::Point9:
    {  
        m_lerp->SetDisplayState(a_value);

        m_northPlaneName->SetDisplayState(a_value);
        m_southPlaneName->SetDisplayState(a_value);
        m_eastPlaneName->SetDisplayState(a_value);
        m_westPlaneName->SetDisplayState(a_value);

        break;
    }
    default:
    {
        m_lerp->SetDisplayState(false);
        
        m_northPlaneName->SetDisplayState(false);
        m_southPlaneName->SetDisplayState(false);
        m_eastPlaneName->SetDisplayState(false);
        m_westPlaneName->SetDisplayState(false);

        break;
    }
    }
}

void MorphPlaneRenderer::Load(PropertyFileProperty* a_property, AnimControl* a_animControl)
{
    LoadValues(a_property, a_animControl);

    const std::list<PropertyFileValue> values = a_property->Values();

    char* morphPlaneName = nullptr;
    char* northPlaneName = nullptr;
    char* southPlaneName = nullptr;
    char* eastPlaneName = nullptr;
    char* westPlaneName = nullptr;
    int renderMode = -1;

    for (auto iter = values.begin(); iter != values.end(); ++iter)
    {
        IFSETTOATTVALCPY(iter->Name, "morphPlaneName", morphPlaneName, iter->Value)
        else IFSETTOATTVALCPY(iter->Name, "northPlaneName", northPlaneName, iter->Value)
        else IFSETTOATTVALCPY(iter->Name, "southPlaneName", southPlaneName, iter->Value)
        else IFSETTOATTVALCPY(iter->Name, "eastPlaneName", eastPlaneName, iter->Value)
        else IFSETTOATTVALCPY(iter->Name, "westPlaneName", westPlaneName, iter->Value)
        else IFSETTOATTVALI(iter->Name, "renderMode", renderMode, iter->Value)
    }

    if (renderMode != -1)
    {
        m_renderMode = (e_MorphRenderMode)renderMode;

        m_selectedMode = ITEMS[renderMode];
    }

    m_morphPlaneName->SelectKeyFrame(0);
    if (morphPlaneName != nullptr)
    {
        m_morphPlaneName->GetValue()->SetString(morphPlaneName);
        delete[] morphPlaneName;
    }
    
    m_northPlaneName->SelectKeyFrame(0);
    if (northPlaneName != nullptr)
    {
        m_northPlaneName->GetValue()->SetString(northPlaneName);
        delete[] northPlaneName;
    }

    m_southPlaneName->SelectKeyFrame(0);
    if (southPlaneName != nullptr)
    {
        m_southPlaneName->GetValue()->SetString(southPlaneName);
        delete[] southPlaneName;
    }

    m_eastPlaneName->SelectKeyFrame(0);
    if (eastPlaneName != nullptr)
    {
        m_eastPlaneName->GetValue()->SetString(eastPlaneName);
        delete[] eastPlaneName;
    }

    m_westPlaneName->SelectKeyFrame(0);
    if (westPlaneName != nullptr)
    {
        m_westPlaneName->GetValue()->SetString(westPlaneName);
        delete[] westPlaneName;
    }
}
void MorphPlaneRenderer::Save(PropertyFile* a_propertyFile, PropertyFileProperty* a_parent) const
{
    PropertyFileProperty* property = SaveValues(a_propertyFile, a_parent);
    
    const char* buff;

    property->EmplaceValue("renderMode", std::to_string((int)m_renderMode).c_str());

    m_morphPlaneName->SelectKeyFrame(0);
    buff = m_morphPlaneName->GetValue()->GetBaseString();
    if (buff != nullptr)
    {
        property->EmplaceValue("morphPlaneName", buff);
    }

    m_northPlaneName->SelectKeyFrame(0);
    buff = m_northPlaneName->GetValue()->GetBaseString();
    if (buff != nullptr)
    {
        property->EmplaceValue("northPlaneName", buff);
    }

    m_southPlaneName->SelectKeyFrame(0);
    buff = m_southPlaneName->GetValue()->GetBaseString();
    if (buff != nullptr)
    {
        property->EmplaceValue("southPlaneName", buff);
    }

    m_eastPlaneName->SelectKeyFrame(0);
    buff = m_eastPlaneName->GetValue()->GetBaseString();
    if (buff != nullptr)
    {
        property->EmplaceValue("eastPlaneName", buff);
    }

    m_westPlaneName->SelectKeyFrame(0);
    buff = m_westPlaneName->GetValue()->GetBaseString();
    if (buff != nullptr)
    {
        property->EmplaceValue("westPlaneName", buff);
    }
}