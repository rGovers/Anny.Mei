#include "Components/ImageRenderer.h"

#include <glad/glad.h>

#include "DataStore.h"
#include "imgui.h"
#include "Material.h"
#include "Models/Model.h"
#include "Object.h"
#include "ShaderProgram.h"
#include "Shaders/ModelVertex.h"
#include "Shaders/StandardPixel.h"
#include "Texture.h"
#include "Transform.h"

ImageRenderer::ImageRenderer(Object* a_object) :
    Renderer(a_object)
{
    m_modelName = new char[1] { 0 };

    const int vertexS = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexS, 1, &MODELVERTEX, nullptr);
    glCompileShader(vertexS);

    const int pixelS = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(pixelS, 1, &STANDARDPIXEL, nullptr);
    glCompileShader(pixelS);

    const ShaderProgram* program = new ShaderProgram(pixelS, vertexS);

    m_material = new Material(program);

    m_model = nullptr;
    m_texture = nullptr;

    m_anchor = glm::vec3(0.5f, 0.5f, 0.0f);

    glDeleteShader(vertexS);
    glDeleteShader(pixelS);
}
ImageRenderer::~ImageRenderer()
{
    delete m_material->GetShaderProgram();
    delete m_material;

    if (m_modelName != nullptr)
    {
        delete[] m_modelName;
    }
}

void ImageRenderer::Draw(bool a_preview)
{
    const Object* object = GetObject();

    if (m_model != nullptr && m_texture != nullptr)
    {
        m_material->Bind();

        glBindVertexArray(m_model->GetVAO());
            
        const int handle = m_material->GetShaderProgram()->GetHandle();

        Transform* transform = object->GetTransform();

        const glm::vec3 scale = transform->Scale();

        const glm::mat4 transformMat = transform->GetWorldMatrix();
        const glm::mat4 shift = transformMat * glm::translate(glm::mat4(1), -m_anchor);

        glm::mat4 orth;

        if (a_preview)
        {
            const ImVec2 size = ImGui::GetWindowSize();

            const glm::vec2 trueSize = { (size.x - 20) / 2048, (size.y - 40) / 2048 };
            const glm::vec2 halfSize = trueSize * 0.5f;

            orth = glm::orthoRH(0.0f, trueSize.x, 0.0f, trueSize.y, -1.0f, 1.0f);
        }
        else
        {
            orth = glm::orthoRH(0.0f, 1.0f, 0.0f, 0.5625f, -1.0f, 1.0f);
        }

        const glm::mat4 finalTransform = orth * shift;

        const int location = glGetUniformLocation(handle, "model");
        glUniformMatrix4fv(location, 1, GL_FALSE, (float*)&finalTransform);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);        

        glDrawElements(GL_TRIANGLES, m_model->GetIndiciesCount(), GL_UNSIGNED_INT, 0);

        glDisable(GL_BLEND);
    }
}

void ImageRenderer::Update(double a_delta)
{
    Draw(false);
}
void ImageRenderer::UpdatePreview(double a_delta)
{
    Draw(true);
}
void ImageRenderer::UpdateGUI()
{
    const size_t len = strlen(m_modelName) + 1;

    char* buff = new char[len] { 0 };
    memcpy(buff, m_modelName, len);

    ImGui::InputText("Model Name", buff, BUFFER_SIZE);
    ImGui::InputFloat3("Anchor Position", (float*)&m_anchor);

    if (strcmp(buff, m_modelName) != 0)
    {
        if (m_modelName != nullptr && strlen(m_modelName) > 0)
        {
            m_material->RemoveTexture(m_modelName);
        }

        const DataStore* store = DataStore::GetInstance();

        const char* texName = store->GetModelTextureName(buff);

        m_model = store->GetModel(buff, e_ModelType::Base);

        if (texName != nullptr)
        {
            m_texture = store->GetTexture(texName);

            if (m_texture != nullptr)
            {
                m_material->AddTexture("MainTex", m_texture);
            }
        }

        delete[] m_modelName;
        m_modelName = buff;
    }
    else
    {
        delete[] buff;
    }
}

const char* ImageRenderer::ComponentName() const
{
    return "ImageRenderer";
}