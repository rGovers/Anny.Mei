#pragma once

#include "Application.h"

class WebcamController;

class AppMain : public Application
{
private:
    WebcamController* m_webcamController;
    
protected:
    virtual void Update(double a_delta);

public:
    AppMain() = delete;
    AppMain(int a_width, int a_height);
    ~AppMain();
};