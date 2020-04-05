#include "Components/MorphTargetMask.h"

#include <glad/glad.h>

#include "DataStore.h"
#include "DepthRenderTexture.h"
#include "FileLoaders/PropertyFile.h"
#include "FileUtils.h"
#include "imgui.h"

const char* MorphTargetMask::COMPONENT_NAME = "MorphTargetMask";

const static int BUFFER_SIZE = 1024;
static char* BUFFER = new char[BUFFER_SIZE];

MorphTargetMask::MorphTargetMask(Object* a_object, AnimControl* a_animControl) :
    MorphTargetRenderer(a_object, a_animControl)
{
    m_maskName = new char[1] { 0 };

    m_renderTexture = new DepthRenderTexture(2048, 2048);
    m_previewRenderTexture = new DepthRenderTexture(2048, 2048);
}
MorphTargetMask::~MorphTargetMask()
{
    if (m_maskName != nullptr)
    {
        DataStore* store = DataStore::GetInstance();

        store->RemoveMask(m_maskName, m_renderTexture);
        store->RemovePreviewMask(m_maskName, m_previewRenderTexture);

        delete[] m_maskName;
    }

    delete m_renderTexture;
    delete m_previewRenderTexture;
}

void MorphTargetMask::Draw(bool a_preview, double a_delta, Camera* a_camera)
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

    MorphTargetDraw(a_preview, a_delta, a_camera);

    renderTexture->Unbind();
}

void MorphTargetMask::Update(double a_delta, Camera* a_camera)
{
    Draw(false, a_delta, a_camera);
}
void MorphTargetMask::UpdatePreview(double a_delta, Camera* a_camera)
{
    Draw(true, a_delta, a_camera);
}
void MorphTargetMask::UpdateGUI()
{
    DataStore* store = DataStore::GetInstance();

    strcpy(BUFFER, m_maskName);

    ImGui::InputText("Mask Name", BUFFER, BUFFER_SIZE);

    if (strcmp(BUFFER, m_maskName) != 0)
    {
        store->RemoveMask(m_maskName, m_renderTexture);
        store->RemovePreviewMask(m_maskName, m_previewRenderTexture);

        delete[] m_maskName;

        const size_t len = strlen(BUFFER);
        m_maskName = new char[len + 1];
        strcpy(m_maskName, BUFFER);

        if (BUFFER[0] != 0)
        {
            store->AddMask(BUFFER, m_renderTexture);
            store->AddPreviewMask(BUFFER, m_previewRenderTexture);
        }
    }

    MorphTargetUpdateGUI();
}

const char* MorphTargetMask::ComponentName() const
{
    return COMPONENT_NAME;
}

void MorphTargetMask::Load(PropertyFileProperty* a_property, AnimControl* a_animControl)
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
void MorphTargetMask::Save(PropertyFile* a_propertyFile, PropertyFileProperty* a_parent) const
{
    PropertyFileProperty* property = SaveValues(a_propertyFile, a_parent);

    property->EmplaceValue("maskname", m_maskName);
}