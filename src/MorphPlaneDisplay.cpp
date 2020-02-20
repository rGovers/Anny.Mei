#include "Renderers/MorphPlaneDisplay.h"

#include <glad/glad.h>

#include "DataStore.h"
#include "Models/MorphPlaneModel.h"
#include "MorphPlane.h"
#include "ShaderProgram.h"
#include "Shaders/MorphPlaneVertex.h"
#include "Shaders/SolidPixel.h"
#include "Shaders/StandardPixel.h"
#include "Texture.h"

unsigned int MorphPlaneDisplay::Ref = 0;
ShaderProgram* MorphPlaneDisplay::BaseShaderProgram = nullptr;
ShaderProgram* MorphPlaneDisplay::WireShaderProgram = nullptr;

MorphPlaneDisplay::MorphPlaneDisplay()
{
    m_modelName = nullptr;
    m_morphPlaneName = nullptr;

    if (BaseShaderProgram == nullptr)
    {
        const unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &MORPHPLANEVERTEX, 0);
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
        glShaderSource(vertexShader, 1, &MORPHPLANEVERTEX, 0);
        glCompileShader(vertexShader);

        const unsigned int solidPixelShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(solidPixelShader, 1, &SOLIDPIXEL, 0);
        glCompileShader(solidPixelShader);

        WireShaderProgram = new ShaderProgram(solidPixelShader, vertexShader);

        glDeleteShader(solidPixelShader);
    }
    
    ++Ref;
}

MorphPlaneDisplay::~MorphPlaneDisplay()
{
    if (m_modelName != nullptr)
    {
        delete[] m_modelName;
    }
    if (m_morphPlaneName != nullptr)
    {
        delete[] m_morphPlaneName;
    }
}

const char* MorphPlaneDisplay::GetModelName() const
{
    return m_modelName;
}

void MorphPlaneDisplay::SetModelName(const char* a_name)
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

const char* MorphPlaneDisplay::GetMorphPlaneName() const
{
    return m_morphPlaneName;
}

void MorphPlaneDisplay::SetMorphPlaneName(const char* a_name)
{
    if (m_morphPlaneName != nullptr)
    {
        delete[] m_morphPlaneName;
        m_morphPlaneName = nullptr;
    }

    if (a_name != nullptr)
    {
        const size_t len = strlen(a_name);

        if (len > 0)
        {
            m_morphPlaneName = new char[len + 1];

            strcpy(m_morphPlaneName, a_name);
        }
    }
}

void MorphPlaneDisplay::Draw(const glm::mat4& a_transform, bool a_alpha, bool a_solid, bool a_wireframe) const
{
    DataStore* store = DataStore::GetInstance();

    Model* model = nullptr;
    Texture* tex = nullptr;
    MorphPlane* morphPlane = nullptr;
    if (m_modelName != nullptr)
    {
        model = store->GetModel(m_modelName, e_ModelType::MorphPlane);
        
        const char* texName = store->GetModelTextureName(m_modelName);
        if (texName != nullptr)
        {
            tex = store->GetTexture(texName);

            morphPlane = store->GetMorphPlane(m_morphPlaneName);
        }
    }

    if (model != nullptr && tex != nullptr && morphPlane != nullptr)
    {
        const Texture* morphTex = morphPlane->ToTexture();

        if (a_alpha)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);     
        }

        glBindVertexArray(model->GetVAO());

        const unsigned int indexCount = model->GetIndicesCount();

        const unsigned int dim = morphPlane->GetSize();
        const unsigned int sSize = dim * dim;

        if (a_wireframe)
        {
            const int wireHandle = WireShaderProgram->GetHandle();
            glUseProgram(wireHandle);

            const int modelLocation = glGetUniformLocation(wireHandle, "Model");
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, (float*)&a_transform);

            const int morphTexLocation = glGetUniformLocation(wireHandle, "MorphTex");
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, morphTex->GetHandle());
            glUniform1i(morphTexLocation, 0);

            const int morphTexSizeLocation = glGetUniformLocation(wireHandle, "MorphSize");
            glUniform1ui(morphTexSizeLocation, sSize);

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

            const int morphTexLocation = glGetUniformLocation(baseHandle, "MorphTex");
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, morphTex->GetHandle());
            glUniform1i(morphTexLocation, 1);

            const int morphTexSizeLocation = glGetUniformLocation(baseHandle, "MorphSize");
            glUniform1ui(morphTexSizeLocation, sSize);

            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        }

        glDisable(GL_BLEND);

        delete morphTex;
    }
}