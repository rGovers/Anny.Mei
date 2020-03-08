#pragma once

#include <glm/glm.hpp>
#include <istream>
#include <map>

#include "EditorController.h"
#include "miniz.h"

class AnimControl;
class IntermediateRenderer;
class Namer;
class Object;
class PropertyFile;
class PropertyFileProperty;
class RenderTexture;
class SkeletonEditorWindow;
class Texture;
class Workspace;

class SkeletonEditor : public EditorController
{
private:
    SkeletonEditorWindow* m_window;

    Workspace*            m_workspace;

    Namer*                m_namer;

    Object*               m_baseObject;
    Object*               m_selectedObject;

    IntermediateRenderer* m_imRenderer;
    RenderTexture*        m_renderTexture;

    AnimControl*          m_animControl;

    double                m_delta;

    void LoadObject(Object* a_object, PropertyFileProperty* a_property);
    void SaveObject(PropertyFile* a_propertyFile, PropertyFileProperty* a_parent, Object* a_object) const;
protected:

public:
    SkeletonEditor() = delete;
    SkeletonEditor(Workspace* a_workspace);
    ~SkeletonEditor();

    void ListObjects(Object* a_object, int& a_node);

    Object* GetBaseObject() const;

    void DrawObjectDetail(Object* a_object) const;

    void Update(double a_delta);

    const Texture* DrawEditor();

    static SkeletonEditor* Load(mz_zip_archive& a_archive, Workspace* a_workspace);
    void Save(mz_zip_archive& a_archive) const;

    virtual void DrawPropertiesWindow();
    virtual void DrawEditorWindow();
};