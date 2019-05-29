#pragma once

class Texture
{
private:
    unsigned int m_handle;

    int          m_width;
    int          m_height;

protected:

public:
    Texture() = delete;
    Texture(int a_width, int a_height, int a_pixelFormat);
    virtual ~Texture();

    unsigned int GetHandle() const;
    int GetWidth() const;
    int GetHeight() const;
};