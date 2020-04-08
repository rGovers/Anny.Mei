#pragma once

#include "Texture.h"

class PBOTexture : public Texture
{
private:
    unsigned int* m_pbo;

    unsigned int  m_readIndex;

    int           m_pixelFormat;
protected:

public:
    PBOTexture() = delete;
    PBOTexture(int a_width, int a_height, int a_pixelFormat, int a_samples);
    ~PBOTexture();

    unsigned int* GetPBOS() const;
    unsigned int GetCurrentPBO() const;
    unsigned int GetCurrentPBOIndex() const;

    void Unbind();

    void* MapPixels();
    void UnmapPixels();
};