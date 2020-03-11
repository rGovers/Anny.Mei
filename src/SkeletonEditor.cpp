#include "SkeletonEditor.h"

#include <glad/glad.h>
#include <list>
#include <string>

#include "AnimControl.h"
#include "Camera.h"
#include "ColorTheme.h"
#include "FileLoaders/PropertyFile.h"
#include "FileUtils.h"
#include "imgui.h"
#include "IntermediateRenderer.h"
#include "MemoryStream.h"
#include "Models/Model.h"
#include "Namer.h"
#include "Object.h"
#include "RenderTexture.h"
#include "StaticTransform.h"
#include "Texture.h"
#include "Transform.h"
#include "WindowControls/SkeletonEditorWindow.h"
#include "Workspace.h"

const static int IMAGE_SIZE = 4096;

SkeletonEditor::SkeletonEditor(Workspace* a_workspace)
{
    m_window = new SkeletonEditorWindow(this);

    m_workspace = a_workspace;

    m_animControl = new AnimControl();

    m_namer = new Namer();

    m_baseObject = new Object(m_namer, m_animControl);
    m_baseObject->SetTrueName("Root Object");

    m_selectedObject = nullptr;

    m_renderTexture = new RenderTexture(IMAGE_SIZE, IMAGE_SIZE, GL_RGB);

    m_imRenderer = new IntermediateRenderer();
}

SkeletonEditor::~SkeletonEditor()
{
    delete m_baseObject;

    delete m_imRenderer;

    delete m_renderTexture;

    delete m_animControl;

    delete m_window;
}

Object* SkeletonEditor::GetBaseObject() const
{
    return m_baseObject;
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
    bool is_selected = (m_selectedObject == a_object);

    if (children.size() > 0)
    {
        open = ImGui::TreeNode((void*)a_node, "");
        ImGui::SameLine();
    }
    else
    {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetTreeNodeToLabelSpacing());
    }
    
    if (ImGui::Selectable(name, &is_selected))
    {
        if (m_selectedObject != nullptr)
        {
            m_selectedObject->DisplayValues(false);
        }

        m_selectedObject = a_object;

        if (m_selectedObject != nullptr)
        {
            m_selectedObject->DisplayValues(true);
        }

        m_workspace->SelectWorkspace(this);
    }

    if (is_selected)
    {
        ImGui::SetItemDefaultFocus();
    }

    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Add Object"))
        {
            Object* object = new Object(m_namer, m_animControl);
            object->SetParent(a_object);
        }

        if (a_object != m_baseObject)
        {
            if (ImGui::MenuItem("Remove Object"))
            {
                if (a_object == m_selectedObject)
                {
                    m_selectedObject = nullptr;
                }

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
        const ImVec2 winSize = ImGui::GetWindowSize();
        const glm::vec2 scaledWinSize = { winSize.x - 20, winSize.y - 60 };

        glm::vec2 scalar = glm::vec2(1);

        if (scaledWinSize.x > scaledWinSize.y)
        {
            scalar.x = scaledWinSize.y / scaledWinSize.x;
        }
        else
        {
            scalar.y = scaledWinSize.x / scaledWinSize.y;
        }

        const Camera* cam = m_window->GetCamera();

        const std::list<Object*> children = a_object->GetChildren();

        const Transform* transform = a_object->GetTransform();
        // My brain hurts after this so aparently if I call the variable pos and it is a vector3 I get runtime exceptions 
        // However if I try to use pos there is no type or variable called it and the compilation fails 
        // This is fixed by using either a vec4 or just simply renaming it 
        // This also persisted through me cleaning all build files
        // It also only occurs in Debug
        // This is exclusive to GCC by what I can tell
        // Only this function aswell...
        const glm::vec4 posi = glm::vec4(transform->GetBaseWorldPosition(), 1.0f);

        const glm::mat4 view = glm::inverse(cam->GetTransform()->ToMatrix());
        const glm::mat4 proj = cam->GetProjection();

        const glm::vec3 fPos = view * proj * posi;

        if (a_object == m_selectedObject)
        {
            m_imRenderer->DrawSolidCircle(fPos, 20, 0.05f, ACTIVE_COLOR, scalar.x, scalar.y);
        }
        else
        {
            m_imRenderer->DrawCircle(fPos, 20, 0.02f, 0.01f, INACTIVE_COLOR, scalar.x, scalar.y);
        }

        for (auto iter = children.begin(); iter != children.end(); ++iter)
        {
            const Transform* cTransform = (*iter)->GetTransform();

            const glm::vec4 cPos = glm::vec4(cTransform->GetBaseWorldPosition(), 1.0f);

            const glm::vec3 fCPos = view * proj * cPos;

            m_imRenderer->DrawLine(fPos, fCPos, 0.01f, { 1, 0, 0, 1 });

            DrawObjectDetail(*iter);
        }
    }
}

void UpdateObject(Object* a_object, Camera* a_camera, double a_delta)
{
    if (a_object != nullptr)
    {
        a_object->UpdateComponents(true, a_camera, a_delta);

        const std::list<Object*> children = a_object->GetChildren();

        for (auto iter = children.begin(); iter != children.end(); ++iter)
        {
            UpdateObject(*iter, a_camera, a_delta);
        }
    }
}

void SkeletonEditor::Update(double a_delta)
{
    m_window->Update();

    m_animControl->Update(a_delta);

    m_delta = a_delta;
}

const Texture* SkeletonEditor::DrawEditor()
{
    DrawObjectDetail(m_baseObject);

    m_renderTexture->Bind();

    glClearColor(0.1, 0.1, 0.1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    

    m_imRenderer->Draw();
            
    Camera* camera = m_window->GetCamera();

    UpdateObject(m_baseObject, camera, m_delta);

    m_renderTexture->Unbind();

    return m_renderTexture->GetTexture();
}

SkeletonEditor* SkeletonEditor::Load(mz_zip_archive& a_archive, Workspace* a_workspace)
{
    SkeletonEditor* skeletonController = new SkeletonEditor(a_workspace);

    char* data = ExtractFileFromArchive("skeleton.prop", a_archive);
    if (data != nullptr)
    {
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
    }
    
    delete[] data;

    skeletonController->m_animControl->LoadValues(a_archive);

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

    m_animControl->SaveValues(a_archive);
}

void SkeletonEditor::LoadObject(Object* a_object, PropertyFileProperty* a_property)
{
    const std::list<PropertyFileValue> values = a_property->Values();

    char* name = nullptr;
    char* trueName = nullptr;

    for (auto iter = values.begin(); iter != values.end(); ++iter)
    {
        IFSETTOATTVALCPY(iter->Name, "name", name, iter->Value)
        else IFSETTOATTVALCPY(iter->Name, "truename", trueName, iter->Value)
    }

    if (name != nullptr && trueName != nullptr)
    {
        a_object->SetTrueName(trueName);
        a_object->SetName(name);
    }

    const std::list<PropertyFileProperty*> children = a_property->GetChildren();
    for (auto iter = children.begin(); iter != children.end(); ++iter)
    {
        if (strcmp((*iter)->GetName(), "object") == 0)
        {
            Object* object = new Object(m_namer, m_animControl);
            object->SetParent(a_object);

            LoadObject(object, *iter);
        }
        else
        {
            a_object->LoadComponent(*iter);
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
    
        a_object->SaveComponents(a_propertyFile, property);

        const std::list<Object*> children = a_object->GetChildren();
        for (auto iter = children.begin(); iter != children.end(); ++iter)
        {
            SaveObject(a_propertyFile, property, *iter);
        }
    }
}

void SkeletonEditor::DrawPropertiesWindow()
{
    m_window->UpdatePropertiesWindow(m_selectedObject);
}
void SkeletonEditor::DrawEditorWindow()
{
    m_imRenderer->Reset();

    m_window->UpdateEditorWindow();
}