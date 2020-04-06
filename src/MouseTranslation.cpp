#include "Components/MouseTranslation.h"

#include <string>

#include "Application.h"
#include "imgui.h"
#include "Object.h"
#include "Transform.h"

const char* MouseTranslation::COMPONENT_NAME = "MouseTranslation";

MouseTranslation::MouseTranslation(Object* a_object, AnimControl* a_animControl) :
    Component(a_object, a_animControl)
{
    m_prevPos = glm::vec2(0);
    m_prevPreviewPos = glm::vec2(0);

    m_mousePos = glm::vec2(0);
    m_previewMousePos = glm::vec2(0);
}
MouseTranslation::~MouseTranslation()
{
    delete m_mouseScale;
    delete m_delta;
    delete m_offset;
}

void MouseTranslation::Init()
{
    const Object* object = GetObject();
    AnimControl* animControl = GetAnimControl();

    const std::string baseName = std::string("[") + object->GetName() + "] [" + this->ComponentName() + "] ";

    m_mouseScale = new AnimValue<Vec2KeyValue>((baseName + "Mouse Scale").c_str(), animControl);
    m_mouseScale->SelectKeyFrame(0);
    m_mouseScale->GetValue()->SetBaseValue(glm::vec2(0.0001f));

    m_delta = new AnimValue<Vec2KeyValue>((baseName + "Delta").c_str(), animControl);
    m_delta->SelectKeyFrame(0);
    m_delta->GetValue()->SetBaseValue(glm::vec2(1));

    m_offset = new AnimValue<Vec2KeyValue>((baseName + "Offset").c_str(), animControl);
    m_offset->SelectKeyFrame(0);
    m_offset->GetValue()->SetBaseValue(glm::vec2(0.5f));
}
void MouseTranslation::ObjectRenamed()
{
    Object* object = GetObject();

    const std::string baseName = std::string("[") + object->GetName() + "] [" + this->ComponentName() + "] ";
    
    m_mouseScale->Rename((baseName + "Mouse Scale").c_str());
    m_delta->Rename((baseName + "Delta").c_str());
    m_offset->Rename((baseName + "Offset").c_str());
}

void MouseTranslation::ResetFloat(float& a_value)
{
    const float abs = glm::abs(a_value);

    if (abs == std::numeric_limits<float>::infinity() || abs != abs)
    {
        a_value = 0;
    }
}

void MouseTranslation::UpdateTransform(bool a_preview, double a_delta, Camera* a_camera)
{
    glm::vec2 pos;

    // Error resolving for when user enters invalid values
    ResetFloat(m_prevPos.x);
    ResetFloat(m_prevPos.y);
    ResetFloat(m_prevPreviewPos.x);
    ResetFloat(m_prevPreviewPos.y);

    if (a_preview)
    {   
        const Vec2KeyValue* mouseScaleValue = m_mouseScale->GetValue();

        if (mouseScaleValue != nullptr)
        {
            const glm::vec2 scale = mouseScaleValue->GetBaseValue();

            pos = glm::vec2(m_previewMousePos.x * scale.x, m_previewMousePos.y * scale.y);
        }

        const Vec2KeyValue* deltaValue = m_delta->GetValue();
        if (deltaValue != nullptr)
        {
            const glm::vec2 delta = deltaValue->GetBaseValue();
            const glm::vec2 scaledDelta = delta * (float)a_delta;

            pos.x = glm::mix(pos.x, m_prevPreviewPos.x, 1 - scaledDelta.x);
            pos.y = glm::mix(pos.y, m_prevPreviewPos.y, 1 - scaledDelta.y);
        }

        const Object* object = GetObject();
        Transform* transform = object->GetTransform();
        glm::vec3 translation = transform->GetBaseTranslation();

        translation.x = pos.x;
        translation.y = pos.y;

        transform->SetTranslation(translation);

        transform->RefreshValues(0.0);

        m_prevPreviewPos = pos;
    }
    else
    {
        const Vec2KeyValue* mouseScaleValue = m_mouseScale->GetAnimValue();

        if (mouseScaleValue != nullptr)
        {
            const glm::vec2 scale = mouseScaleValue->GetValue();

            pos = glm::vec2(m_mousePos.x * scale.x, m_mousePos.y * scale.y);
        }

        const Vec2KeyValue* deltaValue = m_delta->GetAnimValue();
        if (deltaValue != nullptr)
        {
            const glm::vec2 delta = deltaValue->GetValue();
            const glm::vec2 scaledDelta = delta * (float)a_delta;

            pos.x = glm::mix(pos.x, m_prevPos.x, 1 - scaledDelta.x);
            pos.y = glm::mix(pos.y, m_prevPos.y, 1 - scaledDelta.y);
        }

        const Object* object = GetObject();
        Transform* transform = object->GetTransform();
        glm::vec3 translation = transform->GetBaseTranslation();

        translation.x = pos.x;
        translation.y = pos.y;

        transform->SetTranslation(translation);

        transform->RefreshValues(0.0);

        m_prevPos = pos;
    }
}

void MouseTranslation::Update(double a_delta, Camera* a_camera)
{
    const Vec2KeyValue* value = m_offset->GetAnimValue();
    if (value != nullptr)
    {
        const Application* app = Application::GetInstance();    

        GLFWwindow* window = app->GetWindow();

        glm::vec<2, double> cursorPos;
        glm::ivec2 monPos;
        glm::ivec2 monSize;
        glm::ivec2 windowPos;

        glfwGetCursorPos(window, &cursorPos.x, &cursorPos.y);
        glfwGetWindowPos(window, &windowPos.x, &windowPos.y);
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        glfwGetMonitorWorkarea(monitor, &monPos.x, &monPos.y, &monSize.x, &monSize.y);

        const glm::vec2 monitorCPos = glm::vec2(windowPos.x + cursorPos.x, windowPos.y + cursorPos.y);
        const glm::vec2 cPos = glm::vec2(monitorCPos.x - monPos.x, monitorCPos.y - monPos.y);

        const glm::vec2 scalar = value->GetValue();

        m_mousePos = cPos - glm::vec2(scalar.x * monSize.x, scalar.y * monSize.y);
        
        UpdateTransform(false, a_delta, a_camera);
    }
}
void MouseTranslation::UpdatePreview(double a_delta, Camera* a_camera)
{
    const Vec2KeyValue* value = m_offset->GetValue();
    // Values have been initialised safe to transform object
    if (value != nullptr)
    {
        const Application* app = Application::GetInstance();    

        GLFWwindow* window = app->GetWindow();

        glm::vec<2, double> cursorPos;
        glm::ivec2 monPos;
        glm::ivec2 monSize;
        glm::ivec2 windowPos;

        glfwGetCursorPos(window, &cursorPos.x, &cursorPos.y);
        glfwGetWindowPos(window, &windowPos.x, &windowPos.y);
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        glfwGetMonitorWorkarea(monitor, &monPos.x, &monPos.y, &monSize.x, &monSize.y);

        const glm::vec2 monitorCPos = glm::vec2(windowPos.x + cursorPos.x, windowPos.y + cursorPos.y);
        const glm::vec2 cPos = glm::vec2(monitorCPos.x - monPos.x, monitorCPos.y - monPos.y);

        const glm::vec2 scalar = value->GetBaseValue();

        m_previewMousePos = cPos - glm::vec2(scalar.x * monSize.x, scalar.y * monSize.y);
        
        UpdateTransform(true, a_delta, a_camera);
    }
}
void MouseTranslation::UpdateGUI()
{
    Vec2KeyValue* scaleValue = m_mouseScale->GetValue();
    if (scaleValue != nullptr)
    {
        scaleValue->UpdateGUI("Mouse Scale");
    }

    Vec2KeyValue* deltaValue = m_delta->GetValue();
    if (deltaValue != nullptr)
    {
        deltaValue->UpdateGUI("Delta");
    }

    Vec2KeyValue* offsetValue = m_offset->GetValue();
    if (offsetValue != nullptr)
    {
        offsetValue->UpdateGUI("Offset");
    }
}

const char* MouseTranslation::ComponentName() const
{
    return COMPONENT_NAME;
}

void MouseTranslation::DisplayValues(bool a_value)
{
    m_mouseScale->SetDisplayState(a_value);
    m_delta->SetDisplayState(a_value);
    m_offset->SetDisplayState(a_value);
}

void MouseTranslation::Save(PropertyFile* a_propertyFile, PropertyFileProperty* a_parent) const
{
    PropertyFileProperty* property = a_propertyFile->InsertProperty();
    property->SetParent(a_parent);
    property->SetName(ComponentName());
}