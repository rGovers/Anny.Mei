#include "ModelPreview.h"

#include <glad/glad.h>
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string.h>

#include "imgui.h"
#include "Material.h"
#include "Models/Model.h"
#include "RenderTexture.h"
#include "ShaderProgram.h"
#include "Texture.h"

#include "Shaders/ModelVertex.h"
#include "Shaders/SolidPixel.h"
#include "Shaders/StandardPixel.h"

unsigned int ModelPreview::SHADER_PROGRAM_REF = 0;
ShaderProgram* ModelPreview::STANDARD_SHADER_PROGRAM = nullptr;
ShaderProgram* ModelPreview::WIREFRAME_SHADER_PROGRAM = nullptr;

ModelPreview::ModelPreview(Texture* a_texture, Model* a_model) :
    m_model(a_model),
    m_solid(true),
    m_wireframe(false)
{
    m_renderTexture = new RenderTexture(a_texture->GetWidth(), a_texture->GetHeight(), GL_RGBA);

    ++SHADER_PROGRAM_REF;

    if (STANDARD_SHADER_PROGRAM == nullptr)
    {
        unsigned int pixelShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(pixelShader, 1, &STANDARDPIXEL, 0);
        glCompileShader(pixelShader);
        
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &MODELVERTEX, 0);
        glCompileShader(vertexShader);

        STANDARD_SHADER_PROGRAM = new ShaderProgram(pixelShader, vertexShader);

        glDeleteShader(pixelShader);
        glDeleteShader(vertexShader);
    }
    
    if (WIREFRAME_SHADER_PROGRAM == nullptr)
    {
        unsigned int pixelShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(pixelShader, 1, &SOLIDPIXEL, 0);
        glCompileShader(pixelShader);

        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &MODELVERTEX, 0);
        glCompileShader(vertexShader);

        WIREFRAME_SHADER_PROGRAM = new ShaderProgram(pixelShader, vertexShader);

        glDeleteShader(pixelShader);
        glDeleteShader(vertexShader);
    }

    m_material = new Material(STANDARD_SHADER_PROGRAM);
    m_material->AddTexture("MainTex", a_texture);
}

ModelPreview::~ModelPreview()
{
    delete m_material;
    delete m_renderTexture;
    delete m_model;

    --SHADER_PROGRAM_REF;

    if (SHADER_PROGRAM_REF == 0)
    {
        delete STANDARD_SHADER_PROGRAM;
        STANDARD_SHADER_PROGRAM = nullptr;

        delete WIREFRAME_SHADER_PROGRAM;
        WIREFRAME_SHADER_PROGRAM = nullptr;
    }
}

RenderTexture* ModelPreview::GetRenderTexture() const
{
    return m_renderTexture;
}

Model* ModelPreview::GetModel() const
{
    return m_model;
}

void ModelPreview::Update()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("View"))
        {
            ImGui::MenuItem("Solid", nullptr, &m_solid);
            ImGui::MenuItem("Wireframe", nullptr, &m_wireframe);

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
}

void ModelPreview::Render() const
{
    m_renderTexture->Bind();

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 transform = glm::mat4(2);
    transform[3] = { -1.0f, -1.0f, 0, 1 };

    glDisable(GL_DEPTH_TEST);

    glBindVertexArray(m_model->GetVAO());
    
    if (m_solid)
    {
        m_material->Bind();
        const unsigned int handle = STANDARD_SHADER_PROGRAM->GetHandle();
        
        const int location = glGetUniformLocation(handle, "model");
        glUniformMatrix4fv(location, 1, GL_FALSE, (float*)&transform);

        glDrawElements(GL_TRIANGLES, m_model->GetIndicies(), GL_UNSIGNED_INT, 0);
    }

    if (m_wireframe)   
    {
        const unsigned int handle = WIREFRAME_SHADER_PROGRAM->GetHandle();

        glUseProgram(handle);

        const int location = glGetUniformLocation(handle, "model");
        glUniformMatrix4fv(location, 1, GL_FALSE, (float*)&transform);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLES, m_model->GetIndicies(), GL_UNSIGNED_INT, 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glEnable(GL_DEPTH_TEST);

    m_renderTexture->Unbind();
}