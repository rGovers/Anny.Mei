#pragma once

#include "Components/ImageRenderer.h"

class DepthRenderTexture;

class ImageMask : public ImageRenderer
{
private:
    char*               m_maskName;

    DepthRenderTexture* m_renderTexture;
    DepthRenderTexture* m_previewRenderTexture;

    void Draw(bool a_preview, double a_delta, Camera* a_camera);
protected:

public:
    const static char* COMPONENT_NAME;

    ImageMask() = delete;
    ImageMask(Object* a_object, AnimControl* a_animControl);
    virtual ~ImageMask();

    virtual void Update(double a_delta, Camera* a_camera);
    virtual void UpdatePreview(double a_delta, Camera* a_camera);
    virtual void UpdateGUI();

    virtual const char* ComponentName() const;

    virtual void Load(PropertyFileProperty* a_property, AnimControl* a_animControl);
    virtual void Save(PropertyFile* a_propertyFile, PropertyFileProperty* a_parent) const;
};
