#pragma once

#include <istream>

#include "miniz.h"

class Camera;
class SkeletonEditor; 
class WebcamController;

class ModelController
{
private:
    float*  m_backgroundColor;

    Camera* m_camera;
protected:

public:
    ModelController();
    ~ModelController();

    void DrawModel(const SkeletonEditor* a_skeletonEditor, double a_delta);
    void Update(const WebcamController& a_webcamController);

    static ModelController* Load(mz_zip_archive& a_archive);
    void Save(mz_zip_archive& a_archive) const;
};