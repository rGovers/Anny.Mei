#include "Components/Renderer.h"

#include <limits>

#include "AnimControl.h"
#include "FileLoaders/PropertyFile.h"
#include "FileUtils.h"
#include "imgui/imgui.h"
#include "Object.h"

Renderer::Renderer(Object* a_object, AnimControl* a_animControl) :
    Component(a_object, a_animControl)
{
}
Renderer::~Renderer()
{
    delete m_modelName;
    delete m_anchor;
    delete m_depthTest;
    delete m_useMask;
}

void Renderer::InitValues()
{
    AnimControl* animControl = GetAnimControl();
    const Object* object = GetObject();

    const std::string baseName = std::string("[") + object->GetName() + "] [" + this->ComponentName() + "] ";

    m_modelName = new AnimValue<StringKeyValue>((baseName + "Model Name").c_str(), animControl);
    m_anchor = new AnimValue<Vec3KeyValue>((baseName + "Anchor").c_str(), animControl);
    m_depthTest = new AnimValue<BoolKeyValue>((baseName + "Depth Test").c_str(), animControl);
    m_useMask = new AnimValue<StringKeyValue>((baseName + "Use Mask").c_str(), animControl);

    m_anchor->SelectKeyFrame(0);
    m_anchor->GetValue()->SetBaseValue({ 0.5f, 0.5f, 0.0f });

    m_depthTest->SelectKeyFrame(0);
    m_depthTest->GetValue()->SetBoolean(true);
}
void Renderer::RenameValues()
{
    const Object* object = GetObject();

    const std::string baseName = std::string("[") + object->GetName() + "] [" + this->ComponentName() + "] ";

    m_modelName->Rename((baseName + "Model Name").c_str());
    m_anchor->Rename((baseName + "Anchor").c_str());
    m_depthTest->Rename((baseName + "Depth Test").c_str());
    m_useMask->Rename((baseName + "Use Mask").c_str());
}

void Renderer::DisplayRendererValues(bool a_value)
{
    m_anchor->SetDisplayState(a_value);
    
    m_modelName->SetDisplayState(a_value);

    m_depthTest->SetDisplayState(a_value);

    m_useMask->SetDisplayState(a_value);
}

void Renderer::Init()
{
    InitValues();
}

void Renderer::UpdateRendererGUI()
{
    StringKeyValue* nameValue = m_modelName->GetValue();
    if (nameValue != nullptr)
    {
        nameValue->UpdateGUI("Model Name");
    }

    Vec3KeyValue* anchorValue = m_anchor->GetValue();
    if (anchorValue != nullptr)
    {
        anchorValue->UpdateGUI("Anchor");
    }

    BoolKeyValue* depthTestValue = m_depthTest->GetValue();
    if (depthTestValue != nullptr)
    {
        depthTestValue->UpdateGUI("Depth Test");
    }

    StringKeyValue* maskValue = m_useMask->GetValue();
    if (nameValue != nullptr)
    {
        maskValue->UpdateGUI("Use Mask");
    }
}

PropertyFileProperty* Renderer::SaveValues(PropertyFile* a_propertyFile, PropertyFileProperty* a_parent) const
{
    PropertyFileProperty* property = a_propertyFile->InsertProperty();
    property->SetParent(a_parent);
    property->SetName(ComponentName());

    return property;
}

const char* Renderer::GetModelName() const
{
    StringKeyValue* value = m_modelName->GetAnimValue();

    if (value != nullptr)
    {
        return value->GetString();
    }

    return nullptr;
}
const char* Renderer::GetBaseModelName() const
{
    StringKeyValue* value = m_modelName->GetValue();

    if (value != nullptr)
    {
        return value->GetBaseString();
    }

    return nullptr;
}

glm::vec3 Renderer::GetAnchor() const
{
    Vec3KeyValue* value = m_anchor->GetAnimValue();

    if (value != nullptr)
    {
        return value->GetValue();
    }

    return glm::vec3(0.5, 0.5f, 0.0f);
}
glm::vec3 Renderer::GetBaseAnchor() const
{
    Vec3KeyValue* value = m_anchor->GetValue();

    if (value != nullptr)
    {
        return value->GetBaseValue();
    }

    return glm::vec3(0.5f, 0.5f, 0.0f);
}

bool Renderer::GetDepthTest() const
{
    BoolKeyValue* value = m_depthTest->GetAnimValue();

    if (value != nullptr)
    {
        return value->GetBoolean();
    }

    return true;
}

bool Renderer::GetBaseDepthTest() const
{
    BoolKeyValue* value = m_depthTest->GetValue();

    if (value != nullptr)
    {
        return value->GetBaseBoolean();
    }

    return true;
}

const char* Renderer::GetMaskName() const
{
    StringKeyValue* value = m_useMask->GetAnimValue();

    if (value != nullptr)
    {
        return value->GetString();
    }

    return nullptr;
}
const char* Renderer::GetBaseMaskName() const
{
    StringKeyValue* value = m_useMask->GetValue();

    if (value != nullptr)
    {
        return value->GetBaseString();
    }

    return nullptr;
}

void Renderer::ObjectRenamed()
{
    RenameValues();
}

const char* Renderer::ComponentName() const
{
    return "Renderer";
}

void Renderer::DisplayValues(bool a_value)
{
    DisplayRendererValues(a_value);
}

void Renderer::Save(PropertyFile* a_propertyFile, PropertyFileProperty* a_parent) const
{
    SaveValues(a_propertyFile, a_parent);
}