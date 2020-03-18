#include "WebcamController.h"

#include <glad/glad.h>
#include <string.h>

#ifndef WIN32
#include <linux/videodev2.h>
#else

#endif // !WIN32

#include "PBOTexture.h"
#include "RenderTexture.h"
#include "VirtualCameras/DirectShowVirtualCamera.h"
#include "VirtualCameras/V4L2VirtualCamera.h"

WebcamController::WebcamController()
    : WebcamController(640, 480)
{
    
}
WebcamController::WebcamController(int a_width, int a_height)
{
	m_pboTexture = new PBOTexture(a_width, a_height, GL_BGR);

    m_renderTexture = new RenderTexture(m_pboTexture);

	m_virtCam = nullptr;

#ifndef WIN32
	m_virtCam = V4L2VirtualCamera::Create(a_width, a_height, V4L2_PIX_FMT_BGR24);
#else

#endif // !WIN32

	if (m_virtCam != nullptr)
	{
		size_t size = m_virtCam->GetFrameSize();
		unsigned char* buffer = m_virtCam->GetVideoBuffer();

		for (int i = 0; i < size; i += 3)
		{
			buffer[i] = 0xFF;
			buffer[i + 1] = 0x00;
			buffer[i + 2] = 0x00;
		}
	}
}
WebcamController::~WebcamController()
{
	if (m_virtCam != nullptr)
	{
		delete m_virtCam;
	}
    delete m_renderTexture;
}

Texture* WebcamController::GetTexture() const
{
    return m_pboTexture;
}

void WebcamController::Bind()
{
    m_renderTexture->Bind();
}

void WebcamController::Update()
{
	if (m_virtCam != nullptr)
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
	else
	{
		m_renderTexture->Unbind();
	}
}