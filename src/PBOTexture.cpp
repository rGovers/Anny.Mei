#include "PBOTexture.h"

#include <glad/glad.h>
#include <string.h>

PBOTexture::PBOTexture(int a_width, int a_height, int a_pixelFormat, int a_samples) : 
    Texture(a_width, a_height, a_pixelFormat)
{
    m_pixelFormat = a_pixelFormat;

    m_pbo = new unsigned int[2];
    m_readIndex = 0;

    if (a_samples > 1)
    {
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, a_samples, a_pixelFormat, a_width, a_height, GL_TRUE);   
    }
    else if (a_pixelFormat == GL_RGBA)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, a_width, a_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    }
        
    const unsigned int size = a_width * a_height;
    const unsigned int handle = GetHandle();
    
    glGenBuffers(2, m_pbo);
    
    glBindBuffer(GL_PIXEL_PACK_BUFFER, m_pbo[0]);
    if (a_pixelFormat == GL_RGBA)
    {
        glBufferData(GL_PIXEL_PACK_BUFFER, size * 4, 0, GL_STREAM_READ);
    }
    else
    {
        glBufferData(GL_PIXEL_PACK_BUFFER, size * 3, 0, GL_STREAM_READ);
    }

    glBindBuffer(GL_PIXEL_PACK_BUFFER, m_pbo[1]);
    if (a_pixelFormat == GL_RGBA)
    {
        glBufferData(GL_PIXEL_PACK_BUFFER, size * 4, 0, GL_STREAM_READ);
    }
    else
    {
        glBufferData(GL_PIXEL_PACK_BUFFER, size * 3, 0, GL_STREAM_READ);
    }

    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}

PBOTexture::~PBOTexture()
{
    glDeleteBuffers(2, m_pbo);

    delete m_pbo;
}

unsigned int* PBOTexture::GetPBOS() const
{
    return m_pbo;
}
unsigned int PBOTexture::GetCurrentPBO() const
{
    return m_pbo[m_readIndex];
}
unsigned int PBOTexture::GetCurrentPBOIndex() const
{
    return m_readIndex;
}

void PBOTexture::Unbind()
{
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}

void* PBOTexture::MapPixels()
{
    m_readIndex = (m_readIndex + 1) % 2;
    const int nextIndex = (m_readIndex + 1) % 2;

    const int width = GetWidth();
    const int height = GetHeight();

    glBindBuffer(GL_PIXEL_PACK_BUFFER, m_pbo[m_readIndex]);
    glReadPixels(0, 0, width, height, m_pixelFormat, GL_UNSIGNED_BYTE, 0);

    glBindBuffer(GL_PIXEL_PACK_BUFFER, m_pbo[nextIndex]);

    return glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
}
void PBOTexture::UnmapPixels()
{
    glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
}