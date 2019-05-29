#include "RenderTexture.h"

#include <glad/glad.h>

#include "Texture.h"

RenderTexture::RenderTexture(int a_width, int a_height, int a_pixelFormat) 
    : RenderTexture(new Texture(a_width, a_height, a_pixelFormat)) { }
RenderTexture::RenderTexture(Texture* a_texture) 
    : m_texture(a_texture)
{
    glGenTextures(1, &m_depthTexture);
    glBindTexture(GL_TEXTURE_2D, m_depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_texture->GetWidth(), m_texture->GetHeight(), 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glGenFramebuffers(1, &m_bufferHandle);
    glBindFramebuffer(GL_FRAMEBUFFER, m_bufferHandle);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture->GetHandle(), 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
RenderTexture::~RenderTexture()
{
    delete m_texture;

    glDeleteFramebuffers(1, &m_bufferHandle);
    glDeleteTextures(1, &m_depthTexture);
}

const Texture* RenderTexture::GetTexture() const
{
    return m_texture;
}

unsigned int RenderTexture::GetBufferHandle() const
{
    return m_bufferHandle;
}
unsigned int RenderTexture::GetDepthHandle() const
{
    return m_depthTexture;
}

void RenderTexture::Bind() const
{
    glViewport(0, 0, m_texture->GetWidth(), m_texture->GetHeight());
    
    glBindFramebuffer(GL_FRAMEBUFFER, m_bufferHandle);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void RenderTexture::Unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}