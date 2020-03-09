#include "Renderers/MorphTargetDisplay.h"

#include <glad/glad.h>
#include <string.h>

#include "DataStore.h"
#include "Models/Model.h"
#include "ShaderProgram.h"
#include "Shaders/MorphTargetVertex.h"
#include "Shaders/SolidPixel.h"
#include "Shaders/StandardPixel.h"
#include "Texture.h"

unsigned int MorphTargetDisplay::Ref = 0;
ShaderProgram* MorphTargetDisplay::BaseShaderProgram = nullptr;
ShaderProgram* MorphTargetDisplay::WireShaderProgram = nullptr;

MorphTargetDisplay::MorphTargetDisplay()
{
    m_modelName = nullptr;

    if (BaseShaderProgram == nullptr)
    {
        const unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &MORPHTARGETVERTEX, 0);
        glCompileShader(vertexShader);

        const unsigned int pixelShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(pixelShader, 1, &STANDARDPIXEL, 0);
        glCompileShader(pixelShader);

        BaseShaderProgram = new ShaderProgram(pixelShader, vertexShader);

        glDeleteShader(vertexShader);
        glDeleteShader(pixelShader);
    }
    
    if (WireShaderProgram == nullptr)
    {
        const unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &MORPHTARGETVERTEX, 0);
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
MorphTargetDisplay::~MorphTargetDisplay()
{
    if (--Ref <= 0)
    {
        delete WireShaderProgram;
        WireShaderProgram = nullptr;

        delete BaseShaderProgram;
        BaseShaderProgram = nullptr;
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