#include "SkeletonEditor.h"

#include <glad/glad.h>
#include <list>
#include <string>

#include "FileUtils.h"
#include "imgui.h"
#include "IntermediateRenderer.h"
#include "MemoryStream.h"
#include "Models/Model.h"
#include "Object.h"
#include "PropertyFile.h"
#include "RenderTexture.h"
#include "Texture.h"
#include "Transform.h"

SkeletonEditor::SkeletonEditor() :
    m_baseObject(new Object()),
    m_selectedObject(nullptr)
{
    m_baseObject->SetTrueName("Root Object");

    m_renderTexture = new RenderTexture(1920, 1080, GL_RGB);

    m_imRenderer = new IntermediateRenderer();
}

SkeletonEditor::~SkeletonEditor()
{
    delete m_baseObject;

    delete m_imRenderer;
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

void SkeletonEditor::DrawObjectDetail(Object* a_object) const
{
    if (a_object != nullptr)
    {
        const std::list<Object*> children = a_object->GetChildren();

        const Transform* transform = a_object->GetTransform();
        // My brain hurts after this so aparently if I call the variable pos and it is a vector3 I get runtime exceptions 
        // However if I try to use pos there is no type or variable called it and the compilation fails 
        // This is fixed by using either a vec4 or just simply renaming it 
        // This also persisted through me cleaning all build files
        // It also only occurs in Debug
        // This is exclusive to GCC by what I can tell
        // Only this function aswell...
        const glm::vec3 posi = transform->GetWorldPosition();

        for (auto iter = children.begin(); iter != children.end(); ++iter)
        {
            const Transform* cTransform = (*iter)->GetTransform();
            const glm::vec3 cPos = cTransform->GetWorldPosition();

            m_imRenderer->DrawLine(posi, cPos, 0.025f, { 1, 0, 0, 1 });

            DrawObjectDetail(*iter);
        }
    }
}

void UpdateObject(Object* a_object, double a_delta)
{
    if (a_object != nullptr)
    {
        a_object->UpdateComponents(true, a_delta);

        const std::list<Object*> children = a_object->GetChildren();

        for (auto iter = children.begin(); iter != children.end(); ++iter)
        {
            UpdateObject(*iter, a_delta);
        }
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
        }
        ImGui::End();
    }

    if (m_baseObject != nullptr)
    {
        ImGui::SetNextWindowSize({ 200, 600 }, ImGuiCond_Appearing);
        if (ImGui::Begin("Skeleton Hierarchy"))
        {
            int node = 0;
        
            ListObjects(m_baseObject, node);
        }
        ImGui::End();

        ImGui::SetNextWindowSize({ 660, 520 }, ImGuiCond_Appearing);
        if (ImGui::Begin("Skeleton Preview"))
        {
            const ImVec2 winPos = ImGui::GetWindowPos();
            const ImVec2 winSize = ImGui::GetWindowSize();

            m_imRenderer->Reset();

            DrawObjectDetail(m_baseObject);

            m_renderTexture->Bind();

            glClearColor(0.0, 0.0, 0.0, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    

            m_imRenderer->Draw();
            
            UpdateObject(m_baseObject, a_delta);

            m_renderTexture->Unbind();

            ImGui::Image((ImTextureID)m_renderTexture->GetTexture()->GetHandle(), { 640, 480 });
        }
        ImGui::End();
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