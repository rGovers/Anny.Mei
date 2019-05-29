#include "PBOTexture.h"

#include <glad/glad.h>
#include <string.h>

PBOTexture::PBOTexture(int a_width, int a_height, int a_pixelFormat)
 : Texture(a_width, a_height, a_pixelFormat), m_pbo(new unsigned int[2]), m_readIndex(0)
{
    glGenBuffers(2, m_pbo);

    unsigned int handle = GetHandle();
    glBindTexture(GL_TEXTURE_2D, handle);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo[m_readIndex]);

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, a_width, a_height, a_pixelFormat, GL_UNSIGNED_BYTE, 0);

    unsigned int size = a_width * a_height;
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 1);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, size, 0, GL_STREAM_DRAW);

    GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
    if (ptr)
    {
        memset(ptr, 0xFF, size);
        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    }

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
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

void PBOTexture::SwitchPBO()
{
    m_readIndex = (m_readIndex + 1) % 2;
}