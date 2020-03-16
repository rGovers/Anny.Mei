#pragma once

#include "Renderer.h"

class ImageDisplay;
class Model;
class Texture;

class ImageRenderer : public Renderer
{
private:
    ImageDisplay*  m_imageDisplay;

protected:
    void ImageDraw(bool a_preview, Camera* a_camera);

public:
    const static char* COMPONENT_NAME;

    ImageRenderer() = delete;
    ImageRenderer(Object* a_object, AnimControl* a_animControl);
    virtual ~ImageRenderer();

    virtual void Update(double a_delta, Camera* a_camera);
    virtual void UpdatePreview(double a_delta, Camera* a_camera);
    virtual void UpdateGUI();

    virtual const char* ComponentName() const;
};