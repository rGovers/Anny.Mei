#pragma once

#include "Application.h"

class DataStore;
class ModelController;
class ModelEditor;
class SkeletonEditor;
class TextureEditor;
class WebcamController;

class AppMain : public Application
{
private:
    DataStore*            m_dataStore;
      
    WebcamController*     m_webcamController;
          
    ModelController*      m_modelController;
      
    SkeletonEditor*       m_skeletonEditor;
    TextureEditor*        m_textureEditor;
    ModelEditor*          m_modelEditor;
    
    const char*           m_filePath;

    double                m_windowUpdateTimer;

    bool                  m_resetWindows;

    void New();
    void Open();
    void Save() const;
    void SaveAs();

    void Input();

    void ResetDockedWindows();
protected:
    virtual void Update(double a_delta);

    virtual void Resize(int a_newWidth, int a_newHeight);

public:
    AppMain() = delete;
    AppMain(int a_width, int a_height);
    ~AppMain();
};