#pragma once

#include <istream>
#include <ZipLib/ZipArchive.h>

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

    static ModelController* Load(ZipArchive::Ptr& a_archive);
    std::istream* SaveToStream() const;
};