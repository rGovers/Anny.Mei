#include "Components/Renderer.h"

#include <limits>

#include "AnimControl.h"
#include "FileUtils.h"
#include "imgui/imgui.h"
#include "PropertyFile.h"

const static int BUFFER_SIZE = 1024;

Renderer::Renderer(Object* a_object, AnimControl* a_animControl) :
    Component(a_object, a_animControl)
{
    m_modelName = new AnimValue<StringKeyValue>(a_animControl);

    m_anchor = new AnimValue<Vec3KeyValue>(a_animControl);
    m_anchor->GetValue()->SetBaseValue({ 0.5f, 0.5f, 0.0f });
}

Renderer::~Renderer()
{
    delete m_modelName;
    delete m_anchor;
}

void Renderer::UpdateRendererGUI()
{
    StringKeyValue* nameValue = m_modelName->GetValue();

    if (nameValue != nullptr)
    {
        const char* str = nameValue->GetString();

        char* buff;
        if (str != nullptr)
        {
            const size_t len = strlen(str) + 1;

            buff = new char[len] { 0 };

            strcpy(buff, str);
        }
        else
        {
            buff = new char[1] { 0 };
        }

        ImGui::InputText("Model Name", buff, BUFFER_SIZE);

        if (str == nullptr || strcmp(buff, str) != 0)
        {
            nameValue->SetString(buff);
        }

        delete[] buff;
    }

    Vec3KeyValue* anchorValue = m_anchor->GetValue();

    if (anchorValue != nullptr)
    {
        glm::vec3 anchor = anchorValue->GetBaseValue();

        ImGui::InputFloat3("Anchor Position", (float*)&anchor);

        anchorValue->SetBaseValue(anchor);
    }
}

void Renderer::LoadValues(PropertyFileProperty* a_property, AnimControl* a_animControl)
{
    const std::list<PropertyFileValue> values = a_property->Values();

    glm::vec3 anchor = glm::vec3(std::numeric_limits<float>::infinity());
    char* name = nullptr; 

    for (auto iter = values.begin(); iter != values.end(); ++iter)
    {
        IFSETTOATTVALCPY(iter->Name, "modelName", name, iter->Value)
        else IFSETTOATTVALV3(iter->Name, "anchor", anchor, iter->Value)
    }

    if (anchor.x != std::numeric_limits<float>::infinity() && anchor.y != std::numeric_limits<float>::infinity() && anchor.z != std::numeric_limits<float>::infinity())
    {
        Vec3KeyValue* value = m_anchor->GetValue();

        if (value != nullptr)
        {
            value->SetBaseValue(anchor);
        }
    }

    if (name != nullptr)
    {
        StringKeyValue* value = m_modelName->GetValue();

        if (value != nullptr)
        {
            value->SetString(name);
        }

        delete[] name;
    }
}
PropertyFileProperty* Renderer::SaveValues(PropertyFile* a_propertyFile, PropertyFileProperty* a_parent) const
{
    PropertyFileProperty* property = a_propertyFile->InsertProperty();
    property->SetParent(a_parent);
    property->SetName(ComponentName());

    m_modelName->SelectKeyFrame(0);
    const StringKeyValue* nameValue = m_modelName->GetValue();
    if (nameValue != nullptr)
    {
        property->EmplaceValue("modelName", nameValue->GetString());
    }

    m_anchor->SelectKeyFrame(0);
    const Vec3KeyValue* anchorValue = m_anchor->GetValue();
    if (anchorValue != nullptr)
    {
        const glm::vec3 anchor = anchorValue->GetBaseValue();

        const std::string anchorStr = "{ " + std::to_string(anchor.x) + ", " + std::to_string(anchor.y) + ", " + std::to_string(anchor.z) + " }";
        property->EmplaceValue("anchor", anchorStr.c_str());
    }

    return property;
}

const char* Renderer::GetModelName() const
{
    StringKeyValue* value = m_modelName->GetValue();

    if (value != nullptr)
    {
        return value->GetString();
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

const char* Renderer::ComponentName() const
{
    return "Renderer";
}