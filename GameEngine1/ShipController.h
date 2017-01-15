#ifndef __SHIP_CONTROLLER_INCLUDED__
#define __SHIP_CONTROLLER_INCLUDED__

#include "Script.h"
#include "CompositeObject.h"
#include "Transform.h"
#include <DirectXMath.h>

class ShipController : public Script
{
private:
	CompositeObject cam;
	CompositeObject base;
	CompositeObject model;

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