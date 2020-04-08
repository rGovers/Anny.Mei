#pragma once

#include "DepthRenderTexture.h"

class RenderTexture : public DepthRenderTexture
{
private:
    Texture* m_texture;

protected:

public:
    RenderTexture() = delete;
    RenderTexture(int a_width, int a_height, int a_pixelFormat, int a_samples = 1);
    RenderTexture(Texture* a_texture);
    virtual ~RenderTexture();

    Texture* GetTexture() const;

    void Bind() const;
};