#include "RenderTexture.h"

#include <glad/glad.h>

#include "Texture.h"

RenderTexture::RenderTexture(int a_width, int a_height, int a_pixelFormat) : 
    RenderTexture(new Texture(a_width, a_height, a_pixelFormat)) { }
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