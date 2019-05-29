#include "AppMain.h"

#include <stdio.h>

#include "WebcamController.h"

void GLAPIENTRY
MessageCallback( GLenum a_source,
                 GLenum a_type,
                 GLuint a_id,
                 GLenum a_severity,
                 GLsizei a_length,
                 const GLchar* a_message,
                 const void* a_userParam )
{
  printf("GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( a_type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            a_type, a_severity, a_message );
}

AppMain::AppMain(int a_width, int a_height) : Application(a_width, a_height, "Anny.Mei")
{
    m_webcamController = new WebcamController();

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
}
AppMain::~AppMain()
{
    delete m_webcamController;
}

void AppMain::Update(double a_delta)
{
    glEnable(GL_DEPTH_TEST);

    m_webcamController->Bind();

    glClearColor(0, 1, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_webcamController->Update();

    glClearColor(1, 0, 0, 1);

    // It would seem I have to clear it every frame even if I made no change
    // If I where to guess by the image being populated images from other applications
    // the memory is shared and switching between multiple applications
    // Does not happen as often when I am the only app running
    glClear(GL_COLOR_BUFFER_BIT);    
}