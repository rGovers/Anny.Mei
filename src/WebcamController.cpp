#include "WebcamController.h"

#include <glad/glad.h>
#include <linux/videodev2.h>

#include "PBOTexture.h"
#include "RenderTexture.h"
#include "VirtualCamera.h"

WebcamController::WebcamController()
    : WebcamController(640, 480)
{
    
}
WebcamController::WebcamController(int a_width, int a_height) : 
    m_pboTexture(new PBOTexture(a_width, a_height, GL_BGR)), 
    m_virtCam(new VirtualCamera(a_width, a_height, V4L2_PIX_FMT_BGR24))
{
    m_renderTexture = new RenderTexture(a_width, a_height, GL_BGR);

    size_t size = m_virtCam->GetFrameSize();
    unsigned char* buffer = m_virtCam->GetVideoBuffer();

    for (int i = 0; i < size; i += 3)
    {
        buffer[i] = 0xFF;
        buffer[i + 1] = 0x00;
        buffer[i + 2] = 0x00;
    }
}
WebcamController::~WebcamController()
{
    delete m_virtCam;
    delete m_renderTexture;
}

void WebcamController::Bind()
{
    m_renderTexture->Bind();
}

void WebcamController::Update()
{
    m_renderTexture->Unbind();

    m_pboTexture->SwitchPBO();

    m_virtCam->PushFrame();
}