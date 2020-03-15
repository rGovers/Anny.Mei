#pragma once

class Texture;

class DepthRenderTexture
{
private:
    unsigned int m_handle;

    Texture*     m_depthTexture;

    int          m_fbCache; 
    float*       m_viewCache;
protected:

public:
    DepthRenderTexture() = delete;
    DepthRenderTexture(int a_width, int a_height);
    virtual ~DepthRenderTexture();

    unsigned int GetBufferHandle() const;
    
    const Texture* GetDepthTexture() const;

    virtual void Bind() const;
    void Unbind() const;
};