#pragma once

#include "VirtualCameras/VirtualCamera.h"

#ifndef WIN32
#include <stddef.h>

class V4L2VirtualCamera
{
private:
    int             m_driver;
                
    size_t          m_frameSize;
    unsigned char*  m_videoBuffer;
protected:
    V4L2VirtualCamera();

public:
    ~V4L2VirtualCamera();
    
    V4L2VirtualCamera* CreateCamera(unsigned int a_width, unsigned int a_height, unsigned int a_pixelFormat);

    virtual unsigned int GetFrameSize() const;
    virtual unsigned char* GetVideoBuffer() const;

    virtual void PushFrame() const; 
};
#endif