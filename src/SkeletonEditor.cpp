#include "SkeletonEditor.h"

#include <list>
#include <string>

#include "FileUtils.h"
#include "imgui.h"
#include "MemoryStream.h"
#include "Models/Model.h"
#include "Object.h"
#include "PropertyFile.h"
#include "Transform.h"

SkeletonEditor::SkeletonEditor() :
    m_baseObject(new Object()),
    m_selectedObject(nullptr)
{
    m_baseObject->SetTrueName("Root Object");
}

SkeletonEditor::~SkeletonEditor()
{
    delete m_baseObject;
}

void SkeletonEditor::ListObjects(Object* a_object, int& a_node)
{
    const char* name = a_object->GetName();

    if (name == nullptr || name[0] == 0)
    {
        name = "NULL";
    }

    const std::list<Object*> children = a_object->GetChildren();

    bool open = false;
    bool selected = false;

    if (children.size() > 0)
    {
        open = ImGui::TreeNode((void*)a_node, "");
        ImGui::SameLine();
    }
    else
    {
        ImGui::TreeAdvanceToLabelPos();
    }
    
    ImGui::Selectable(name, &selected);

    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Add Object"))
        {
            Object* object = new Object();
            object->SetParent(a_object);
        }

        if (a_object != m_baseObject)
        {
            if (ImGui::MenuItem("Remove Object"))
            {
                delete a_object;

                ImGui::EndPopup();

                if (open)
                {
                    ImGui::TreePop();
                }

                return;
            }
        }
        
        ImGui::EndPopup();
    }

    if (selected)
    {
        m_selectedObject = a_object;
    }

    ++a_node;

    if (open)
    {
        for (auto iter = children.begin(); iter != children.end(); ++iter)
        {
            ListObjects(*iter, a_node);
        }

        ImGui::TreePop();
    }   
}

void SkeletonEditor::Update(double a_delta)
{
    if (m_selectedObject != nullptr)
    {
        ImGui::SetNextWindowSize({ 200, 400 }, ImGuiCond_Appearing);
        if (ImGui::Begin("Object Properties"))
        {
            static const int BUFFER_SIZE = 1024;

            char* buffer = new char[BUFFER_SIZE];
            buffer[0] = 0;
            
            const char* name = m_selectedObject->GetTrueName();

            if (name != nullptr)
            {
                strcpy(buffer, name);
            }

            ImGui::InputText("Name", buffer, BUFFER_SIZE);

            ImGui::Separator();

            Transform* transform = m_selectedObject->GetTransform();

            ImGui::InputFloat3("Translation", (float*)&transform->Translation(), 4);

            glm::fquat quat = transform->Rotation();
            ImGui::InputFloat4("Rotation", (float*)&quat, 4);
            transform->SetRotation(glm::normalize(quat));

            ImGui::InputFloat3("Scale", (float*)&transform->Scale(), 4);

            m_selectedObject->SetTrueName(buffer);

            ImGui::Separator();

            m_selectedObject->UpdateComponentUI();

            ImGui::End();
        }
    }

    if (m_baseObject != nullptr)
    {
        ImGui::SetNextWindowSize({ 200, 600 }, ImGuiCond_Appearing);
        if (ImGui::Begin("Skeleton Hierarchy"))
        {
            int node = 0;
        
            ListObjects(m_baseObject, node);

            ImGui::End();
        }
    }
}

SkeletonEditor* SkeletonEditor::Load(mz_zip_archive& a_archive)
{
    SkeletonEditor* skeletonController = new SkeletonEditor();

    char* data = ExtractFileFromArchive("skeleton.prop", a_archive);

    PropertyFile* propertyFile = new PropertyFile(data);

    std::list<PropertyFileProperty*> properties = propertyFile->GetBaseProperties();

    for (auto iter = properties.begin(); iter != properties.end(); ++iter)
    {
        std::list<PropertyFileValue> values = (*iter)->Values();

        bool name = false;
        bool trueName = false;

        for (auto vIter = values.begin(); vIter != values.end(); ++vIter)
        {
            if (strcmp(vIter->Name, "name") == 0 && strcmp(vIter->Value, "Root Object") == 0)
            {
                name = true;
            }
            if (strcmp(vIter->Name, "truename") == 0 && strcmp(vIter->Value, "Root Object") == 0)
            {
                trueName = true;
            }
        }

        if (name && trueName)
        {
            skeletonController->LoadObject(skeletonController->m_baseObject, *iter);

            break;
        }
    }

    delete propertyFile;
    delete[] data;

    return skeletonController;
}
void SkeletonEditor::Save(mz_zip_archive& a_archive) const
{
    PropertyFile* propertyFile = new PropertyFile();

    SaveObject(propertyFile, nullptr, m_baseObject);

    const char* data = propertyFile->ToString();

    mz_zip_writer_add_mem(&a_archive, "skeleton.prop", data, strlen(data), MZ_DEFAULT_COMPRESSION);

    delete[] data;
    delete propertyFile;
}

void SkeletonEditor::LoadObject(Object* a_object, PropertyFileProperty* a_property)
{
    const std::list<PropertyFileValue> values = a_property->Values();

    char* name = nullptr;
    char* trueName = nullptr;
    char* transformString = nullptr;

    for (auto iter = values.begin(); iter != values.end(); ++iter)
    {
        IFSETTOATTVALCPY(iter->Name, "name", name, iter->Value)
        else IFSETTOATTVALCPY(iter->Name, "truename", trueName, iter->Value)
        else IFSETTOATTVALCPY(iter->Name, "transform", transformString, iter->Value)
    }

    if (name != nullptr && trueName != nullptr)
    {
        a_object->SetTrueName(trueName);
        a_object->SetName(name);
        
        if (transformString != nullptr)
        {
            a_object->GetTransform()->Parse(transformString);
        }
    }

    const std::list<PropertyFileProperty*> children = a_property->GetChildren();

    for (auto iter = children.begin(); iter != children.end(); ++iter)
    {
        if (strcmp((*iter)->GetName(), "object") == 0)
        {
            Object* object = new Object();
            object->SetParent(a_object);

            LoadObject(object, *iter);
        }
    }
}
void SkeletonEditor::SaveObject(PropertyFile* a_propertyFile, PropertyFileProperty* a_parent, Object* a_object) const
{
    if (a_object != nullptr)
    {
        PropertyFileProperty* property = a_propertyFile->InsertProperty();
        property->SetName("object");
        property->SetParent(a_parent);
        property->EmplaceValue("name", a_object->GetName());
        property->EmplaceValue("truename", a_object->GetTrueName());
        
        char* sTransform = a_object->GetTransform()->ToString();
        property->EmplaceValue("transform", sTransform);
        delete[] sTransform;
    
        const std::list<Object*> children = a_object->GetChildren();
        for (auto iter = children.begin(); iter != children.end(); ++iter)
        {
            SaveObject(a_propertyFile, property, *iter);
        }
    }
}