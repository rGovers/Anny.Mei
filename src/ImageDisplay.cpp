#include "Renderers/ImageDisplay.h"

#include <glad/glad.h>
#include <string.h>

#include "DataStore.h"
#include "DepthRenderTexture.h"
#include "Models/Model.h"
#include "ShaderProgram.h"
#include "Shaders/MaskedPixel.h"
#include "Shaders/ModelVertex.h"
#include "Shaders/SolidPixel.h"
#include "Shaders/StandardPixel.h"
#include "Texture.h"

unsigned int ImageDisplay::Ref = 0;
ShaderProgram* ImageDisplay::BaseShaderProgram = nullptr;
ShaderProgram* ImageDisplay::MaskShaderProgram = nullptr;
ShaderProgram* ImageDisplay::WireShaderProgram = nullptr;

ImageDisplay::ImageDisplay()
{
    m_modelName = nullptr;

    if (BaseShaderProgram == nullptr)
    {
        const unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &MODELVERTEX, 0);
        glCompileShader(vertexShader);

        const unsigned int pixelShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(pixelShader, 1, &STANDARDPIXEL, 0);
        glCompileShader(pixelShader);

        BaseShaderProgram = new ShaderProgram(pixelShader, vertexShader);

        glDeleteShader(vertexShader);
        glDeleteShader(pixelShader);
    }
    
    if (MaskShaderProgram == nullptr)
    {
        const unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &MODELVERTEX, 0);
        glCompileShader(vertexShader);

        const unsigned int maskPixelShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(maskPixelShader, 1, &MASKEDPIXEL, 0);
        glCompileShader(maskPixelShader);

        MaskShaderProgram = new ShaderProgram(maskPixelShader, vertexShader);

        glDeleteShader(vertexShader);
        glDeleteShader(maskPixelShader);
    }

    if (WireShaderProgram == nullptr)
    {
        const unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &MODELVERTEX, 0);
        glCompileShader(vertexShader);

        const unsigned int solidPixelShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(solidPixelShader, 1, &SOLIDPIXEL, 0);
        glCompileShader(solidPixelShader);

        WireShaderProgram = new ShaderProgram(solidPixelShader, vertexShader);

        glDeleteShader(vertexShader);
        glDeleteShader(solidPixelShader);
    }
    
    ++Ref;
}
ImageDisplay::~ImageDisplay()
{
    if (m_modelName != nullptr)
    {
        delete[] m_modelName;
    }

    if (--Ref <= 0)
    {
        delete WireShaderProgram;
        WireShaderProgram = nullptr;

        delete MaskShaderProgram;
        MaskShaderProgram = nullptr;

        delete BaseShaderProgram;
        BaseShaderProgram = nullptr;
    }
}

const char* ImageDisplay::GetModelName() const
{
    return m_modelName;
}

void ImageDisplay::SetModelName(const char* a_name)
{
    if (m_modelName != nullptr)
    {
        delete[] m_modelName;
        m_modelName = nullptr;
    }

    if (a_name != nullptr)
    {
        const size_t len = strlen(a_name);

        if (len > 0)
        {
            m_modelName = new char[len + 1];

            strcpy(m_modelName, a_name);
        }
    }
}

void ImageDisplay::Draw(const glm::mat4& a_transform, bool a_alpha, bool a_solid, bool a_wireframe) const
{
    DataStore* store = DataStore::GetInstance();

    Model* model = nullptr;
    Texture* tex = nullptr;
    if (m_modelName != nullptr)
    {
        model = store->GetModel(m_modelName, e_ModelType::Base);
        
        const char* texName = store->GetModelTextureName(m_modelName);

        if (texName != nullptr)
        {
            tex = store->GetTexture(texName);
        }
    }

    if (model != nullptr && tex != nullptr)
    {
        if (a_alpha)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);     
        }

        glBindVertexArray(model->GetVAO());

        const unsigned int indexCount = model->GetIndicesCount();

        if (a_wireframe)
        {
            const int wireHandle = WireShaderProgram->GetHandle();
            glUseProgram(wireHandle);

            const int modelLocation = glGetUniformLocation(wireHandle, "Model");
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, (float*)&a_transform);

            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        if (a_solid)
        {
            const int baseHandle = BaseShaderProgram->GetHandle();
            glUseProgram(baseHandle);

            const int modelLocation = glGetUniformLocation(baseHandle, "Model");
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, (float*)&a_transform);

            const int location = glGetUniformLocation(baseHandle, "MainTex");
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, tex->GetHandle());
            glUniform1i(location, 0);

            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        }

        glDisable(GL_BLEND);
    }
}
void ImageDisplay::DrawMasked(const glm::mat4& a_transform, const DepthRenderTexture* a_mask) const
{
    DataStore* store = DataStore::GetInstance();

    Model* model = nullptr;
    Texture* tex = nullptr;
    if (m_modelName != nullptr)
    {
        model = store->GetModel(m_modelName, e_ModelType::Base);
        
        const char* texName = store->GetModelTextureName(m_modelName);

        if (texName != nullptr)
        {
            tex = store->GetTexture(texName);
        }
    }

    if (model != nullptr && tex != nullptr && a_mask != nullptr)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

        glBindVertexArray(model->GetVAO());

        glm::vec4 view;

        glGetFloatv(GL_VIEWPORT, (float*)&view);

        const unsigned int indexCount = model->GetIndicesCount();

        const int baseHandle = MaskShaderProgram->GetHandle();
        glUseProgram(baseHandle);

        const int modelLocation = glGetUniformLocation(baseHandle, "Model");
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, (float*)&a_transform);

        const int mainTexLocation = glGetUniformLocation(baseHandle, "MainTex");
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex->GetHandle());
        glUniform1i(mainTexLocation, 0);

        const int maskTexLocation = glGetUniformLocation(baseHandle, "MaskTex");
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, a_mask->GetDepthTexture()->GetHandle());
        glUniform1i(maskTexLocation, 1);

        const glm::vec2 screenSize = glm::vec2(view.z, view.w);
        const int screenSizeLocation = glGetUniformLocation(baseHandle, "ScreenSize");
        glUniform2fv(screenSizeLocation, 1, (float*)&screenSize);

        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

        glDisable(GL_BLEND);
    }
}