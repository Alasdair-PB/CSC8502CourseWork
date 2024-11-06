#pragma once
#include "Matrix4.h"
#include "Vector3.h"

class Camera
{
public:
	Camera(void) {
		yaw = 0.0f;
		pitch = 0.0f;
	};

	Camera(float pitch, float yaw, Vector3 position, float nearPlane = 1.0f, float farPlane = 1000.0f) {
		this->pitch = pitch;
		this->yaw = yaw;
		this->position = position;
		this->nearPlane = nearPlane;
		this->farPlane = farPlane;
	}

	~Camera(void) {};

	void UpdateCamera(float dt = 1.0f);
	Matrix4 BuildViewMatrix();

	Vector3 GetPosition() const { return position; }
	void SetPosition(Vector3 val) { position = val; }

	float GetYaw() const { return yaw; }
	void SetYaw(float y) { yaw = y; }

	float GetFarPlane() const { return farPlane; }
	void SetFarPlane(float val) { farPlane = val; }

	float GetPitch() const { return pitch; }
	void SetPitch(float p) { pitch = p; }


protected:
	float yaw;
	float pitch;
	float nearPlane; 
	float farPlane;
	Vector3 position; // Set to 0 ,0 ,0 by Vector3 constructor ;)
};