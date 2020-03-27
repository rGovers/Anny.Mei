#pragma once

#include <glm/glm.hpp>

class Camera;
struct ModelData;
class ModelEditor;

enum class e_Axis;
enum class e_ToolMode;

class ModelEditorWindow
{
private:
    ModelEditor* m_modelEditor;

    e_ToolMode   m_toolMode;
    e_Axis       m_axis;

    bool         m_dragging;

    glm::vec2    m_startDragPos;
    glm::vec2    m_endDragPos;

    glm::vec2    m_lastMousePos;

    glm::vec2    m_lastPos;

    glm::vec2    m_selectionPoint;

    float        m_zoom;
    
    Camera*      m_camera;

    bool         m_mouseLastDown;

    bool         m_solid;
    bool         m_wireframe;
    bool         m_alpha;

    void SetSelectTool();
    void SetMoveTool();

    void MorphTargetDisplay(const char* a_name, glm::vec4* a_morphTarget);
protected:

public:
    ModelEditorWindow(ModelEditor* a_modelEditor);
    ~ModelEditorWindow();

    void ResetTools();

    void SetSelectionPoint(const glm::vec2& a_point);

    Camera* GetCamera() const;

    e_ToolMode GetToolMode() const;
    e_Axis GetAxis() const;

    float GetZoom() const;
    float GetMaxZoom() const;

    bool GetAlphaMode() const;
    bool GetSolidMode() const;
    bool GetWireFrameMode() const;

    void Update();
    void UpdatePropertiesWindow(const ModelData* a_modelData);
    void UpdateEditorWindow();
};