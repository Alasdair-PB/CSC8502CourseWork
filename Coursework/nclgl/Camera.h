#pragma once
#include "Matrix4.h"
#include "Vector3.h"
#include "Pathing.h"
#include <vector>

class Camera
{
public:
	Camera(void) {
		yaw = 0.0f;
		pitch = 0.0f;
		path = new Pathing();
	};

	Camera(float pitch, float yaw, Vector3 position, float nearPlane = 1.0f, float farPlane = 1000.0f) {
		this->pitch = pitch;
		this->yaw = yaw;
		this->position = position;
		this->nearPlane = nearPlane;
		this->farPlane = farPlane;
		path = new Pathing();
	}

	~Camera(void) {};

	void UpdateCamera(float dt = 1.0f);
	Matrix4 BuildViewMatrix();

	Vector3 GetPosition() const { return position; }
	void SetPosition(Vector3 val) { position = val; }

	float GetYaw() const { return yaw; }
	void SetYaw(float y) { yaw = y; }

	float GetPitch() const { return pitch; }
	void SetPitch(float p) { pitch = p; }

	void SetTrack(bool state) { path->SetPathing(state);}

	float GetFarPlane() const { return farPlane; }
	void SetFarPlane(float val) { farPlane = val; }

	Pathing* GetPath() { return path; }


protected:

	void UpdateCameraFreeRoam(float dt);
	void UpdateCameraPathing(float dt);


	float yaw;
	float pitch;
	float nearPlane;
	float farPlane;
	Vector3 position;
	Pathing* path;

};