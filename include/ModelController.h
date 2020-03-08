#pragma once

#include <istream>

#include "miniz.h"

class Camera;
class ModelControllerWindow;
class Texture;
class WebcamController;
class Workspace;

class ModelController
{
private:
    ModelControllerWindow* m_window;

    Camera*                m_camera;

    Texture*               m_texture;
protected:

public:
    ModelController();
    ~ModelController();

    void DrawModel(const Workspace* a_workspace, double a_delta);
    void Update(const WebcamController& a_webcamController);

    static ModelController* Load(mz_zip_archive& a_archive);
    void Save(mz_zip_archive& a_archive) const;

    Texture* GetPreviewTexture() const;
};