#include "Camera.h"
#include "GraphicsController.h"

void Camera::Create()
{
	engine->graphics->SetCamera(this);
}