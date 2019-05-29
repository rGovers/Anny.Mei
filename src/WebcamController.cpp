#include "WebcamController.h"

#include <glad/glad.h>
#include <linux/videodev2.h>
#include <string.h>

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
    m_renderTexture = new RenderTexture(m_pboTexture);

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
    unsigned char* pixels = (unsigned char*)m_pboTexture->MapPixels();
    if (pixels)
    {
        unsigned char* buffer = m_virtCam->GetVideoBuffer();

        memcpy(buffer, pixels, m_pboTexture->GetWidth() * m_pboTexture->GetHeight() * 3);

        m_pboTexture->UnmapPixels();
    }
    m_pboTexture->Unbind();

    m_renderTexture->Unbind();
    
    m_virtCam->PushFrame();
}