#include "AppMain.h"

#include <fstream>
#include <list>
#include <stdio.h>

#undef IMGUI_IMPL_OPENGL_LOADER_GL3W
#undef IMGUI_IMPL_OPENGL_LOADER_GLEW
#define IMGUI_IMPL_OPENGL_LOADER_GLAD

#include "FileDialog.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "miniz.h"
#include "ModelController.h"
#include "ModelEditor.h"
#include "SkeletonEditor.h"
#include "Texture.h"
#include "TextureEditor.h"
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

AppMain::AppMain(int a_width, int a_height) : 
    Application(a_width, a_height, "Anny.Mei")
{
    m_webcamController = new WebcamController(1280, 720);

    m_modelController = nullptr;

    m_skeletonEditor = nullptr;
    m_textureEditor = nullptr;
    m_modelEditor = nullptr;

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigWindowsMoveFromTitleBarOnly = true;

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

    if (m_skeletonEditor != nullptr)
    {
        delete m_skeletonEditor;
    }
    if (m_modelController != nullptr)
    {
        delete m_modelController;
    }
    if (m_modelEditor != nullptr)
    {
        delete m_modelEditor;
    }
    if (m_textureEditor != nullptr)
    {
        delete m_textureEditor;
    }
}

void AppMain::New()
{
    if (m_modelController != nullptr)
    {
        delete m_modelController;
    }
    if (m_textureEditor != nullptr)
    {
        delete m_textureEditor;
    }    
    if (m_skeletonEditor != nullptr)
    {
        delete m_skeletonEditor;
    }
    if (m_modelEditor != nullptr)
    {
        delete m_modelEditor;
    }

    if (m_dataStore != nullptr)
    {
        delete m_dataStore;
    }

    m_dataStore = new DataStore();

    m_skeletonEditor = new SkeletonEditor();
    m_modelController = new ModelController();
    
    m_textureEditor = new TextureEditor();
    m_modelEditor = new ModelEditor();

    m_filePath = nullptr;
}
void AppMain::Open()
{
    char* const* const filters = new char*[1] { "*.aMei" };
    m_filePath = FileDialog::OpenFile("Open Project File", filters, 1);

    if (m_dataStore != nullptr)
    {
        delete m_dataStore;
    }
        
    m_dataStore = new DataStore();

    if (m_filePath != nullptr)
    {
        const std::string str = std::string("Anny.Mei [") + m_filePath + "]";
        glfwSetWindowTitle(GetWindow(), str.c_str());

        if (m_filePath[0] != 0)
        {
            mz_zip_archive zip;
            memset(&zip, 0, sizeof(zip));

            if (mz_zip_reader_init_file(&zip, m_filePath, 0))
            {
                if (m_modelController != nullptr)
                {
                    delete m_modelController;
                }
                if (m_textureEditor != nullptr)
                {
                    delete m_textureEditor;
                }
                if (m_skeletonEditor != nullptr)
                {
                    delete m_skeletonEditor;
                }

                

                m_modelController = ModelController::Load(zip); 
                m_skeletonEditor = SkeletonEditor::Load(zip);
                m_textureEditor = TextureEditor::Load(zip);
                m_modelEditor = ModelEditor::Load(zip);
            } 

            mz_zip_reader_end(&zip);

            if (m_modelController == nullptr)
            {
                m_modelController = new ModelController();
            }
            if (m_skeletonEditor == nullptr)
            {
                m_skeletonEditor = new SkeletonEditor();
            }
            if (m_textureEditor == nullptr)
            {
                m_textureEditor = new TextureEditor();
            }
            if (m_modelEditor == nullptr)
            {
                m_modelEditor = new ModelEditor();
            }
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
    mz_zip_archive zip;
    memset(&zip, 0, sizeof(zip));

    if (mz_zip_writer_init_file(&zip, m_filePath, 0))
    {
        m_modelController->Save(zip);
        m_textureEditor->Save(zip);
        m_skeletonEditor->Save(zip);
        m_modelEditor->Save(zip);

        mz_zip_writer_finalize_archive(&zip);
        mz_zip_writer_end(&zip);
    }
}
void AppMain::SaveAs()
{
    char* const* const filters = new char*[1] { "*.aMei" };
    m_filePath = FileDialog::SaveFile("Save Project File", filters, 1);

    if (m_filePath != nullptr)
    {
        const std::string str = std::string("Anny.Mei [") + m_filePath + "]";
        glfwSetWindowTitle(GetWindow(), str.c_str());

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
        m_modelController->DrawModel(m_skeletonEditor, a_delta);
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
            
            const bool enabledModel = m_textureEditor != nullptr;

            if (ImGui::MenuItem("Save", "Ctrl+S", nullptr, m_filePath != nullptr && enabledModel))
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
                        m_textureEditor->LoadTexture(filePath);
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

    if (m_skeletonEditor != nullptr)
    {
        m_skeletonEditor->Update(a_delta);
    }
    if (m_textureEditor != nullptr)
    {
        m_textureEditor->Update(a_delta, m_modelEditor);
    }
    if (m_modelController != nullptr)
    {
        m_modelController->Update(*m_webcamController);
    }
    if (m_modelEditor != nullptr)
    {
        m_modelEditor->Update(a_delta);
    }

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}