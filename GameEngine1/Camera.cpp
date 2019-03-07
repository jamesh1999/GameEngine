#include "Camera.h"
#include "GraphicsController.h"

using namespace GameEngine::Graphics;

void Camera::Create()
{
	engine->graphics->SetCamera(this);
}

void Camera::Draw()
{
	// Bind and render with this camera
	engine->graphics->m_camera = this;
	//engine->graphics->Draw();
}