#include "ApplicationConfig.h"

#include <stdio.h>

#include "AppMain.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>

int main (int argc, char *argv[])
{
    printf("Anny.Mei: %d.%d \n", APPLICATION_VERSION_MAJOR, APPLICATION_VERSION_MINOR);

    Application* app = new AppMain(640, 480);

    app->Run();

    delete app;

    return 0;
}
