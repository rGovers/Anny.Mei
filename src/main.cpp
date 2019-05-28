// A simple program that computes the square root of a number
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <linux/videodev2.h>

#include "ApplicationConfig.h"
#include "VirtualCamera.h"

int main (int argc, char *argv[])
{
    printf("Anny.Mei: %d.%d \n", Application_VERSION_MAJOR, Application_VERSION_MINOR);

    VirtualCamera* virtualCamera = new VirtualCamera(1920, 1080, V4L2_PIX_FMT_BGR24);

    size_t size = virtualCamera->GetFrameSize();
    unsigned char* buffer = virtualCamera->GetVideoBuffer();

    for (int i = 0; i < size; i += 3)
    {
        buffer[i] = 0x00;
        buffer[i + 1] = 0x00;
        buffer[i + 2] = 0x00;
    }

    virtualCamera->PushFrame();

    char buff[128];

    std::cin >> buff;

    delete virtualCamera;

    return 0;
}
