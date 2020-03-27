#include "Exporters/ImageSetExporter.h"

#include <glad/glad.h>
#include <stb/stb_image_write.h>
#include <string.h>

#include "AnimControl.h"
#include "imgui.h"
#include "ModelController.h"
#include "PBOTexture.h"
#include "RenderTexture.h"
#include "Workspace.h"

ImageSetExporter::ImageSetExporter(const char* a_path, ModelController* a_modelController, Workspace* a_workspace)
{   
    m_modelController = a_modelController;

    m_workspace = a_workspace;

    m_finished = false;
    m_init = false;

    const size_t len = strlen(a_path);

    char chr = a_path[len - 1];
    if (chr == '/' || chr == '\\')
    {
        m_path = new char[len + 1];
        strcpy(m_path, a_path);
    }
    else
    {
        m_path = new char[len + 2];
        strcpy(m_path, a_path);

        m_path[len] = '/';
        m_path[len + 1] = 0;
    }

    m_internalResolution = glm::vec2(1920, 1080);
    m_frameRate = 60;
}
ImageSetExporter::~ImageSetExporter()
{
    delete[] m_path;
}

bool ImageSetExporter::IsFinished() const
{
    return m_finished;
}

void ImageSetExporter::Update()
{
    bool open = true;

    if (!m_init)
    {
        ImGui::OpenPopup("Image Exporter");

        m_init = true;
    }

    if (ImGui::BeginPopupModal("Image Exporter", &open))
    {
        ImGui::InputInt("Target Framerate", &m_frameRate);
        m_frameRate = glm::max(m_frameRate, 1);

        ImGui::InputInt2("Internal Resolution", (int*)&m_internalResolution);
        // Have had issues in the past with lower res on render textures
        m_internalResolution.x = glm::max(m_internalResolution.x, 640);
        m_internalResolution.y = glm::max(m_internalResolution.y, 480);

        if (ImGui::Button("Export Images"))
        {
            double delta = 1.0 / m_frameRate;

            PBOTexture* tex = new PBOTexture(m_internalResolution.x, m_internalResolution.y, GL_RGBA);
            RenderTexture* renderTexture = new RenderTexture(tex);

            AnimControl* animControl = m_workspace->GetAnimControl();
            animControl->ResetTime();
            renderTexture->Bind();

            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

            m_modelController->DrawModel(m_workspace, delta);
            void* tmp = tex->MapPixels();
            if (tmp)
            {
                tex->UnmapPixels();
            }
            tex->Unbind();

            m_modelController->DrawModel(m_workspace, delta);
            tmp = tex->MapPixels();
            if (tmp)
            {
                tex->UnmapPixels();
            }
            tex->Unbind();

            for (unsigned int i = 0; i * delta <= animControl->GetMaxTime(); ++i)
            {
                animControl->Update(delta);

                m_modelController->DrawModel(m_workspace, delta);

                unsigned char* pixels = (unsigned char*)tex->MapPixels();
                if (pixels)
                {
                    const std::string fileName = std::string(m_path) + "Frame[" + std::to_string(i) + "].png";

                    // Why must file io be so slow 
                    stbi_write_png(fileName.c_str(), m_internalResolution.x, m_internalResolution.y, 4, pixels, 0);   
                    
                    tex->UnmapPixels();
                }
                tex->Unbind();
            }
        
            renderTexture->Unbind();

            delete renderTexture;

            m_finished = true;
        }

        ImGui::EndPopup();
    }
}