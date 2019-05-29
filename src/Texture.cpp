#include "Texture.h"

#include <glad/glad.h>

Texture::Texture(int a_width, int a_height, int a_pixelFormat) : m_width(a_width), m_height(a_height)
{
    glGenTextures(1, &m_handle);
    glBindTexture(GL_TEXTURE_2D, m_handle);
    glTexImage2D(GL_TEXTURE_2D, 0, a_pixelFormat, m_width, m_height, 0, a_pixelFormat, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

Texture::~Texture()
{
    glDeleteTextures(GL_TEXTURE_2D, &m_handle);
}

unsigned int Texture::GetHandle() const
{
    return m_handle;
}
int Texture::GetWidth() const
{
    return m_width;
}
int Texture::GetHeight() const
{
    return m_height;
}