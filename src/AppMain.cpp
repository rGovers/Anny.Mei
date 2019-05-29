#include "AppMain.h"

#include "WebcamController.h"

AppMain::AppMain(int a_width, int a_height) : Application(a_width, a_height, "Anny.Mei")
{
    m_webcamController = new WebcamController();

    glClearColor(1, 0, 0, 1);
}
AppMain::~AppMain()
{
    delete m_webcamController;
}

void AppMain::Update(double a_delta)
{
    m_webcamController->Bind();

    glClear(GL_COLOR_BUFFER_BIT);

    m_webcamController->Update();

    // It would seem I have to clear it every frame even if I made no change
    // If I where to guess by the image being populated images from other applications
    // the memory is shared and switching between multiple applications
    // Does not happen as often when I am the only app running
    glClear(GL_COLOR_BUFFER_BIT);    
}