#include "Components/MorphPlaneMask.h"

#include <glad/glad.h>

#include <string>

#include "Camera.h"
#include "DataStore.h"
#include "DepthRenderTexture.h"
#include "FileLoaders/PropertyFile.h"
#include "FileUtils.h"
#include "imgui.h"
#include "Object.h"
#include "StaticTransform.h"

const char* MorphPlaneMask::COMPONENT_NAME = "MorphPlaneMask";

const static int BUFFER_SIZE = 1024;

MorphPlaneMask::MorphPlaneMask(Object* a_object, AnimControl* a_animControl) :
    MorphPlaneRenderer(a_object, a_animControl)
{
    m_animValuesDisplayed = false;

    m_renderTexture = new DepthRenderTexture(2048, 2048);
    m_previewRenderTexture = new DepthRenderTexture(2048, 2048);

    m_maskName = new char[1] { 0 };
}
MorphPlaneMask::~MorphPlaneMask()
{
    if (m_maskName != nullptr)
    {   
        DataStore* store = DataStore::GetInstance();

        store->RemoveMask(m_maskName, m_renderTexture);
        store->RemovePreviewMask(m_maskName, m_previewRenderTexture);
    }

    delete m_renderTexture;
    delete m_previewRenderTexture;

    delete[] m_maskName;
}

void MorphPlaneMask::Init()
{
    MorphPlaneInit();
}
void MorphPlaneMask::ObjectRenamed()
{
    MorphPlaneObjectRenamed();

}
void MorphPlaneMask::Draw(bool a_preview, double a_delta, Camera* a_camera)
{
    DataStore* store = DataStore::GetInstance();

    DepthRenderTexture* renderTexture;

    if (a_preview)
    {
        renderTexture = m_previewRenderTexture;
    }
    else
    {
        renderTexture = m_renderTexture;
    }    

    renderTexture->Bind();

    glClear(GL_DEPTH_BUFFER_BIT);

    MorphPlaneDraw(a_preview, a_delta, a_camera);

    renderTexture->Unbind();
}

void MorphPlaneMask::Update(double a_delta, Camera* a_camera)
{
    Draw(false, a_delta, a_camera);
}
void MorphPlaneMask::UpdatePreview(double a_delta, Camera* a_camera)
{
    Draw(true, a_delta, a_camera);
}
void MorphPlaneMask::UpdateGUI()
{
    DataStore* store = DataStore::GetInstance();

    char* buff;

    const size_t len = strlen(m_maskName);

    buff = new char[len + 2];
    strcpy(buff, m_maskName);

    ImGui::InputText("Mask Name", buff, BUFFER_SIZE);

    if (strcmp(buff, m_maskName) != 0)
    {
        store->RemoveMask(m_maskName, m_renderTexture);
        store->RemovePreviewMask(m_maskName, m_previewRenderTexture);

        delete[] m_maskName;

        m_maskName = buff;

        if (buff[0] != 0)
        {
            store->AddMask(buff, m_renderTexture);
            store->AddPreviewMask(buff, m_previewRenderTexture);
        }
    }
    else
    {
        delete[] buff;
    }
        
    MorphPlaneUpdateGUI();
}

const char* MorphPlaneMask::ComponentName() const
{
    return COMPONENT_NAME;
}

void MorphPlaneMask::DisplayValues(bool a_value)
{
    MorphPlaneDisplayValues(a_value);
}

void MorphPlaneMask::Load(PropertyFileProperty* a_property, AnimControl* a_animControl)
{
    DataStore* store = DataStore::GetInstance();

    const std::list<PropertyFileValue> values = a_property->Values();

    if (m_maskName != nullptr)
    {
        delete[] m_maskName;
        m_maskName = nullptr;
    }

    for (auto iter = values.begin(); iter != values.end(); ++iter)
    {
        IFSETTOATTVALCPY(iter->Name, "maskname", m_maskName, iter->Value)
    }

    if (m_maskName == nullptr)
    {
        m_maskName = new char[1] { 0 };
    }
    else 
    {
        if (m_maskName[0] != 0)
        {
            store->AddMask(m_maskName, m_renderTexture);
            store->AddPreviewMask(m_maskName, m_previewRenderTexture);
        }
    }
}
void MorphPlaneMask::Save(PropertyFile* a_propertyFile, PropertyFileProperty* a_parent) const
{
    PropertyFileProperty* property = SaveValues(a_propertyFile, a_parent);

    property->EmplaceValue("maskname", m_maskName);
}
