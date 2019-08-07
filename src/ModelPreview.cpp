#include "ModelPreview.h"

#include <glad/glad.h>
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string.h>

#include "Material.h"
#include "Models/Model.h"
#include "RenderTexture.h"
#include "ShaderProgram.h"
#include "Texture.h"

#include "Shaders/ModelVertex.h"
#include "Shaders/StandardPixel.h"

unsigned int ModelPreview::SHADER_PROGRAM_REF = 0;
ShaderProgram* ModelPreview::STANDARD_SHADER_PROGRAM = nullptr;

ModelPreview::ModelPreview(Texture* a_texture, Model* a_model) :
    m_model(a_model)
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
    }
}

RenderTexture* ModelPreview::GetRenderTexture() const
{
    return m_renderTexture;
}

void ModelPreview::Render() const
{
    m_renderTexture->Bind();

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_material->Bind();
    const unsigned int handle = STANDARD_SHADER_PROGRAM->GetHandle();
    const int location = glGetUniformLocation(handle, "model");
    glm::mat4 iden = glm::mat4(2);
    iden[3] = { -1.0f, -1.0f, 0, 1 };
    glUniformMatrix4fv(location, 1, GL_FALSE, (float*)&iden);

    glBindVertexArray(m_model->GetVAO());
    glDrawElements(GL_TRIANGLES, m_model->GetIndicies(), GL_UNSIGNED_INT, 0);

    m_renderTexture->Unbind();
}