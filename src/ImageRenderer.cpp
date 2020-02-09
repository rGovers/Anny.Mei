#include "Components/ImageRenderer.h"

#include <glad/glad.h>

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

void ImageRenderer::Draw(bool a_preview, Camera* a_camera)
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

        glDrawElements(GL_TRIANGLES, m_model->GetIndiciesCount(), GL_UNSIGNED_INT, 0);

        glDisable(GL_BLEND);
    }
    else 
    {
        const DataStore* store = DataStore::GetInstance();

        if (m_model == nullptr)
        {
            m_model = store->GetModel(m_modelName, e_ModelType::Base);
        }

        if (m_texture == nullptr)
        {
            const char* texName = store->GetModelTextureName(m_modelName);

            if (texName != nullptr)
            {
                m_texture = store->GetTexture(texName);

                if (m_texture != nullptr)
                {
                    m_material->AddTexture("MainTex", m_texture);
                }
            }
        }
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

        m_model = nullptr;
        m_texture = nullptr;

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
    return COMPONENT_NAME;
}

void ImageRenderer::Load(PropertyFileProperty* a_property)
{
    const std::list<PropertyFileValue> values = a_property->Values();

    for (auto iter = values.begin(); iter != values.end(); ++iter)
    {
        IFSETTOATTVALCPY(iter->Name, "modelName", m_modelName, iter->Value)
        else IFSETTOATTVALV3(iter->Name, "anchor", m_anchor, iter->Value)
    }
}
void ImageRenderer::Save(PropertyFile* a_propertyFile, PropertyFileProperty* a_parent) const
{
    PropertyFileProperty* property = a_propertyFile->InsertProperty();
    property->SetParent(a_parent);
    property->SetName(ComponentName());

    property->EmplaceValue("modelName", m_modelName);

    const std::string anchorStr = "{ " + std::to_string(m_anchor.x) + ", " + std::to_string(m_anchor.y) + ", " + std::to_string(m_anchor.z) + " }";
    property->EmplaceValue("anchor", anchorStr.c_str());
}