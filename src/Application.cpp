#include "Application.h"

#include <assert.h>
#include <stdio.h>

void ErrorCallback(int a_error, const char* a_description)
{
    printf("Error: %s \n", a_description);
}

Application::Application(int a_width, int a_height, const char* a_application) :
    m_width(a_width),
    m_height(a_height)
{
    assert(glfwInit());
    glfwSetErrorCallback(ErrorCallback);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(a_width, a_height, a_application, NULL, NULL);
    if (!m_window)
    {
        glfwTerminate();
        assert(0);
    }
    glfwMakeContextCurrent(m_window);
    glfwMaximizeWindow(m_window);

    assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress));

    glfwSwapInterval(1);
}

Application::~Application()
{
    glfwDestroyWindow(m_window);

    glfwTerminate();
}

GLFWwindow* Application::GetWindow() const
{
    return m_window;
}

void Application::Run()
{
    double prevTime = glfwGetTime();

    while (!glfwWindowShouldClose(m_window))
    {
        double time = glfwGetTime();

        glfwGetWindowSize(m_window, &m_width, &m_height);
        glViewport(0, 0, m_width, m_height);

        glfwPollEvents();

        Update(time - prevTime);

        glfwSwapBuffers(m_window);

        prevTime = time;
    }
}