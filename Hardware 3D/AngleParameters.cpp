#include "AngleParameters.h"

#include <DirectXMath.h>
using namespace DirectX;

void AngleParameters::Update(const AngleParameters& delta, float dt)
{
	Roll = XMScalarModAngle(Roll + delta.Roll * dt);
	Pitch = XMScalarModAngle(Pitch + delta.Pitch * dt);
	Yaw = XMScalarModAngle(Yaw + delta.Yaw * dt);
	Theta = XMScalarModAngle(Theta + delta.Theta * dt);
	Phi = XMScalarModAngle(Phi + delta.Phi * dt);
	Chi = XMScalarModAngle(Chi + delta.Chi * dt);
}
