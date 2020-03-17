#include "Components/ImageRenderer.h"

#include <glm/glm.hpp>

#include "Camera.h"
#include "DataStore.h"
#include "FileUtils.h"
#include "imgui.h"
#include "Models/Model.h"
#include "Object.h"
#include "Renderers/ImageDisplay.h"
#include "StaticTransform.h"
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

void ImageRenderer::ImageDraw(bool a_preview, Camera* a_camera)
{  
    DataStore* store = DataStore::GetInstance();

    const Object* object = GetObject();

    Transform* transform = object->GetTransform();

    glm::mat4 transformMat;
    glm::vec3 anchor;
    const char* modelName = nullptr;
    const char* useMask = nullptr;

    DepthRenderTexture* mask = nullptr;

    if (a_preview)
    {
        transformMat = transform->GetBaseWorldMatrix();

        anchor = -GetBaseAnchor();
        modelName = GetBaseModelName();
        useMask = GetBaseMaskName();

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
        useMask = GetMaskName();

        if (useMask != nullptr)
        {
            mask = store->GetMask(useMask);
        }
    }


    m_imageDisplay->SetModelName(modelName);

    const glm::mat4 shift = transformMat * glm::translate(glm::mat4(1), anchor);

    glm::mat4 view = glm::mat4(1);
    glm::mat4 proj = glm::orthoRH(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);

    if (a_camera != nullptr)
    {
        view = glm::inverse(a_camera->GetTransform()->ToMatrix());
        proj = a_camera->GetProjection();
    }

    const glm::mat4 finalTransform = view * proj * shift;

    if (useMask == nullptr || useMask[0] == 0)
    {
        m_imageDisplay->Draw(finalTransform);
    }
    else
    {
        m_imageDisplay->DrawMasked(finalTransform, mask);
    }
}

void ImageRenderer::Update(double a_delta, Camera* a_camera)
{
    ImageDraw(false, a_camera);
}
void ImageRenderer::UpdatePreview(double a_delta, Camera* a_camera)
{
    ImageDraw(true, a_camera);
}
void ImageRenderer::UpdateGUI()
{
    UpdateRendererGUI();
}

const char* ImageRenderer::ComponentName() const
{
    return COMPONENT_NAME;
}
