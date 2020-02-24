#pragma once

#include <istream>

#include "miniz.h"

class Camera;
class ModelControllerWindow;
class SkeletonEditor; 
class Texture;
class WebcamController;

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

    void DrawModel(const SkeletonEditor* a_skeletonEditor, double a_delta);
    void Update(const WebcamController& a_webcamController);

    static ModelController* Load(mz_zip_archive& a_archive);
    void Save(mz_zip_archive& a_archive) const;

    Texture* GetPreviewTexture() const;
};