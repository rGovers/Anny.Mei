#include "Components/Renderer.h"

#include "PropertyFile.h"

const static int BUFFER_SIZE = 1024;

#include "FileUtils.h"
#include "imgui/imgui.h"

Renderer::Renderer(Object* a_object) :
    Component(a_object)
{
    m_modelName = new char[1] { 0 };

    m_anchor = glm::vec3(0.5f, 0.5f, 0.0f);
}

Renderer::~Renderer()
{
    delete[] m_modelName;
}

void Renderer::UpdateRendererGUI()
{
    const size_t len = strlen(m_modelName) + 1;

    char* buff = new char[len] { 0 };
    memcpy(buff, m_modelName, len);

    ImGui::InputText("Model Name", buff, BUFFER_SIZE);

    if (strcmp(buff, m_modelName) != 0)
    {
        delete[] m_modelName;
        m_modelName = buff;
    }
    else
    {
        delete[] buff;
    }

    ImGui::InputFloat3("Anchor Position", (float*)&m_anchor);
}

void Renderer::LoadValues(PropertyFileProperty* a_property)
{
    const std::list<PropertyFileValue> values = a_property->Values();

    for (auto iter = values.begin(); iter != values.end(); ++iter)
    {
        IFSETTOATTVALCPY(iter->Name, "modelName", m_modelName, iter->Value)
        else IFSETTOATTVALV3(iter->Name, "anchor", m_anchor, iter->Value)
    }
}
PropertyFileProperty* Renderer::SaveValues(PropertyFile* a_propertyFile, PropertyFileProperty* a_parent) const
{
    PropertyFileProperty* property = a_propertyFile->InsertProperty();
    property->SetParent(a_parent);
    property->SetName(ComponentName());

    property->EmplaceValue("modelName", m_modelName);

    const std::string anchorStr = "{ " + std::to_string(m_anchor.x) + ", " + std::to_string(m_anchor.y) + ", " + std::to_string(m_anchor.z) + " }";
    property->EmplaceValue("anchor", anchorStr.c_str());

    return property;
}

const char* Renderer::GetModelName() const
{
    return m_modelName;
}
glm::vec3 Renderer::GetAnchor() const
{
    return m_anchor;
}

const char* Renderer::ComponentName() const
{
    return "Renderer";
}