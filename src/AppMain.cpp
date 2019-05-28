#include "AppMain.h"

#include <linux/videodev2.h>

#include "VirtualCamera.h"

AppMain::AppMain(int a_width, int a_height) : Application(a_width, a_height, "Anny.Mei")
{
    m_virtualCamera = new VirtualCamera(640, 480, V4L2_PIX_FMT_BGR24);

    size_t size = m_virtualCamera->GetFrameSize();
    unsigned char* buffer = m_virtualCamera->GetVideoBuffer();

    for (int i = 0; i < size; i += 3)
    {
        buffer[i] = 0xFF;
        buffer[i + 1] = 0x00;
        buffer[i + 2] = 0x00;
    }
}
AppMain::~AppMain()
{
    delete m_virtualCamera;
}

void AppMain::Update(double a_delta)
{
    m_virtualCamera->PushFrame();
}