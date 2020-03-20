#include "VirtualCameras/V4L2VirtualCamera.h"

#ifndef WIN32
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

void GetSize(unsigned int a_format, unsigned int a_width, unsigned int a_height, size_t& a_lineWidth, size_t& a_frameWidth)
{
    switch (a_format)
    {
    case V4L2_PIX_FMT_ABGR32:
    {
        a_lineWidth = a_width * 4;
        
        break;
    }
    case V4L2_PIX_FMT_BGR24:
    {
        a_lineWidth = a_width * 3;

        break;
    }
    }
    a_frameWidth = a_lineWidth * a_height;
}

V4L2VirtualCamera::V4L2VirtualCamera()
{
    m_driver = -1;

    m_videoBuffer = nullptr;
}
V4L2VirtualCamera::~V4L2VirtualCamera()
{
    if (m_driver >= 0)
    {
        close(m_driver);
    }

    if (m_videoBuffer != nullptr)
    {
        delete[] m_videoBuffer;
    }
 
    printf("Closed Camera \n");
}

V4L2VirtualCamera* V4L2VirtualCamera::CreateCamera(unsigned int a_width, unsigned int a_height, unsigned int a_pixelFormat)
{
    V4L2VirtualCamera* camera = new V4L2VirtualCamera();

    const char* videoDevice = "/dev/video0";

    camera->m_driver = open(videoDevice, O_RDWR);
    if (camera->m_driver <= 0)
    {
        delete camera;

        return nullptr;
    }
    
    int returnCode;

    v4l2_format videoFormat;
    memset(&videoFormat, 0, sizeof(videoFormat));

    size_t lineWidth;

    GetSize(a_pixelFormat, a_width, a_height, lineWidth, camera->m_frameSize);

    videoFormat.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;

    ioctl(camera->m_driver, VIDIOC_G_FMT, &videoFormat);
    videoFormat.fmt.pix.width = a_width;
    videoFormat.fmt.pix.height = a_height;
    videoFormat.fmt.pix.bytesperline = lineWidth;
    videoFormat.fmt.pix.pixelformat = a_pixelFormat;
    videoFormat.fmt.pix.sizeimage = camera->m_frameSize;
    videoFormat.fmt.pix.field = V4L2_FIELD_NONE;
    videoFormat.fmt.pix.colorspace = V4L2_COLORSPACE_SRGB;

    camera->m_videoBuffer = new unsigned char[camera->m_frameSize];

    returnCode = ioctl(camera->m_driver, VIDIOC_S_FMT, &videoFormat);

    if (returnCode == -1)
    {
        printf("Error getting video format: %s \n", strerror(errno));
        
        delete camera;

        return nullptr;
    }

    printf("Initialised Camera \n");

    return camera;
}

unsigned int V4L2VirtualCamera::GetFrameSize() const
{
    return m_frameSize;
}
unsigned char* V4L2VirtualCamera::GetVideoBuffer() const
{
    return m_videoBuffer;
}

void V4L2VirtualCamera::PushFrame() const
{
    const size_t dataPushed = write(m_driver, m_videoBuffer, m_frameSize);

    assert(dataPushed == m_frameSize);
}
#endif