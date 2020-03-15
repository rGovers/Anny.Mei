#pragma once

#include "MorphPlaneRenderer.h"

#include "KeyValues/Vec2KeyValue.h"

class DepthRenderTexture;

class MorphPlaneMask : public MorphPlaneRenderer
{
private:
    bool                m_animValuesDisplayed;

    char*               m_maskName;

    DepthRenderTexture* m_renderTexture;

    void Draw(bool a_preview, double a_delta, Camera* a_camera);
protected:

public:
    const static char* COMPONENT_NAME;

    MorphPlaneMask(Object* a_object, AnimControl* a_animControl);
    virtual ~MorphPlaneMask();

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