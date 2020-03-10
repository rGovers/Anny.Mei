#pragma once

#include "Renderer.h"

#include "KeyValues/Vec2KeyValue.h"

class MorphPlaneDisplay;

class MorphPlaneRenderer : public Renderer
{
private:
    const static char* ITEMS[];

    MorphPlaneDisplay*         m_morphPlaneDisplay;

    bool                       m_animValuesDisplayed;

    AnimValue<StringKeyValue>* m_morphPlaneName;

    const char*                m_selectedMode;
    e_MorphRenderMode          m_renderMode;

    AnimValue<Vec2KeyValue>*   m_lerp;

    AnimValue<StringKeyValue>* m_northPlaneName;
    AnimValue<StringKeyValue>* m_southPlaneName;
    AnimValue<StringKeyValue>* m_eastPlaneName;
    AnimValue<StringKeyValue>* m_westPlaneName;

    AnimValue<StringKeyValue>* m_northEastPlaneName;
    AnimValue<StringKeyValue>* m_southEastPlaneName;
    AnimValue<StringKeyValue>* m_southWestPlaneName;
    AnimValue<StringKeyValue>* m_northWestPlaneName;

    void Draw(bool a_preview, double a_delta, Camera* a_camera);
protected:

public:
    const static char* COMPONENT_NAME;

    MorphPlaneRenderer(Object* a_object, AnimControl* a_animControl);
    virtual ~MorphPlaneRenderer();

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