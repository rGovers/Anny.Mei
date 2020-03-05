#pragma once

#include "AnimValue.h"
#include "Component.h"
#include "KeyValues/StringKeyValue.h"
#include "KeyValues/Vec3KeyValue.h"

#include <glm/glm.hpp>

class PropertyFileProperty;

class Renderer : public Component
{
private:
    AnimValue<StringKeyValue>* m_modelName;

    AnimValue<Vec3KeyValue>*   m_anchor;
protected:
    void UpdateRendererGUI();

    PropertyFileProperty* SaveValues(PropertyFile* a_propertyFile, PropertyFileProperty* a_parent) const;

    const char* GetModelName() const;
    const char* GetBaseModelName() const;

    glm::vec3 GetAnchor() const;
    glm::vec3 GetBaseAnchor() const;

    void InitValues();
    void RenameValues();

    void DisplayRendererValues(bool a_value);
public:
    Renderer(Object* a_object, AnimControl* a_animControl);
    virtual ~Renderer();

    virtual void Init();

    virtual void Update(double a_delta, Camera* a_camera) = 0;
    virtual void UpdatePreview(double a_delta, Camera* a_camera) = 0;
    virtual void UpdateGUI() = 0;

    virtual void ObjectRenamed();

    virtual const char* ComponentName() const;

    virtual void DisplayValues(bool a_value);

    virtual void Load(PropertyFileProperty* a_property, AnimControl* a_animControl) = 0;
    virtual void Save(PropertyFile* a_propertyFile, PropertyFileProperty* a_parent) const = 0;
};