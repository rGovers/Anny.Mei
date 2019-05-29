#pragma once

class PBOTexture;
class RenderTexture;
class VirtualCamera;

class WebcamController
{
private:
    VirtualCamera* m_virtCam;
    
    PBOTexture*    m_pboTexture;
    RenderTexture* m_renderTexture;

protected:

public:
    WebcamController();
    WebcamController(int a_width, int a_height);
    ~WebcamController();

    void Bind();

    void Update();
};