#include "ApplicationConfig.h"

#include <stdio.h>

#include "AppMain.h"

int main (int argc, char *argv[])
{
    printf("Anny.Mei: %d.%d \n", Application_VERSION_MAJOR, Application_VERSION_MINOR);

    Application* app = new AppMain(640, 480);

    app->Run();

    delete app;

    return 0;
}
