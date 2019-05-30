#pragma once

#include "Application.h"

class ModelEditor;
class WebcamController;

class AppMain : public Application
{
private:
    WebcamController* m_webcamController;
    
    ModelEditor*      m_modelEditor;

    float             m_backgroundColor[3] = { 0, 1, 0 };

    bool*             m_menuState;

protected:
    virtual void Update(double a_delta);

public:
    AppMain() = delete;
    AppMain(int a_width, int a_height);
    ~AppMain();
};