#pragma once

#include <istream>

#include "miniz.h"

class WebcamController;

class ModelController
{
private:
    float* m_backgroundColor;
protected:

public:
    ModelController();
    ~ModelController();

    void DrawModel();
    void Update(double a_delta, const WebcamController& a_webcamController);

    static ModelController* Load(mz_zip_archive& a_archive);
    void Save(mz_zip_archive& a_archive) const;
};