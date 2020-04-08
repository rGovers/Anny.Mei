#pragma once

#include "Renderer.h"

#include "KeyValues/IntKeyValue.h"
#include "KeyValues/Vec2KeyValue.h"

class MorphPlaneDisplay;

class MorphPlaneRenderer : public Renderer
{
private:
    const static char* ITEMS[];

    MorphPlaneDisplay*         m_morphPlaneDisplay;

    Workspace*                 m_workSpace;

    bool                       m_animValuesDisplayed;

    AnimValue<StringKeyValue>* m_morphPlaneName;

    bool                       m_editingMorphPlane;

    const char*                m_selectedMode;
    AnimValue<IntKeyValue>*    m_renderMode;

    AnimValue<Vec2KeyValue>*   m_lerp;

    AnimValue<StringKeyValue>* m_northPlaneName;
    AnimValue<StringKeyValue>* m_southPlaneName;
    AnimValue<StringKeyValue>* m_eastPlaneName;
    AnimValue<StringKeyValue>* m_westPlaneName;

    AnimValue<StringKeyValue>* m_northEastPlaneName;
    AnimValue<StringKeyValue>* m_southEastPlaneName;
    AnimValue<StringKeyValue>* m_southWestPlaneName;
    AnimValue<StringKeyValue>* m_northWestPlaneName;

    void UpdateMorphPlaneGUI(const char* a_label, AnimValue<StringKeyValue>* a_morphPlane);

protected:
    void MorphPlaneDraw(bool a_preview, double a_delta, Camera* a_camera);

    void MorphPlaneInit();
    void MorphPlaneObjectRenamed();

    void MorphPlaneUpdateGUI(Workspace* a_workspace);

    void MorphPlaneDisplayValues(bool a_value);
public:
    const static char* COMPONENT_NAME;

    MorphPlaneRenderer(Object* a_object, AnimControl* a_animControl);
    virtual ~MorphPlaneRenderer();

    virtual void Init();
    virtual void ObjectRenamed();

    virtual void Update(double a_delta, Camera* a_camera);
    virtual void UpdatePreview(double a_delta, Camera* a_camera);
    virtual void UpdateGUI(Workspace* a_workspace);

    virtual const char* ComponentName() const;

    virtual void DisplayValues(bool a_value);
};