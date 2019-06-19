#include "ModelController.h"

#include <fstream>
#include <glad/glad.h>

#include "FileUtils.h"
#include "imgui.h"
#include "MemoryStream.h"
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

    std::shared_ptr<ZipArchiveEntry> propertiesEntry = a_archive->GetEntry("properties.conf");

    char* propertiesData;
    GETFILEDATA(propertiesData, propertiesEntry);

    memcpy((char*)modelController->m_backgroundColor, propertiesData + 0, sizeof(float) * 3);

    delete[] propertiesData;

    return modelController;
}
std::istream* ModelController::SaveToStream() const
{
    std::vector<char> data;

    data.insert(data.end(), (char*)m_backgroundColor, (char*)m_backgroundColor + 12);

    IMemoryStream* memStream = new IMemoryStream(data.data(), data.size());

    return memStream;
}