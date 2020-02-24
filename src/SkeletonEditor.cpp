#include "SkeletonEditor.h"

#include <glad/glad.h>
#include <list>
#include <string>

#include "AnimControl.h"
#include "Camera.h"
#include "FileUtils.h"
#include "imgui.h"
#include "IntermediateRenderer.h"
#include "MemoryStream.h"
#include "Models/Model.h"
#include "Namer.h"
#include "Object.h"
#include "PropertyFile.h"
#include "RenderTexture.h"
#include "Texture.h"
#include "Transform.h"

const static float MOUSE_SENSITIVITY = 0.001f;
const static float MOUSE_WHEEL_SENSITIVITY = 0.1f;
const static int IMAGE_SIZE = 4096;
const static float MAX_ZOOM = 2.5f;

SkeletonEditor::SkeletonEditor()
{
    m_animControl = new AnimControl();

    m_namer = new Namer();

    m_baseObject = new Object(m_namer);
    m_baseObject->SetTrueName("Root Object");

    m_selectedObject = nullptr;

    m_lastMousePos = glm::vec2(-1);
    m_zoom = 1;

    m_renderTexture = new RenderTexture(IMAGE_SIZE, IMAGE_SIZE, GL_RGB);

    m_camera = new Camera();

    m_imRenderer = new IntermediateRenderer();
}

SkeletonEditor::~SkeletonEditor()
{
    delete m_baseObject;

    delete m_imRenderer;

    delete m_renderTexture;

    delete m_camera;

    delete m_animControl;
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
    bool selected = false;

    if (children.size() > 0)
    {
        open = ImGui::TreeNode((void*)a_node, "");
        ImGui::SameLine();
    }
    else
    {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetTreeNodeToLabelSpacing());
    }
    
    ImGui::Selectable(name, &selected);

    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Add Object"))
        {
            Object* object = new Object(m_namer);
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
        const glm::vec4 posi = glm::vec4(transform->GetWorldPosition(), 1.0f);

        const glm::mat4 view = glm::inverse(m_camera->GetTransform()->ToMatrix());
        const glm::mat4 proj = m_camera->GetProjection();

        const glm::vec3 fPos = view * proj * posi;

        if (a_object == m_selectedObject)
        {
            m_imRenderer->DrawCircle(fPos, 20, 0.05f, 0.01f, { 0, 1, 0, 1});
        }
        else
        {
            m_imRenderer->DrawCircle(fPos, 20, 0.02f, 0.01f);
        }

        for (auto iter = children.begin(); iter != children.end(); ++iter)
        {
            const Transform* cTransform = (*iter)->GetTransform();

            const glm::vec4 cPos = glm::vec4(cTransform->GetWorldPosition(), 1.0f);

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
    m_animControl->Update(a_delta);

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

            if (m_selectedObject != m_baseObject)
            {
                ImGui::InputText("Name", buffer, BUFFER_SIZE);
                
                ImGui::Separator();
            }

            Transform* transform = m_selectedObject->GetTransform();

            ImGui::InputFloat3("Translation", (float*)&transform->Translation(), 4);

            // Need to a some point implement euler angles for easy user control
            glm::fquat quat = transform->Rotation();
            ImGui::InputFloat4("Rotation", (float*)&quat, 4);
            transform->SetRotation(glm::normalize(quat));

            ImGui::InputFloat3("Scale", (float*)&transform->Scale(), 4);

            m_selectedObject->SetTrueName(buffer);

            ImGui::Separator();

            m_selectedObject->UpdateComponentUI(m_animControl);
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

        ImGui::SetNextWindowSize({ 660, 400 }, ImGuiCond_Appearing);
        if (ImGui::Begin("Skeleton Preview"))
        {
            const ImVec2 size = ImGui::GetWindowSize();

            if (ImGui::IsWindowFocused())
            {
                glm::vec3 translation = m_camera->GetTransform()->Translation();

                if (ImGui::IsMouseDown(2))
                {
                    const ImVec2 tPos = ImGui::GetMousePos();
                    const glm::vec2 mousePos = { tPos.x, tPos.y };

                    if (m_lastMousePos.x >= 0 && m_lastMousePos.y >= 0)
                    {
                        glm::vec2 mov = m_lastMousePos - mousePos;

                        translation += glm::vec3(mov.x, mov.y, 0.0f) * MOUSE_SENSITIVITY;
                    }

                    m_lastMousePos = mousePos;
                }
                else
                {
                    m_lastMousePos = glm::vec2(-1);
                }

                const float mouseWheel = ImGui::GetIO().MouseWheel;

                const float wheelDelta = mouseWheel * MOUSE_WHEEL_SENSITIVITY;

                m_zoom = glm::clamp(m_zoom - wheelDelta, 0.01f, MAX_ZOOM);

                translation.x = glm::clamp(translation.x, -5.0f, 5.0f);
                translation.y = glm::clamp(translation.y, -5.0f, 5.0f);

                const glm::vec2 trueSize = { (size.x - 20) / IMAGE_SIZE, (size.y - 40) / IMAGE_SIZE };

                const glm::mat4 proj = glm::orthoRH(0.0f, trueSize.x * m_zoom * 5, 0.0f, trueSize.y * m_zoom * 5, -1.0f, 1.0f);

                m_camera->SetProjection(proj);
                m_camera->GetTransform()->Translation() = translation;
            }
            
            m_imRenderer->Reset();

            DrawObjectDetail(m_baseObject);

            m_renderTexture->Bind();

            glClearColor(0.1, 0.1, 0.1, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    

            m_imRenderer->Draw();
            
            UpdateObject(m_baseObject, m_camera, a_delta);

            m_renderTexture->Unbind();

            ImGui::Image((ImTextureID)m_renderTexture->GetTexture()->GetHandle(), { size.x - 20, size.y - 40 });
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
            Object* object = new Object(m_namer);
            object->SetParent(a_object);

            LoadObject(object, *iter);
        }
        else
        {
            a_object->LoadComponent(*iter, m_animControl);
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
    
        a_object->SaveComponents(a_propertyFile, property);

        const std::list<Object*> children = a_object->GetChildren();
        for (auto iter = children.begin(); iter != children.end(); ++iter)
        {
            SaveObject(a_propertyFile, property, *iter);
        }
    }
}