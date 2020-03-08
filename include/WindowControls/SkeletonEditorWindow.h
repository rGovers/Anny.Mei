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

    glm::vec2       m_lastMousePos;

    float           m_zoom;

protected:

public:
    SkeletonEditorWindow(SkeletonEditor* a_skeletonEditor);
    ~SkeletonEditorWindow();

    Camera* GetCamera() const;

    void Update();
    void UpdatePropertiesWindow(Object* a_selectedObject);
    void UpdateEditorWindow();
};
