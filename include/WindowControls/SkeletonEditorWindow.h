#pragma once

#include <glm/glm.hpp>

class Camera;
class Object;
class SkeletonEditor;

class SkeletonEditorWindow
{
private:
    SkeletonEditor* m_skeletonEditor;

    Camera*         m_camera;

    Object*         m_selectedObject;

    glm::vec2       m_lastMousePos;

    float           m_zoom;

    void ListObjects(Object* a_object, int& a_node);

protected:

public:
    SkeletonEditorWindow(SkeletonEditor* a_skeletonEditor);
    ~SkeletonEditorWindow();

    Camera* GetCamera() const;

    Object* GetSelectedObject() const;

    void Update();
    void UpdatePropertiesWindow(Object* a_selectedObject);
    void UpdateEditorWindow();
};
