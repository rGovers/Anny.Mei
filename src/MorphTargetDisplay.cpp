#include "Renderers/MorphTargetDisplay.h"

#include <glad/glad.h>
#include <string.h>

#include "DataStore.h"
#include "DepthRenderTexture.h"
#include "Models/Model.h"
#include "ShaderProgram.h"
#include "Shaders/MaskedPixel.h"
#include "Shaders/MorphTargetVertex.h"
#include "Shaders/MorphTarget9PointVertex.h"
#include "Shaders/SolidPixel.h"
#include "Shaders/StandardPixel.h"
#include "Texture.h"

unsigned int MorphTargetDisplay::Ref = 0;
ShaderProgram* MorphTargetDisplay::BaseShaderProgram = nullptr;
ShaderProgram* MorphTargetDisplay::MaskShaderProgram = nullptr;
ShaderProgram* MorphTargetDisplay::WireShaderProgram = nullptr;

ShaderProgram* MorphTargetDisplay::Point9ShaderProgram = nullptr;
ShaderProgram* MorphTargetDisplay::Point9MaskShaderProgram = nullptr;
ShaderProgram* MorphTargetDisplay::Point9WireShaderProgram = nullptr;

MorphTargetDisplay::MorphTargetDisplay()
{
    m_modelName = nullptr;

	const char* str;

    if (BaseShaderProgram == nullptr)
    {
        const unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		str = MORPHTARGETVERTEX;
        glShaderSource(vertexShader, 1, &str, 0);
        glCompileShader(vertexShader);

		str = STANDARDPIXEL;
        const unsigned int pixelShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(pixelShader, 1, &str, 0);
        glCompileShader(pixelShader);

        BaseShaderProgram = new ShaderProgram(pixelShader, vertexShader);

        glDeleteShader(vertexShader);
        glDeleteShader(pixelShader);
    }

    if (MaskShaderProgram == nullptr)
    {	
        const unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		str = MORPHTARGETVERTEX;
        glShaderSource(vertexShader, 1, &str, 0);
        glCompileShader(vertexShader);

		str = MASKEDPIXEL;
        const unsigned int maskPixelShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(maskPixelShader, 1, &str, 0);
        glCompileShader(maskPixelShader);

        MaskShaderProgram = new ShaderProgram(maskPixelShader, vertexShader);

        glDeleteShader(vertexShader);
        glDeleteShader(maskPixelShader);
    }

    if (WireShaderProgram == nullptr)
    {
        const unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		str = MORPHTARGETVERTEX;
        glShaderSource(vertexShader, 1, &str, 0);
        glCompileShader(vertexShader);

        const unsigned int solidPixelShader = glCreateShader(GL_FRAGMENT_SHADER);
		str = SOLIDPIXEL;
        glShaderSource(solidPixelShader, 1, &str, 0);
        glCompileShader(solidPixelShader);

        WireShaderProgram = new ShaderProgram(solidPixelShader, vertexShader);

        glDeleteShader(vertexShader);
        glDeleteShader(solidPixelShader);
    }
    
    if (Point9ShaderProgram == nullptr)
    {
        const unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		str = MORPHTARGET9POINTVERTEX;
		glShaderSource(vertexShader, 1, &str, 0);
        glCompileShader(vertexShader);

        const unsigned int pixelShader = glCreateShader(GL_FRAGMENT_SHADER);
		str = STANDARDPIXEL;
        glShaderSource(pixelShader, 1, &str, 0);
        glCompileShader(pixelShader);

        Point9ShaderProgram = new ShaderProgram(pixelShader, vertexShader);

        glDeleteShader(vertexShader);
        glDeleteShader(pixelShader);
    }

    if (Point9MaskShaderProgram == nullptr)
    {
        const unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		str = MORPHTARGET9POINTVERTEX;
        glShaderSource(vertexShader, 1, &str, 0);
        glCompileShader(vertexShader);

        const unsigned int maskedPixelShader = glCreateShader(GL_FRAGMENT_SHADER);
		str = MASKEDPIXEL;
        glShaderSource(maskedPixelShader, 1, &str, 0);
        glCompileShader(maskedPixelShader);

        Point9MaskShaderProgram = new ShaderProgram(maskedPixelShader, vertexShader);

        glDeleteShader(vertexShader);
        glDeleteShader(maskedPixelShader);
    }

    if (Point9WireShaderProgram == nullptr)
    {
        const unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		str = MORPHTARGET9POINTVERTEX;
        glShaderSource(vertexShader, 1, &str, 0);
        glCompileShader(vertexShader);

        const unsigned int solidPixelShader = glCreateShader(GL_FRAGMENT_SHADER);
		str = SOLIDPIXEL;
        glShaderSource(solidPixelShader, 1, &str, 0);
        glCompileShader(solidPixelShader);

        Point9WireShaderProgram = new ShaderProgram(solidPixelShader, vertexShader);

        glDeleteShader(vertexShader);
        glDeleteShader(solidPixelShader);
    }

    ++Ref;
}
MorphTargetDisplay::~MorphTargetDisplay()
{
    if (--Ref <= 0)
    {
        delete WireShaderProgram;
        WireShaderProgram = nullptr;

        delete MaskShaderProgram;
        MaskShaderProgram = nullptr;

        delete BaseShaderProgram;
        BaseShaderProgram = nullptr;

        delete Point9ShaderProgram;
        Point9ShaderProgram = nullptr;

        delete Point9MaskShaderProgram;
        Point9MaskShaderProgram = nullptr;

        delete Point9WireShaderProgram;
        Point9WireShaderProgram = nullptr;
    }

    if (m_modelName != nullptr)
    {
        delete[] m_modelName;
    }
}

const char* MorphTargetDisplay::GetModelName() const
{
    return m_modelName;
}
void MorphTargetDisplay::SetModelName(const char* a_name)
{
    if (m_modelName != nullptr)
    {
        delete[] m_modelName;
        m_modelName = nullptr;
    }

    if (a_name != nullptr)
    {
        const size_t len = strlen(a_name);

        m_modelName = new char[len + 1];

        strcpy(m_modelName, a_name);
    }
}

void MorphTargetDisplay::Draw(const glm::mat4& a_transform, const glm::vec2& a_lerp, bool a_alpha, bool a_solid, bool a_wireframe) const
{
    DataStore* store = DataStore::GetInstance();

    Model* model = nullptr;
    Texture* tex = nullptr;
    if (m_modelName != nullptr)
    {
        model = store->GetModel(m_modelName, e_ModelType::MorphTarget);
        
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
        }

        glBindVertexArray(model->GetVAO());

        const unsigned int indexCount = model->GetIndicesCount();

        if (a_wireframe)
        {
            const int wireHandle = WireShaderProgram->GetHandle();
            glUseProgram(wireHandle);

            const int modelLocation = glGetUniformLocation(wireHandle, "Model");
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, (float*)&a_transform);

            const int lerpLocation = glGetUniformLocation(wireHandle, "Lerp");
            glUniform2fv(lerpLocation, 1, (float*)&a_lerp);

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

            const int lerpLocation = glGetUniformLocation(baseHandle, "Lerp");
            glUniform2fv(lerpLocation, 1, (float*)&a_lerp);

            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        }

        glDisable(GL_BLEND);
    }
}
void MorphTargetDisplay::DrawMasked(const glm::mat4& a_transform, const DepthRenderTexture* a_mask, const glm::vec2& a_lerp) const
{
    DataStore* store = DataStore::GetInstance();

    Model* model = nullptr;
    Texture* tex = nullptr;
    if (m_modelName != nullptr)
    {
        model = store->GetModel(m_modelName, e_ModelType::MorphTarget);
        
        const char* texName = store->GetModelTextureName(m_modelName);

        if (texName != nullptr)
        {
            tex = store->GetTexture(texName);
        }
    }

    if (model != nullptr && tex != nullptr && a_mask != nullptr)
    {
        glEnable(GL_BLEND);   

        glBindVertexArray(model->GetVAO());

        glm::vec4 view;

        glGetFloatv(GL_VIEWPORT, (float*)&view);

        const unsigned int indexCount = model->GetIndicesCount();

        const int baseHandle = MaskShaderProgram->GetHandle();
        glUseProgram(baseHandle);

        const int modelLocation = glGetUniformLocation(baseHandle, "Model");
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, (float*)&a_transform);

        const int location = glGetUniformLocation(baseHandle, "MainTex");
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex->GetHandle());
        glUniform1i(location, 0);

        const int maskTexLocation = glGetUniformLocation(baseHandle, "MaskTex");
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, a_mask->GetDepthTexture()->GetHandle());
        glUniform1i(maskTexLocation, 1);

        const glm::vec2 screenSize = glm::vec2(view.z, view.w);
        const int screenSizeLocation = glGetUniformLocation(baseHandle, "ScreenSize");
        glUniform2fv(screenSizeLocation, 1, (float*)&screenSize);

        const int lerpLocation = glGetUniformLocation(baseHandle, "Lerp");
        glUniform2fv(lerpLocation, 1, (float*)&a_lerp);

        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

        glDisable(GL_BLEND);
    }
}

void MorphTargetDisplay::Draw9Point(const glm::mat4& a_transform, const glm::vec2& a_lerp, bool a_alpha, bool a_solid, bool a_wireframe) const
{
    DataStore* store = DataStore::GetInstance();

    Model* model = nullptr;
    Texture* tex = nullptr;
    if (m_modelName != nullptr)
    {
        model = store->GetModel(m_modelName, e_ModelType::MorphTarget);
        
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
        }

        glBindVertexArray(model->GetVAO());

        const unsigned int indexCount = model->GetIndicesCount();

        if (a_wireframe)
        {
            const int wireHandle = Point9WireShaderProgram->GetHandle();
            glUseProgram(wireHandle);

            const int modelLocation = glGetUniformLocation(wireHandle, "Model");
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, (float*)&a_transform);

            const int lerpLocation = glGetUniformLocation(wireHandle, "Lerp");
            glUniform2fv(lerpLocation, 1, (float*)&a_lerp);

            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        if (a_solid)
        {
            const int baseHandle = Point9ShaderProgram->GetHandle();
            glUseProgram(baseHandle);

            const int modelLocation = glGetUniformLocation(baseHandle, "Model");
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, (float*)&a_transform);

            const int location = glGetUniformLocation(baseHandle, "MainTex");
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, tex->GetHandle());
            glUniform1i(location, 0);

            const int lerpLocation = glGetUniformLocation(baseHandle, "Lerp");
            glUniform2fv(lerpLocation, 1, (float*)&a_lerp);

            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        }

        glDisable(GL_BLEND);
    }
}
void MorphTargetDisplay::Draw9PointMasked(const glm::mat4& a_transform, const DepthRenderTexture* a_mask, const glm::vec2& a_lerp) const
{
    DataStore* store = DataStore::GetInstance();

    Model* model = nullptr;
    Texture* tex = nullptr;
    if (m_modelName != nullptr)
    {
        model = store->GetModel(m_modelName, e_ModelType::MorphTarget);
        
        const char* texName = store->GetModelTextureName(m_modelName);

        if (texName != nullptr)
        {
            tex = store->GetTexture(texName);
        }
    }

    if (model != nullptr && tex != nullptr && a_mask != nullptr)
    {
        glEnable(GL_BLEND);

        glBindVertexArray(model->GetVAO());

        glm::vec4 view;

        glGetFloatv(GL_VIEWPORT, (float*)&view);

        const unsigned int indexCount = model->GetIndicesCount();

        const int baseHandle = Point9MaskShaderProgram->GetHandle();
        glUseProgram(baseHandle);

        const int modelLocation = glGetUniformLocation(baseHandle, "Model");
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, (float*)&a_transform);

        const int location = glGetUniformLocation(baseHandle, "MainTex");
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex->GetHandle());
        glUniform1i(location, 0);

        const int maskTexLocation = glGetUniformLocation(baseHandle, "MaskTex");
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, a_mask->GetDepthTexture()->GetHandle());
        glUniform1i(maskTexLocation, 1);

        const glm::vec2 screenSize = glm::vec2(view.z, view.w);
        const int screenSizeLocation = glGetUniformLocation(baseHandle, "ScreenSize");
        glUniform2fv(screenSizeLocation, 1, (float*)&screenSize);

        const int lerpLocation = glGetUniformLocation(baseHandle, "Lerp");
        glUniform2fv(lerpLocation, 1, (float*)&a_lerp);

        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

        glDisable(GL_BLEND);
    }
}