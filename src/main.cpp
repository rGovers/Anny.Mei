#include "ApplicationConfig.h"

#include <stdio.h>

#include "AppMain.h"

int main (int argc, char *argv[])
{
    printf("Anny.Mei: %d.%d \n", APPLICATION_VERSION_MAJOR, APPLICATION_VERSION_MINOR);

    Application* app = new AppMain(640, 480);

    app->Run();

    delete app;

    return 0;
}
