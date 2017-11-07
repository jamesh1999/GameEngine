#include "Camera.h"
#include "GraphicsController.h"

Camera::Camera()
{
	GraphicsController::instance->SetCamera(this);
}