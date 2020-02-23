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
#include "Texture.h"
#include "Transform.h"

const static int BUFFER_SIZE = 1024;
const char* MorphPlaneRenderer::COMPONENT_NAME = "MorphPlaneRenderer";

MorphPlaneRenderer::MorphPlaneRenderer(Object* a_object, AnimControl* a_animControl) : 
    Renderer(a_object, a_animControl)
{
    m_morphPlaneName = new char[1] { 0 };

    m_morphPlaneDisplay = new MorphPlaneDisplay();
}
MorphPlaneRenderer::~MorphPlaneRenderer()
{
    delete m_morphPlaneDisplay;
}

void MorphPlaneRenderer::Draw(bool a_preview, double a_delta, Camera* a_camera)
{
    const Object* object = GetObject();

    Transform* transform = object->GetTransform();

    const glm::mat4 transformMat = transform->GetWorldMatrix();
    const glm::vec3 anchor = -GetAnchor();
    const glm::mat4 shift = transformMat * glm::translate(glm::mat4(1), anchor) * glm::scale(glm::mat4(1), glm::vec3(0.5f));

    glm::mat4 view = glm::mat4(1);
    glm::mat4 proj = glm::orthoRH(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);

    if (a_camera != nullptr)
    {
        view = glm::inverse(a_camera->GetTransform()->ToMatrix());
        proj = a_camera->GetProjection();
    }

    const glm::mat4 finalTransform = view * proj * shift;

    m_morphPlaneDisplay->SetModelName(GetModelName());
    m_morphPlaneDisplay->SetMorphPlaneName(m_morphPlaneName);

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

    const size_t len = strlen(m_morphPlaneName) + 1;

    char* buff = new char[len] { 0 };
    memcpy(buff, m_morphPlaneName, len);

    ImGui::InputText("Morph Plane Name", buff, BUFFER_SIZE);

    if (strcmp(buff, m_morphPlaneName) != 0)
    {
        delete[] m_morphPlaneName;
        m_morphPlaneName = buff;
    }
    else
    {
        delete[] buff;
    }
}

const char* MorphPlaneRenderer::ComponentName() const
{
    return COMPONENT_NAME;
}

void MorphPlaneRenderer::Load(PropertyFileProperty* a_property, AnimControl* a_animControl)
{
    LoadValues(a_property, a_animControl);

    const std::list<PropertyFileValue> values = a_property->Values();

    for (auto iter = values.begin(); iter != values.end(); ++iter)
    {
        IFSETTOATTVALCPY(iter->Name, "morphPlaneName", m_morphPlaneName, iter->Value)
    }
}
void MorphPlaneRenderer::Save(PropertyFile* a_propertyFile, PropertyFileProperty* a_parent) const
{
    PropertyFileProperty* property = SaveValues(a_propertyFile, a_parent);
    
    property->EmplaceValue("morphPlaneName", m_morphPlaneName);
}