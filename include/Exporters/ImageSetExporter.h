#pragma once

#include <glm/glm.hpp>

class ModelController;
class Workspace;

class ImageSetExporter
{
private:
    char*            m_path;
    ModelController* m_modelController;
    Workspace*       m_workspace;

    bool             m_finished;

    bool             m_init;

    glm::ivec2       m_internalResolution;
    int              m_frameRate;
protected:

public:
    ImageSetExporter() = delete;
    ImageSetExporter(const char* a_path, ModelController* a_modelController, Workspace* a_workspace);
    ~ImageSetExporter();

    bool IsFinished() const;

    void Update();
};