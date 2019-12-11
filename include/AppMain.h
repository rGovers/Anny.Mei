#pragma once

#include "Application.h"

class DataStore;
class ModelController;
class SkeletonEditor;
class TextureEditor;
class WebcamController;

class AppMain : public Application
{
private:
    DataStore*        m_dataStore;

    WebcamController* m_webcamController;
      
    ModelController*  m_modelController;

    SkeletonEditor*   m_skeletonEditor;
    TextureEditor*    m_textureEditor;

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