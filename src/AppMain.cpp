#include "AppMain.h"

#include <stdio.h>
#include <fstream>

#undef IMGUI_IMPL_OPENGL_LOADER_GL3W
#undef IMGUI_IMPL_OPENGL_LOADER_GLEW
#define IMGUI_IMPL_OPENGL_LOADER_GLAD

#include "FileDialog.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "ModelController.h"
#include "ModelEditor.h"
#include "Texture.h"
#include "WebcamController.h"
#include "ZipLib/ZipFile.h"

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

AppMain::AppMain(int a_width, int a_height) : 
    Application(a_width, a_height, "Anny.Mei"),
    m_modelController(nullptr),
    m_modelEditor(nullptr),
    m_menuState(new bool(true)),
    m_filePath(nullptr)
{
    m_webcamController = new WebcamController();

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(GetWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 130");

    FileDialog::Create();
}
AppMain::~AppMain()
{
    FileDialog::Destroy();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    delete m_webcamController;

    if (m_modelEditor != nullptr)
    {
        delete m_modelEditor;
        m_modelEditor = nullptr;
    }
}

void AppMain::New()
{
    if (m_modelController != nullptr)
    {
        delete m_modelController;
    }
    if (m_modelEditor != nullptr)
    {
        delete m_modelEditor;
    }    

    m_modelController = new ModelController();
    m_modelEditor = new ModelEditor();
    m_filePath = nullptr;
}
void AppMain::Open()
{
    char* const* const filters = new char*[1] { "*.aMei" };
    m_filePath = FileDialog::OpenFile("Open Project File", filters, 1);

    if (m_filePath != nullptr)
    {
        if (m_filePath[0] != 0)
        {
            ZipArchive::Ptr zip = ZipFile::Open(m_filePath);

            if (m_modelController != nullptr)
            {
                delete m_modelController;
            }
            if (m_modelEditor != nullptr)
            {
                delete m_modelEditor;
            }

            m_modelController = ModelController::Load(zip);
        }
        else
        {
            m_filePath = nullptr;
        }
    }

    delete[] filters;
}
void AppMain::Save() const
{
    std::fstream fstream;
    fstream.open(m_filePath, std::ios_base::openmode::_S_bin | std::ios_base::openmode::_S_out);
    if (fstream.good())
    {
        ZipArchive::Ptr zipArchive = ZipArchive::Create();

        std::istream* mControllerStream = nullptr;
        std::istream* mEditorStream = nullptr;

        if (m_modelEditor != nullptr)
        {
            mEditorStream = m_modelEditor->SaveToStream();

            if (mEditorStream != nullptr)
            {
                std::shared_ptr<ZipArchiveEntry> entryptr = zipArchive->CreateEntry("model.prop");
                entryptr->SetCompressionStream(*mEditorStream);
            }
        }
        if (m_modelController != nullptr)
        {
            std::shared_ptr<ZipArchiveEntry> entryptr = zipArchive->CreateEntry("main.conf");

            mControllerStream = m_modelController->SaveToStream();
            entryptr->SetCompressionStream(*mControllerStream);
        }

        zipArchive->WriteToStream(fstream);

        fstream.close();

        if (mControllerStream != nullptr)
        {
            delete mControllerStream;
        }
        if (mEditorStream != nullptr)
        {
            delete mEditorStream;
        }
    }
}
void AppMain::SaveAs()
{
    char* const* const filters = new char*[1] { "*.aMei" };
    m_filePath = FileDialog::SaveFile("Save Project File", filters, 1);

    delete[] filters;

    Save();
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

void AppMain::Update(double a_delta)
{
    glEnable(GL_DEPTH_TEST);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    Input();

    m_webcamController->Bind();

    if (m_modelController != nullptr)
    {
        m_modelController->DrawModel();
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
            
            const bool enabledModel = m_modelEditor != nullptr;

            if (ImGui::MenuItem("Save", "Ctrl+S", nullptr, m_filePath != nullptr))
            {
                Save();
            }
            if (ImGui::MenuItem("Save As", "Ctrl+Shift+S", nullptr, enabledModel))
            {
                SaveAs();   
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Open Image File", "", nullptr, enabledModel))
            {
                char* const* const filters = new char*[1] { "*.png" };
                const char* filePath = FileDialog::OpenFile("Open Image File", filters, 1);
                
                delete[] filters;
                
                if (filePath != nullptr)
                {
                    if (filePath[0] != 0)
                    {
                        m_modelEditor->LoadTexture(filePath);
                    }
                }
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Exit", "Alt+F4"))
            {
                glfwSetWindowShouldClose(GetWindow(), true);
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    if (m_modelEditor != nullptr)
    {
        m_modelEditor->Update(a_delta);
    }
    if (m_modelController != nullptr)
    {
        m_modelController->Update(a_delta, *m_webcamController);
    }

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}