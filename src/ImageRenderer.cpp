#include "Components/ImageRenderer.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Camera.h"
#include "DataStore.h"
#include "FileUtils.h"
#include "imgui.h"
#include "Material.h"
#include "Models/Model.h"
#include "Object.h"
#include "PropertyFile.h"
#include "ShaderProgram.h"
#include "Shaders/ModelVertex.h"
#include "Shaders/StandardPixel.h"
#include "Texture.h"
#include "Transform.h"

const char* ImageRenderer::COMPONENT_NAME = "ImageRenderer";

ImageRenderer::ImageRenderer(Object* a_object) :
    Renderer(a_object)
{
    const int vertexS = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexS, 1, &MODELVERTEX, nullptr);
    glCompileShader(vertexS);

    const int pixelS = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(pixelS, 1, &STANDARDPIXEL, nullptr);
    glCompileShader(pixelS);

    m_shaderProgram = new ShaderProgram(pixelS, vertexS);

    glDeleteShader(vertexS);
    glDeleteShader(pixelS);
}
ImageRenderer::~ImageRenderer()
{
    delete m_shaderProgram;
}

void ImageRenderer::Draw(bool a_preview, Camera* a_camera)
{
    const DataStore* store = DataStore::GetInstance();

    const Object* object = GetObject();

    const char* modelName = GetModelName();

    const Model* model = store->GetModel(modelName, e_ModelType::Base);

    const char* textureName = store->GetModelTextureName(modelName);

    Texture* texture = nullptr;
    if (textureName != nullptr)
    {
        texture = store->GetTexture(textureName);
    }

    if (model != nullptr && texture != nullptr)
    {
        glBindVertexArray(model->GetVAO());
            
        const int handle = m_shaderProgram->GetHandle();

        glUseProgram(handle);

        const int texLocation = glGetUniformLocation(handle, "MainTex");
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture->GetHandle());
        glUniform1i(texLocation, 0);

        Transform* transform = object->GetTransform();

        const glm::vec3 scale = transform->Scale();

        const glm::mat4 transformMat = transform->GetWorldMatrix();
        const glm::mat4 shift = transformMat * glm::translate(glm::mat4(1), -GetAnchor());

        glm::mat4 view = glm::mat4(1);
        glm::mat4 proj = glm::orthoRH(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);

        if (a_camera != nullptr)
        {
            view = glm::inverse(a_camera->GetTransform()->ToMatrix());
            proj = a_camera->GetProjection();
        }

        const glm::mat4 finalTransform = view * proj * shift;

        const int location = glGetUniformLocation(handle, "model");
        glUniformMatrix4fv(location, 1, GL_FALSE, (float*)&finalTransform);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);        

        glDrawElements(GL_TRIANGLES, model->GetIndiciesCount(), GL_UNSIGNED_INT, 0);

        glDisable(GL_BLEND);
    }
}

void ImageRenderer::Update(double a_delta, Camera* a_camera)
{
    Draw(false, a_camera);
}
void ImageRenderer::UpdatePreview(double a_delta, Camera* a_camera)
{
    Draw(true, a_camera);
}
void ImageRenderer::UpdateGUI()
{
    UpdateRendererGUI();
}

const char* ImageRenderer::ComponentName() const
{
    return COMPONENT_NAME;
}

void ImageRenderer::Load(PropertyFileProperty* a_property)
{
    LoadValues(a_property);
}
void ImageRenderer::Save(PropertyFile* a_propertyFile, PropertyFileProperty* a_parent) const
{
    SaveValues(a_propertyFile, a_parent);
}