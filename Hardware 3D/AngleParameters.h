#pragma once

struct AngleParameters
{
	void Update(const AngleParameters& delta, float dt);

	float Roll;
	float Pitch;
	float Yaw;
	float Theta;
	float Phi;
	float Chi;
};
