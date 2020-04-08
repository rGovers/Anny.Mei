#pragma one

#include "Components/Component.h"

#include "AnimValue.h"
#include "KeyValues/Vec2KeyValue.h"

class MouseScale : public Component
{
private:
    AnimValue<Vec2KeyValue>* m_mouseScale;
    AnimValue<Vec2KeyValue>* m_delta;
    AnimValue<Vec2KeyValue>* m_offset;

    glm::vec2                m_prevScale;
    glm::vec2                m_prevPreviewScale;

    glm::vec2                m_mousePos;
    glm::vec2                m_previewMousePos;

    void ResetFloat(float& a_value);

protected:
    void UpdateTransform(bool a_preview, double a_delta, Camera* a_camera);

public:
    const static char* COMPONENT_NAME;

    MouseScale() = delete;
    MouseScale(Object* a_object, AnimControl* a_animControl);
    virtual ~MouseScale();

    virtual void Init();
    virtual void ObjectRenamed();

    virtual void Update(double a_delta, Camera* a_camera);
    virtual void UpdatePreview(double a_delta, Camera* a_camera);
    virtual void UpdateGUI(Workspace* a_workspace);

    virtual const char* ComponentName() const;

    virtual void DisplayValues(bool a_value);

    virtual void Save(PropertyFile* a_propertyFile, PropertyFileProperty* a_parent) const;
};