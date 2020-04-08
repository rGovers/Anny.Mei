#include "Components/MouseScale.h"

#include <string>

#include "Application.h"
#include "imgui.h"
#include "Object.h"
#include "Transform.h"

const char* MouseScale::COMPONENT_NAME = "MouseScale";

MouseScale::MouseScale(Object* a_object, AnimControl* a_animControl) :
    Component(a_object, a_animControl)
{
    m_prevScale = glm::vec2(1);
    m_prevPreviewScale = glm::vec2(1);

    m_mousePos = glm::vec2(0);
    m_previewMousePos = glm::vec2(0);
}
MouseScale::~MouseScale()
{
    delete m_mouseScale;
    delete m_delta;
    delete m_offset;
}

void MouseScale::Init()
{
    const Object* object = GetObject();
    AnimControl* animControl = GetAnimControl();

    const std::string baseName = std::string("[") + object->GetName() + "] [" + this->ComponentName() + "] ";

    m_mouseScale = new AnimValue<Vec2KeyValue>((baseName + "Mouse Scale").c_str(), animControl);
    m_mouseScale->SelectKeyFrame(0);
    m_mouseScale->GetValue()->SetBaseValue(glm::vec2(1));

    m_delta = new AnimValue<Vec2KeyValue>((baseName + "Delta").c_str(), animControl);
    m_delta->SelectKeyFrame(0);
    m_delta->GetValue()->SetBaseValue(glm::vec2(1));

    m_offset = new AnimValue<Vec2KeyValue>((baseName + "Offset").c_str(), animControl);
    m_offset->SelectKeyFrame(0);
    m_offset->GetValue()->SetBaseValue(glm::vec2(0.5f));
}
void MouseScale::ObjectRenamed()
{
    Object* object = GetObject();

    const std::string baseName = std::string("[") + object->GetName() + "] [" + this->ComponentName() + "] ";
    
    m_mouseScale->Rename((baseName + "Mouse Scale").c_str());
    m_delta->Rename((baseName + "Delta").c_str());
    m_offset->Rename((baseName + "Offset").c_str());
}

void MouseScale::ResetFloat(float& a_value)
{
    const float abs = glm::abs(a_value);

    if (abs == std::numeric_limits<float>::infinity() || abs != abs)
    {
        a_value = 1;
    }
}

void MouseScale::UpdateTransform(bool a_preview, double a_delta, Camera* a_camera)
{
    glm::vec2 scale;

    // Error resolving for when user enters invalid values
    ResetFloat(m_prevScale.x);
    ResetFloat(m_prevScale.y);
    ResetFloat(m_prevPreviewScale.x);
    ResetFloat(m_prevPreviewScale.y);

    if (a_preview)
    {   
        const Vec2KeyValue* mouseScaleValue = m_mouseScale->GetValue();

        if (mouseScaleValue != nullptr)
        {
            const glm::vec2 mScale = mouseScaleValue->GetBaseValue() * 0.0001f;

            scale = glm::vec2(1 - glm::abs(m_previewMousePos.x * mScale.x), 1 - glm::abs(m_previewMousePos.y * mScale.y));
        }

        const Vec2KeyValue* deltaValue = m_delta->GetValue();
        if (deltaValue != nullptr)
        {
            const glm::vec2 delta = deltaValue->GetBaseValue();
            const glm::vec2 scaledDelta = delta * (float)a_delta;

            scale.x = glm::mix(scale.x, m_prevPreviewScale.x, 1 - scaledDelta.x);
            scale.y = glm::mix(scale.y, m_prevPreviewScale.y, 1 - scaledDelta.y);
        }

        const Object* object = GetObject();
        Transform* transform = object->GetTransform();
        glm::vec3 tScale = transform->GetBaseScale();

        tScale.x = scale.x;
        tScale.y = scale.y;

        transform->SetScale(tScale);

        transform->RefreshValues(0.0);

        m_prevPreviewScale = scale;
    }
    else
    {
        const Vec2KeyValue* mouseScaleValue = m_mouseScale->GetAnimValue();

        if (mouseScaleValue != nullptr)
        {
            const glm::vec2 mScale = mouseScaleValue->GetValue() * 0.0001f;

            scale = glm::vec2(1 - glm::abs(m_mousePos.x * mScale.x), 1 - glm::abs(m_mousePos.y * mScale.y));
        }

        const Vec2KeyValue* deltaValue = m_delta->GetAnimValue();
        if (deltaValue != nullptr)
        {
            const glm::vec2 delta = deltaValue->GetValue();
            const glm::vec2 scaledDelta = delta * (float)a_delta;

            scale.x = glm::mix(scale.x, m_prevScale.x, 1 - scaledDelta.x);
            scale.y = glm::mix(scale.y, m_prevScale.y, 1 - scaledDelta.y);
        }

        const Object* object = GetObject();
        Transform* transform = object->GetTransform();
        glm::vec3 tScale = transform->GetBaseTranslation();

        tScale.x = scale.x;
        tScale.y = scale.y;

        transform->SetScale(tScale);

        transform->RefreshValues(0.0);

        m_prevScale = scale;
    }
}

void MouseScale::Update(double a_delta, Camera* a_camera)
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
void MouseScale::UpdatePreview(double a_delta, Camera* a_camera)
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
void MouseScale::UpdateGUI(Workspace* a_workspace)
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

const char* MouseScale::ComponentName() const
{
    return COMPONENT_NAME;
}

void MouseScale::DisplayValues(bool a_value)
{
    m_mouseScale->SetDisplayState(a_value);
    m_delta->SetDisplayState(a_value);
    m_offset->SetDisplayState(a_value);
}

void MouseScale::Save(PropertyFile* a_propertyFile, PropertyFileProperty* a_parent) const
{
    PropertyFileProperty* property = a_propertyFile->InsertProperty();
    property->SetParent(a_parent);
    property->SetName(ComponentName());
}