#include "DepthRenderTexture.h"

#include <glad/glad.h>

#include "Texture.h"

DepthRenderTexture::DepthRenderTexture(int a_width, int a_height)
{
    m_depthTexture = new Texture(a_width, a_height, GL_DEPTH_COMPONENT);

    m_viewCache = new float[4]; 

    glGenFramebuffers(1, &m_handle);
    glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture->GetHandle(), 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
DepthRenderTexture::~DepthRenderTexture()
{
    delete m_depthTexture;

    glDeleteFramebuffers(1, &m_handle);
}

unsigned int DepthRenderTexture::GetBufferHandle() const
{
    return m_handle;
}

const Texture* DepthRenderTexture::GetDepthTexture() const
{
    return m_depthTexture;
}

void DepthRenderTexture::Bind() const
{
    glGetFloatv(GL_VIEWPORT, m_viewCache);
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&m_fbCache);

    glViewport(0, 0, m_depthTexture->GetWidth(), m_depthTexture->GetHeight());

    glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
}
void DepthRenderTexture::Unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbCache);

    glViewport(m_viewCache[0], m_viewCache[1], m_viewCache[2], m_viewCache[3]);
}