#include "AppMain.h"

#include <fstream>
#include <list>
#include <stdio.h>

#include "ColorTheme.h"
#include "DataStore.h"
#include "FileDialog.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "ModelController.h"
#include "Texture.h"
#include "WebcamController.h"
#include "Workspace.h"

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

void GUIStyle()
{
    ImGuiStyle* style = &ImGui::GetStyle();
    
    style->WindowRounding = 0.0f;

    ImVec4* colors = style->Colors;

    const ImVec4 iBackColor = ImVec4(BACKGROUND_COLOR.x, BACKGROUND_COLOR.y, BACKGROUND_COLOR.z, BACKGROUND_COLOR.w);
    const ImVec4 iTitleColor = ImVec4(TITLE_COLOR.x, TITLE_COLOR.y, TITLE_COLOR.z, TITLE_COLOR.w);
    const ImVec4 iActiveTitleColor = ImVec4(ACTIVE_TITLE_COLOR.x, ACTIVE_TITLE_COLOR.y, ACTIVE_TITLE_COLOR.z, ACTIVE_TITLE_COLOR.w);

    const ImVec4 iInactiveColor = ImVec4(INACTIVE_COLOR.x, INACTIVE_COLOR.y, INACTIVE_COLOR.z, INACTIVE_COLOR.w);
    const ImVec4 iHoverColor = ImVec4(HOVER_COLOR.x, HOVER_COLOR.y, HOVER_COLOR.z, HOVER_COLOR.w);
    const ImVec4 iActiveColor = ImVec4(ACTIVE_COLOR.x, ACTIVE_COLOR.y, ACTIVE_COLOR.z, ACTIVE_COLOR.w);

    colors[ImGuiCol_WindowBg]               = iBackColor;
    colors[ImGuiCol_TitleBg]                = iTitleColor;
    colors[ImGuiCol_TitleBgActive]          = iActiveTitleColor;

    colors[ImGuiCol_Header]                 = iInactiveColor;
    colors[ImGuiCol_HeaderHovered]          = iHoverColor;
    colors[ImGuiCol_HeaderActive]           = iActiveColor;

    colors[ImGuiCol_Tab]                    = iInactiveColor;
    colors[ImGuiCol_TabHovered]             = iHoverColor;
    colors[ImGuiCol_TabActive]              = iActiveColor;
    colors[ImGuiCol_TabUnfocused]           = iInactiveColor;
    colors[ImGuiCol_TabUnfocusedActive]     = iHoverColor;

    colors[ImGuiCol_FrameBg]                = iInactiveColor;
    colors[ImGuiCol_FrameBgHovered]         = iHoverColor;
    colors[ImGuiCol_FrameBgActive]          = iActiveColor;

    colors[ImGuiCol_Button]                 = iInactiveColor;
    colors[ImGuiCol_ButtonHovered]          = iHoverColor;
    colors[ImGuiCol_ButtonActive]           = iActiveColor;
}

AppMain::AppMain(int a_width, int a_height) : 
    Application(a_width, a_height, "Anny.Mei")
{
    m_webcamController = new WebcamController(1280, 720);

    m_modelController = nullptr;

    m_filePath = nullptr;

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    ImGui::StyleColorsDark();
    GUIStyle();

    ImGui_ImplGlfw_InitForOpenGL(GetWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 130");

    FileDialog::Create();

    m_workspace = new Workspace();

    m_resetWindows = true;

    m_windowUpdateTimer = 0.0;
}
AppMain::~AppMain()
{
    FileDialog::Destroy();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    delete m_webcamController;

    delete m_workspace;

    if (m_filePath != nullptr)
    {
        delete[] m_filePath;
    }

    if (m_modelController != nullptr)
    {
        delete m_modelController;
    }
}

void AppMain::New()
{
    if (m_modelController != nullptr)
    {
        delete m_modelController;
    }

    if (m_dataStore != nullptr)
    {
        delete m_dataStore;
    }

    m_dataStore = new DataStore();

    m_workspace->Init();

    m_modelController = new ModelController();
    
    if (m_filePath != nullptr)
    {
        delete[] m_filePath;
        m_filePath = nullptr;
    }
}
void AppMain::Open()
{
    char* const* const filters = new char*[1] { "*.aMei" };
    const char* path = FileDialog::OpenFile("Open Project File", filters, 1);

    if (path != nullptr)
    {
        if (path[0] != 0)
        {
            if (m_dataStore != nullptr)
            {
                delete m_dataStore;
            }

            m_dataStore = new DataStore();

            if (m_filePath != nullptr)
            {
                delete[] m_filePath;
            }

            const size_t len = strlen(path);
            m_filePath = new char[len + 1];
            strcpy(m_filePath, path);

            mz_zip_archive zip;
            memset(&zip, 0, sizeof(zip));

            if (mz_zip_reader_init_file(&zip, m_filePath, 0))
            {
                if (m_modelController != nullptr)
                {
                    delete m_modelController;
                }

                m_modelController = ModelController::Load(zip); 

                m_workspace->Open(zip);
            } 
            else
            {
                m_workspace->Init();
            } 

            mz_zip_reader_end(&zip);

            if (m_modelController == nullptr)
            {
                m_modelController = new ModelController();
            }
        }
        else if (m_filePath != nullptr)
        {
            delete[] m_filePath;
            m_filePath = nullptr;
        }
    }

    delete[] filters;
}
void AppMain::Save() const
{
    mz_zip_archive zip;
    memset(&zip, 0, sizeof(zip));

    if (mz_zip_writer_init_file(&zip, m_filePath, 0))
    {
        m_modelController->Save(zip);

        m_workspace->Save(zip);

        mz_zip_writer_finalize_archive(&zip);
        mz_zip_writer_end(&zip);
    }
}
void AppMain::SaveAs()
{
    char* const* const filters = new char*[1] { "*.aMei" };
    const char* path = FileDialog::SaveFile("Save Project File", filters, 1);

    if (path != nullptr && path[0] != 0)
    {
        if (m_filePath != nullptr)
        {
            delete[] m_filePath;
        }

        const size_t len = strlen(path);
        m_filePath = new char[len + 1];
        strcpy(m_filePath, path);

        Save();
    }

    delete[] filters;
}

void AppMain::Input()
{
    GLFWwindow* window = GetWindow();
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL))
    {
        if (glfwGetKey(window, GLFW_KEY_N))
        {
            New();
        }
        else if (glfwGetKey(window, GLFW_KEY_O))
        {
            Open();
        }
        else if (glfwGetKey(window, GLFW_KEY_S) && m_filePath != nullptr)
        {
            Save();
        }
        else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT))
        {
            if (glfwGetKey(window, GLFW_KEY_S))
            {
                SaveAs();
            }
        }
    }
}

void AppMain::ResetDockedWindows()
{
    ImGuiID id = ImGui::GetID("Dock Main");

    ImGui::DockBuilderRemoveNode(id);
    ImGui::DockBuilderAddNode(id, ImGuiDockNodeFlags_CentralNode | ImGuiDockNodeFlags_NoResize);

    ImGui::DockBuilderSetNodePos(id, { 0, 18 });
    ImGui::DockBuilderSetNodeSize(id, { GetWidth(), GetHeight() - 20});

    ImGuiID dockMainID = id;
    ImGuiID dockBottom = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Down, 0.2f, nullptr, &dockMainID);
    ImGuiID dockRight = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Right, 0.1f, nullptr, &dockMainID);
    ImGuiID dockTop = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Up, 0.05f, nullptr, &dockMainID);
    ImGuiID dockLeft = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Left, 0.1f, nullptr, &dockMainID);

    ImGui::DockBuilderDockWindow("Preview", dockMainID);
    ImGui::DockBuilderDockWindow("Editor", dockMainID);

    ImGui::DockBuilderDockWindow("Options", dockRight);
    ImGui::DockBuilderDockWindow("Properties", dockRight);

    ImGui::DockBuilderDockWindow("Texture List", dockLeft);
    ImGui::DockBuilderDockWindow("Model List", dockLeft);
    ImGui::DockBuilderDockWindow("Skeleton Hierarchy", dockLeft);

    ImGui::DockBuilderDockWindow("Model Tools", dockTop);

    ImGui::DockBuilderDockWindow("Anim Key Frames", dockBottom);

    ImGui::DockBuilderFinish(id);
}

void AppMain::Update(double a_delta)
{
    glEnable(GL_DEPTH_TEST);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    Input();

    if (m_windowUpdateTimer >= 0.5f)
    {
        std::string windowTitle = "Anny.Mei ";

        if (m_filePath != nullptr)
        {
            windowTitle += std::string("[Working Path: ") + m_filePath + "] ";
        }

        // Cant be stuffed gives me a close enough read on FPS
        int fps = (int)(1 / a_delta);
        windowTitle += "[FPS: " + std::to_string(fps) + "]";

        glfwSetWindowTitle(GetWindow(), windowTitle.c_str());

        m_windowUpdateTimer -= 0.5f;
    }

    m_windowUpdateTimer += a_delta;
    
    m_webcamController->Bind();

    if (m_modelController != nullptr)
    {
        m_modelController->DrawModel(m_workspace, a_delta);
    }

    m_webcamController->Update();

    glClearColor(0.3, 0.3, 0.3, 1);

    // It would seem I have to clear it every frame even if I made no change
    // If I where to guess by the image being populated images from other applications
    // the memory is shared and switching between multiple applications
    // Does not happen as often when I am the only app running
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New", "Ctrl+N"))
            {
                New();
            }
            if (ImGui::MenuItem("Open", "Ctrl+O"))
            {
                Open();
            }
            
            const bool enabledModel = m_workspace->IsEnabled();

            if (ImGui::MenuItem("Save", "Ctrl+S", nullptr, m_filePath != nullptr && enabledModel))
            {
                Save();
            }
            if (ImGui::MenuItem("Save As", "Ctrl+Shift+S", nullptr, enabledModel))
            {
                SaveAs();   
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Import Image File", "", nullptr, enabledModel))
            {
                char* const* const filters = new char*[2] { "*.png", "*.kra" };
                const char* filePath = FileDialog::OpenFile("Open Image File", filters, 2);
                
                delete[] filters;
                
                if (filePath != nullptr && filePath[0] != 0)
                {
                    m_workspace->LoadTexture(filePath);
                }
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Exit", "Alt+F4"))
            {
                glfwSetWindowShouldClose(GetWindow(), true);
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Workspace"))
        {
            if (ImGui::MenuItem("Default Layout"))
            {
                m_resetWindows = true;
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    if (m_resetWindows)
    {
        m_resetWindows = false;

        ResetDockedWindows();
    }

    if (m_modelController != nullptr)
    {
        m_modelController->Update(*m_webcamController);
    }

    m_workspace->Update(a_delta);

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void AppMain::Resize(int a_newWidth, int a_newHeight)
{
    m_resetWindows = true;
}