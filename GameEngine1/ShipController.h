#ifndef __SHIP_CONTROLLER_INCLUDED__
#define __SHIP_CONTROLLER_INCLUDED__

#include <DirectXMath.h>
#include "CompositeObject.h"
#include "Script.h"

class ShipController : public Script
{
private:
	GameEngine::Elements::CompositeObject* cam;
	GameEngine::Elements::CompositeObject* base;
	GameEngine::Elements::CompositeObject* model;
	GameEngine::Elements::CompositeObject* light;

	DirectX::XMFLOAT3 curNorm;
	DirectX::XMFLOAT3 facingDirection;
	DirectX::XMFLOAT3 velocity;
	float drag = 0.18f;
	float acceleration = 20.0f;
	float vertSpeed = 80.0f;
	float alignSpeed = 0.03f;
	float rollSpeed = 0.06f;
	float rollMax = 0.4f;

	float curRoll = 0.0f;
	bool overTri = false;

	void UpdateBase();

public:
	void Update() override;
	void Create() override;
	ShipController();
	~ShipController();
};

#endif
