#include "RenderTexture.h"

#include <glad/glad.h>

#include "Texture.h"

RenderTexture::RenderTexture(int a_width, int a_height, int a_pixelFormat, int a_samples) : 
    RenderTexture(new Texture(a_width, a_height, a_pixelFormat)) 
{
    if (a_samples > 1)
    {
        const unsigned int handle = m_texture->GetHandle();

        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, handle);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, a_samples, a_pixelFormat, a_width, a_height, GL_TRUE);
        
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	    glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        const unsigned int bufferHandle = GetBufferHandle();

        glBindFramebuffer(GL_FRAMEBUFFER, bufferHandle);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, handle, 0);
    }
}
RenderTexture::RenderTexture(Texture* a_texture) :
    DepthRenderTexture(a_texture->GetWidth(), a_texture->GetHeight())
{
    m_texture = a_texture;

    const unsigned int bufferHandle = GetBufferHandle();

    glBindFramebuffer(GL_FRAMEBUFFER, bufferHandle);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture->GetHandle(), 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
RenderTexture::~RenderTexture()
{
    delete m_texture;
}

Texture* RenderTexture::GetTexture() const
{
    return m_texture;
}

void RenderTexture::Bind() const
{
    DepthRenderTexture::Bind();

    glDrawBuffer(GL_COLOR_ATTACHMENT0);
}