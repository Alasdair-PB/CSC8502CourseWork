#pragma once
#include "Matrix4.h"
#include "Vector3.h"
#include <vector>

struct BezierCurve
{
	Vector3 p0, p1, p2, p3;

	Vector3 GetPoint(float t) const {
		float u = 1 - t;
		float tt = t * t;
		float uu = u * u;
		float uuu = uu * u;
		float ttt = tt * t;

		Vector3 point = p0 * uuu;
		point += p1 * 3 * uu * t;
		point += p2 * 3 * u * tt;
		point += p3 * ttt;
		return point;
	}
};



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

		followingPath = true;
		bezierT = 0.0f;
		currentCurveIndex = 0;
		bezierSpeed = 0.1f;	
		bezierPath = std::vector<BezierCurve>();

		bezierPath.push_back(BezierCurve{
			position + Vector3(0, 0, 0),
			position + Vector3(10, 10, 0),
			position + Vector3(20, 10, 0),
			position + Vector3(30, 0, 0)
			});
		bezierPath.push_back(BezierCurve{
			position + Vector3(30, 0, 0),
			position + Vector3(40, -10, 0),
			position + Vector3(50, -10, 0),
			position + Vector3(60, 0, 0)
			});
		bezierPath.push_back(BezierCurve{
			position + Vector3(60, 0, 0),
			position + Vector3(70, 10, 0),
			position + Vector3(80, 10, 0),
			position + Vector3(90, 0, 0)
			});


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

	float GetFarPlane() const { return farPlane; }
	void SetFarPlane(float val) { farPlane = val; }


protected:


	float yaw;
	float pitch;
	float nearPlane;
	float farPlane;
	Vector3 position;
	std::vector <BezierCurve> bezierPath;
	bool followingPath = true;
	float bezierT = 0.0f;
	int currentCurveIndex = 0;
	float bezierSpeed = 0.1f;
};