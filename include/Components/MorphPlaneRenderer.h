#pragma once

#include "Renderer.h"

class MorphPlaneDisplay;

class MorphPlaneRenderer : public Renderer
{
private:
    MorphPlaneDisplay*         m_morphPlaneDisplay;

    AnimValue<StringKeyValue>* m_morphPlaneName;

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

    virtual void Load(PropertyFileProperty* a_property, AnimControl* a_animControl);
    virtual void Save(PropertyFile* a_propertyFile, PropertyFileProperty* a_parent) const;
};