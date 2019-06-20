#include "ModelController.h"

#include <fstream>
#include <glad/glad.h>

#include "FileUtils.h"
#include "imgui.h"
#include "MemoryStream.h"
#include "PropertyFile.h"
#include "Texture.h"
#include "WebcamController.h"

ModelController::ModelController()
{
    m_backgroundColor = new float[3] { 0, 1, 0 };
}
ModelController::~ModelController()
{
    
}

void ModelController::DrawModel()
{
    glClearColor(m_backgroundColor[0], m_backgroundColor[1], m_backgroundColor[2], 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void ModelController::Update(double a_delta, const WebcamController& a_webcamController)
{
    ImGui::SetNextWindowPos({ 10, 10 }, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize({ 660, 520 }, ImGuiCond_Appearing);
    if (ImGui::Begin("Preview"))
    {
        ImGui::Image((ImTextureID)a_webcamController.GetTexture()->GetHandle(), { 640, 480 });
    }
    ImGui::End();

    ImGui::SetNextWindowPos({ 670, 10 }, ImGuiCond_Appearing);
    if (ImGui::Begin("Options"))
    {
        ImGui::ColorPicker3("Background Color", m_backgroundColor);
    }
    ImGui::End();
}

ModelController* ModelController::Load(ZipArchive::Ptr& a_archive)
{
    ModelController* modelController = new ModelController();

    std::shared_ptr<ZipArchiveEntry> propertiesEntry = a_archive->GetEntry("main.prop");

    char* propertiesData;
    GETFILEDATA(propertiesData, propertiesEntry);

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
    delete[] propertiesData;

    return modelController;
}
std::istream* ModelController::SaveToStream() const
{
    PropertyFile* propertyFile = new PropertyFile();
    PropertyFileProperty* prop = propertyFile->InsertProperty();

    prop->SetName("backcolor");
    prop->EmplaceValue("r", std::to_string(m_backgroundColor[0]).c_str());
    prop->EmplaceValue("g", std::to_string(m_backgroundColor[1]).c_str());
    prop->EmplaceValue("b", std::to_string(m_backgroundColor[2]).c_str());

    char* data = propertyFile->ToString();

    IMemoryStream* memStream = new IMemoryStream(data, strlen(data));

    delete[] data;

    return memStream;
}