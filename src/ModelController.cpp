#include "ModelController.h"

#include <fstream>
#include <glad/glad.h>

#include "FileUtils.h"
#include "imgui.h"
#include "MemoryStream.h"
#include "Object.h"
#include "PropertyFile.h"
#include "SkeletonEditor.h"
#include "Texture.h"
#include "WebcamController.h"

ModelController::ModelController()
{
    m_backgroundColor = new float[3] { 0, 1, 0 };
}
ModelController::~ModelController()
{
    
}

void DrawObjects(Object* a_object, double a_delta)
{
    if (a_object != nullptr)
    {
        a_object->UpdateComponents(false, a_delta);

        std::list<Object*> children = a_object->GetChildren();

        for (auto iter = children.begin(); iter != children.end(); ++iter)
        {
            DrawObjects(*iter, a_delta);
        }
    }
}

void ModelController::DrawModel(const SkeletonEditor* a_skeletonEditor, double a_delta)
{
    glClearColor(m_backgroundColor[0], m_backgroundColor[1], m_backgroundColor[2], 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Object* baseObject = a_skeletonEditor->GetBaseObject();

    DrawObjects(baseObject, a_delta);
}
void ModelController::Update(const WebcamController& a_webcamController)
{
    ImGui::SetNextWindowSize({ 660, 400 }, ImGuiCond_Appearing);
    if (ImGui::Begin("Preview"))
    {
        const Texture* tex = a_webcamController.GetTexture();

        const ImVec2 size = ImGui::GetWindowSize();

        const glm::vec2 sScale = { size.x / 16, size.y / 9 };

        glm::vec2 fSize;

        if (sScale.x < sScale.y)
        {
            const float xSize = size.x - 20;

            fSize = { xSize, xSize * 0.5625f };
        }
        else
        {
            const float ySize = size.y - 40;

            fSize = { ySize * 1.77777777778f, ySize };
        }
        

        ImGui::Image((ImTextureID)tex->GetHandle(), { fSize.x, fSize.y });
    }
    ImGui::End();

    ImGui::SetNextWindowSize({ 200, 300 }, ImGuiCond_Appearing);
    if (ImGui::Begin("Options"))
    {
        ImGui::ColorPicker3("Background Color", m_backgroundColor);
    }
    ImGui::End();
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
                for (auto valIter = prop->Values().begin(); valIter != prop->Values().end(); ++valIter)
                {
                    IFSETTOATTVALF("r", valIter->Name, modelController->m_backgroundColor[0], valIter->Value)
                    else IFSETTOATTVALF("g", valIter->Name, modelController->m_backgroundColor[1], valIter->Value)
                    else IFSETTOATTVALF("b", valIter->Name, modelController->m_backgroundColor[2], valIter->Value)
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
    prop->EmplaceValue("r", std::to_string(m_backgroundColor[0]).c_str());
    prop->EmplaceValue("g", std::to_string(m_backgroundColor[1]).c_str());
    prop->EmplaceValue("b", std::to_string(m_backgroundColor[2]).c_str());

    char* data = propertyFile->ToString();

    mz_zip_writer_add_mem(&a_archive, "main.prop", data, strlen(data), MZ_DEFAULT_COMPRESSION);

    delete[] data;
    delete propertyFile;
}