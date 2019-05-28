#include "Application.h"

#include <assert.h>
#include <stdio.h>

void ErrorCallback(int a_error, const char* a_description)
{
    printf("Error: %s \n", a_description);
}

Application::Application(int a_width, int a_height, const char* a_application)
{
    assert(glfwInit());
    glfwSetErrorCallback(ErrorCallback);

    m_window = glfwCreateWindow(a_width, a_height, a_application, NULL, NULL);
    if (!m_window)
    {
        glfwTerminate();
        assert(0);
    }

    // glfwSwapInterval(1);

    glfwMakeContextCurrent(m_window);
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

        glfwPollEvents();

        Update(time - prevTime);

        // glfwSwapBuffers(m_window);

        prevTime = time;
    }
}