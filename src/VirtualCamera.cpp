#include "VirtualCamera.h"

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

VirtualCamera::VirtualCamera(unsigned int a_width, unsigned int a_height, unsigned int a_pixelFormat)
{
    const char* videoDevice = "/dev/video0";

    m_driver = open(videoDevice, O_RDWR);
    assert(m_driver >= 0);
    
    int returnCode;

    v4l2_format videoFormat;
    memset(&videoFormat, 0, sizeof(videoFormat));

    size_t lineWidth;

    GetSize(a_pixelFormat, a_width, a_height, lineWidth, m_frameSize);

    videoFormat.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;

    ioctl(m_driver, VIDIOC_G_FMT, &videoFormat);
    videoFormat.fmt.pix.width = a_width;
    videoFormat.fmt.pix.height = a_height;
    videoFormat.fmt.pix.bytesperline = lineWidth;
    videoFormat.fmt.pix.pixelformat = a_pixelFormat;
    videoFormat.fmt.pix.sizeimage = m_frameSize;
    videoFormat.fmt.pix.field = V4L2_FIELD_NONE;
    videoFormat.fmt.pix.colorspace = V4L2_COLORSPACE_SRGB;

    m_videoBuffer = new unsigned char[m_frameSize];

    returnCode = ioctl(m_driver, VIDIOC_S_FMT, &videoFormat);

    if (returnCode == -1)
    {
        printf("Error getting video format: %s \n", strerror(errno));
        assert(0);
    }

    printf("Initialised Camera \n");
}

VirtualCamera::~VirtualCamera()
{
    close(m_driver);

    delete m_videoBuffer;
 
    printf("Closed Camera \n");
}

size_t VirtualCamera::GetFrameSize() const
{
    return m_frameSize;
}
unsigned char* VirtualCamera::GetVideoBuffer() const
{
    return m_videoBuffer;
}

void VirtualCamera::PushFrame() const
{
    const size_t dataPushed = write(m_driver, m_videoBuffer, m_frameSize);

    assert(dataPushed == m_frameSize);
}