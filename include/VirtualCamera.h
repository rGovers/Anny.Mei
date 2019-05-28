#pragma once

#include <stddef.h>

class VirtualCamera
{
private:
    int    m_driver;
    
    size_t m_frameSize;
    unsigned char*  m_videoBuffer;
protected:

public:
    VirtualCamera() = delete;
    VirtualCamera(unsigned int a_width, unsigned int a_height, unsigned int a_pixelFormat);
    ~VirtualCamera();

    size_t GetFrameSize() const;
    unsigned char* GetVideoBuffer() const;

    void PushFrame() const; 
};