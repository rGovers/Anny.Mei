#include "Components/ImageRenderer.h"

#include <glm/glm.hpp>

#include "Camera.h"
#include "DataStore.h"
#include "FileUtils.h"
#include "imgui.h"
#include "Material.h"
#include "Models/Model.h"
#include "Object.h"
#include "PropertyFile.h"
#include "Renderers/ImageDisplay.h"
#include "Texture.h"
#include "Transform.h"

const char* ImageRenderer::COMPONENT_NAME = "ImageRenderer";

ImageRenderer::ImageRenderer(Object* a_object, AnimControl* a_animControl) :
    Renderer(a_object, a_animControl)
{
    m_imageDisplay = new ImageDisplay();
}
ImageRenderer::~ImageRenderer()
{
    delete m_imageDisplay;
}

void ImageRenderer::Draw(bool a_preview, Camera* a_camera)
{  
    m_imageDisplay->SetModelName(GetModelName());

    const Object* object = GetObject();

    Transform* transform = object->GetTransform();

    const glm::mat4 transformMat = transform->GetWorldMatrix();
    const glm::mat4 shift = transformMat * glm::translate(glm::mat4(1), -GetAnchor());

    glm::mat4 view = glm::mat4(1);
    glm::mat4 proj = glm::orthoRH(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);

    if (a_camera != nullptr)
    {
        view = glm::inverse(a_camera->GetTransform()->ToMatrix());
        proj = a_camera->GetProjection();
    }

    const glm::mat4 finalTransform = view * proj * shift;

    m_imageDisplay->Draw(finalTransform);
}

void ImageRenderer::Update(double a_delta, Camera* a_camera)
{
    Draw(false, a_camera);
}
void ImageRenderer::UpdatePreview(double a_delta, Camera* a_camera)
{
    Draw(true, a_camera);
}
void ImageRenderer::UpdateGUI()
{
    UpdateRendererGUI();
}

const char* ImageRenderer::ComponentName() const
{
    return COMPONENT_NAME;
}

void ImageRenderer::Load(PropertyFileProperty* a_property, AnimControl* a_animControl)
{
    LoadValues(a_property, a_animControl);
}
void ImageRenderer::Save(PropertyFile* a_propertyFile, PropertyFileProperty* a_parent) const
{
    SaveValues(a_propertyFile, a_parent);
}