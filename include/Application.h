#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Application
{
private:
    GLFWwindow* m_window;

protected:
    virtual void Update(double a_delta) = 0;

public:
    Application() = delete;
    Application(int a_width, int a_height, const char* a_title = "Application");
    virtual ~Application();

    GLFWwindow* GetWindow() const;

    void Run();
};
