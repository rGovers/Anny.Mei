#pragma once

#include "Application.h"

class ModelController;
class ModelEditor;
class WebcamController;

class AppMain : public Application
{
private:
    WebcamController* m_webcamController;
    
    ModelController*  m_modelController;
    ModelEditor*      m_modelEditor;


    bool*             m_menuState;

    const char*       m_filePath;

    void New();
    void Open();
    void Save() const;
    void SaveAs();

    void Input();
protected:
    virtual void Update(double a_delta);

public:
    AppMain() = delete;
    AppMain(int a_width, int a_height);
    ~AppMain();
};