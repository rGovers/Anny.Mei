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

MorphPlaneRenderer::MorphPlaneRenderer(Object* a_object, AnimControl* a_animControl) : 
    Renderer(a_object, a_animControl)
{
    m_morphPlaneDisplay = new MorphPlaneDisplay();
}
MorphPlaneRenderer::~MorphPlaneRenderer()
{
    delete m_morphPlaneDisplay;
}

void MorphPlaneRenderer::Init()
{
    InitValues();

    AnimControl* animControl = GetAnimControl();
    const Object* object = GetObject();

    const std::string baseName = std::string("[") + object->GetName() + "] [" + this->ComponentName() + "] ";

    m_morphPlaneName = new AnimValue<StringKeyValue>((baseName + "Morph Plane Name").c_str(), animControl);
}

void MorphPlaneRenderer::Draw(bool a_preview, double a_delta, Camera* a_camera)
{
    const Object* object = GetObject();

    Transform* transform = object->GetTransform();

    glm::mat4 transformMat;
    glm::vec3 anchor;
    const char* modelName;
    const char* morphPlaneName = nullptr;

    if (a_preview)
    {
        transformMat = transform->GetBaseWorldMatrix();

        anchor = -GetBaseAnchor();
        modelName = GetBaseModelName();

        const StringKeyValue* value = m_morphPlaneName->GetValue();

        if (value != nullptr)
        {
            morphPlaneName = value->GetBaseString();
        }
    }
    else
    {
        transformMat = transform->GetWorldMatrix();  
        
        anchor = -GetAnchor();  
        modelName = GetModelName();

        const StringKeyValue* value = m_morphPlaneName->GetAnimValue();

        if (value != nullptr)
        {
            morphPlaneName = value->GetString();
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

    m_morphPlaneDisplay->Draw(finalTransform);
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
        const char* nameVal = value->GetBaseString();

        const size_t len = strlen(nameVal);

        char* buff = new char[len + 1];
        strcpy(buff, nameVal);

        ImGui::InputText("Morph Plane Name", buff, BUFFER_SIZE);

        if (strcmp(buff, nameVal) != 0)
        {
            value->SetString(buff);
        }

        delete[] buff;
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

void MorphPlaneRenderer::Load(PropertyFileProperty* a_property, AnimControl* a_animControl)
{
    LoadValues(a_property, a_animControl);

    const std::list<PropertyFileValue> values = a_property->Values();

    char* buff = nullptr;

    for (auto iter = values.begin(); iter != values.end(); ++iter)
    {
        IFSETTOATTVALCPY(iter->Name, "morphPlaneName", buff, iter->Value)
    }

    m_morphPlaneName->SelectKeyFrame(0);
    if (buff != nullptr)
    {
        m_morphPlaneName->GetValue()->SetString(buff);
    }
}
void MorphPlaneRenderer::Save(PropertyFile* a_propertyFile, PropertyFileProperty* a_parent) const
{
    PropertyFileProperty* property = SaveValues(a_propertyFile, a_parent);
    
    m_morphPlaneName->SelectKeyFrame(0);
    const char* buff = m_morphPlaneName->GetValue()->GetString();

    property->EmplaceValue("morphPlaneName", buff);
}