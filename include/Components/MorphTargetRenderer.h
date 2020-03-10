#pragma once

#include "Renderer.h"

#include "KeyValues/Vec2KeyValue.h"

class MorphTargetDisplay;

class MorphTargetRenderer : public Renderer
{
private:
    const static char* ITEMS[];

    MorphTargetDisplay*      m_morphTargetDisplay;

    bool                     m_animValuesDisplayed;

    const char*              m_selectedMode;
    e_MorphRenderMode        m_renderMode;

    AnimValue<Vec2KeyValue>* m_lerp;

    void Draw(bool a_preview, double a_delta, Camera* a_camera);
protected:

public:
    const static char* COMPONENT_NAME;

    MorphTargetRenderer(Object* a_object, AnimControl* a_animControl);
    virtual ~MorphTargetRenderer();

    virtual void Init();
    virtual void ObjectRenamed();

    virtual void Update(double a_delta, Camera* a_camera);
    virtual void UpdatePreview(double a_delta, Camera* a_camera);
    virtual void UpdateGUI();

    virtual const char* ComponentName() const;

    virtual void DisplayValues(bool a_value);

    virtual void Load(PropertyFileProperty* a_property, AnimControl* a_animControl);
    virtual void Save(PropertyFile* a_propertyFile, PropertyFileProperty* a_parent) const;
};