#pragma once

class Texture;

class RenderTexture
{
private:
    const Texture* m_texture;

    unsigned int   m_depthTexture;
  
    unsigned int   m_bufferHandle;

    float*         m_viewCache;
protected:

public:
    RenderTexture() = delete;
    RenderTexture(int a_width, int a_height, int a_pixelFormat);
    RenderTexture(Texture* a_texture);
    ~RenderTexture();

    const Texture* GetTexture() const;

    unsigned int GetBufferHandle() const;
    unsigned int GetDepthHandle() const;

    void Bind() const;
    void Unbind() const;
};