#include "AppMain.h"

#include <stdio.h>

#undef IMGUI_IMPL_OPENGL_LOADER_GL3W
#undef IMGUI_IMPL_OPENGL_LOADER_GLEW
#define IMGUI_IMPL_OPENGL_LOADER_GLAD

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "ModelEditor.h"
#include "Texture.h"
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
    m_modelEditor = new ModelEditor("Test.kra");

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(GetWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 130");
}
AppMain::~AppMain()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    delete m_webcamController;
    delete m_modelEditor;
}

void AppMain::Update(double a_delta)
{
    glEnable(GL_DEPTH_TEST);

    m_webcamController->Bind();

    glClearColor(m_backgroundColor[0], m_backgroundColor[1], m_backgroundColor[2], 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_webcamController->Update();

    glClearColor(0.3, 0.3, 0.3, 1);

    // It would seem I have to clear it every frame even if I made no change
    // If I where to guess by the image being populated images from other applications
    // the memory is shared and switching between multiple applications
    // Does not happen as often when I am the only app running
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos({ 10, 10 }, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize({ 660, 520 }, ImGuiCond_Appearing);
    ImGui::Begin("Preview");

    ImGui::Image((ImTextureID)m_webcamController->GetTexture()->GetHandle(), { 640, 480 });

    ImGui::End();

    ImGui::SetNextWindowPos({ 670, 10 }, ImGuiCond_Appearing);
    ImGui::Begin("Options");

    ImGui::ColorPicker3("Background Color", m_backgroundColor);

    ImGui::End();

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}