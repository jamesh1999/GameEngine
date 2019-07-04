#include "Camera.h"
#include "GraphicsController.h"

using namespace GameEngine::Graphics;

void Camera::Create()
{
	GetEngine()->graphics->SetCamera(this);
}

void Camera::Draw()
{
	// Bind and render with this camera
	GetEngine()->graphics->m_camera = this;
	//GetEngine()->graphics->Draw();
}