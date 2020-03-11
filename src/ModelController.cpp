#include "ModelController.h"

#include <fstream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include "FileLoaders/PropertyFile.h"
#include "FileUtils.h"
#include "MemoryStream.h"
#include "Object.h"
#include "Texture.h"
#include "WebcamController.h"
#include "WindowControls/ModelControllerWindow.h"
#include "Workspace.h"

ModelController::ModelController()
{
    m_window = new ModelControllerWindow(this);

    m_camera = new Camera();

    const glm::mat4 proj = glm::orthoRH(0.0f, 1.0f, 0.0f, 0.5625f, -1.0f, 1.0f);

    m_camera->SetProjection(proj);
}
ModelController::~ModelController()
{
    delete m_camera;

    delete m_window;
}

void DrawObjects(Object* a_object, Camera* a_camera, double a_delta)
{
    if (a_object != nullptr)
    {
        a_object->UpdateComponents(false, a_camera, a_delta);

        std::list<Object*> children = a_object->GetChildren();

        for (auto iter = children.begin(); iter != children.end(); ++iter)
        {
            DrawObjects(*iter, a_camera, a_delta);
        }
    }
}

void ModelController::DrawModel(const Workspace* a_workspace, double a_delta)
{
    const glm::vec3 backColor = m_window->GetBackgroundColor();

    glClearColor(backColor.x, backColor.y, backColor.z, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Object* baseObject = a_workspace->GetBaseObject();

    DrawObjects(baseObject, m_camera, a_delta);
}
void ModelController::Update(const WebcamController& a_webcamController)
{
    m_texture = a_webcamController.GetTexture();

    m_window->Update();
}

ModelController* ModelController::Load(mz_zip_archive& a_archive)
{
    ModelController* modelController = new ModelController();

    char* propertiesData = ExtractFileFromArchive("main.prop", a_archive);

    if (propertiesData != nullptr)
    {
        PropertyFile* propertiesFile = new PropertyFile(propertiesData);

        const std::list<PropertyFileProperty*> properties = propertiesFile->GetProperties();

        for (auto iter = properties.begin(); iter != properties.end(); ++iter)
        {
            PropertyFileProperty* prop = *iter;

            const char* name = prop->GetName();

            if (strcmp("backcolor", name) == 0)
            {
                glm::vec3 backgroundColor = glm::vec3(std::numeric_limits<float>::infinity());

                for (auto valIter = prop->Values().begin(); valIter != prop->Values().end(); ++valIter)
                {
                    IFSETTOATTVALF("r", valIter->Name, backgroundColor.x, valIter->Value)
                    else IFSETTOATTVALF("g", valIter->Name, backgroundColor.y, valIter->Value)
                    else IFSETTOATTVALF("b", valIter->Name, backgroundColor.z, valIter->Value)
                }

                if (backgroundColor.x != std::numeric_limits<float>::infinity() && backgroundColor.y != std::numeric_limits<float>::infinity() && backgroundColor.z != std::numeric_limits<float>::infinity())
                {
                    modelController->m_window->SetBackgroundColor(backgroundColor);
                }
            }
        }

        delete propertiesFile;
        mz_free(propertiesData);

        return modelController;
    }

    return nullptr;
}
void ModelController::Save(mz_zip_archive& a_archive) const
{
    PropertyFile* propertyFile = new PropertyFile();

    PropertyFileProperty* prop = propertyFile->InsertProperty();

    prop->SetName("backcolor");
    const glm::vec3 backgroundColor = m_window->GetBackgroundColor();

    prop->EmplaceValue("r", std::to_string(backgroundColor.x).c_str());
    prop->EmplaceValue("g", std::to_string(backgroundColor.y).c_str());
    prop->EmplaceValue("b", std::to_string(backgroundColor.z).c_str());

    char* data = propertyFile->ToString();

    mz_zip_writer_add_mem(&a_archive, "main.prop", data, strlen(data), MZ_DEFAULT_COMPRESSION);

    delete[] data;
    delete propertyFile;
}

Texture* ModelController::GetPreviewTexture() const
{
    return m_texture;
}