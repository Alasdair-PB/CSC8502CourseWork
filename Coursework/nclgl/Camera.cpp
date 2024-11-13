#include "Camera.h"
#include "Window.h"
#include <algorithm>
#include "Matrix4.h"
#include <vector>

void Camera::UpdateCamera(float dt)
{
    if (path->ContinePath())
        UpdateCameraPathing(dt);
    else
        UpdateCameraFreeRoam(dt);
}

void Camera::UpdateCameraFreeRoam(float dt) 
{
    pitch -= (Window::GetMouse()->GetRelativePosition().y);
    yaw -= (Window::GetMouse()->GetRelativePosition().x);

    pitch = std::min(pitch, 90.0f);
    pitch = std::max(pitch, -90.0f);

    if (yaw < 0) yaw += 360.0f;
    if (yaw > 360.0f) yaw -= 360.0f;

    Matrix4 rotation = Matrix4::Rotation(yaw, Vector3(0, 1, 0));
    Vector3 forward = rotation * Vector3(0, 0, -1);
    Vector3 right = rotation * Vector3(1, 0, 0);
    float speed = 300.0f * dt;

    if (Window::GetKeyboard()->KeyDown(KEYBOARD_W)) position += forward * speed;
    if (Window::GetKeyboard()->KeyDown(KEYBOARD_S)) position -= forward * speed;
    if (Window::GetKeyboard()->KeyDown(KEYBOARD_A)) position -= right * speed;
    if (Window::GetKeyboard()->KeyDown(KEYBOARD_D)) position += right * speed;
    if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT)) position.y += speed;
    if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)) position.y -= speed;
}

Vector3 Lerp(const Vector3& start, const Vector3& end, float t) {
    return start + (end - start) * t;
}

void Camera::UpdateCameraPathing(float dt) {
    Vector3 lastPos = position;
    Vector3 targetPos = path->GetNextPosition(dt);

    position = Lerp(position, targetPos, dt);
    Vector3 viewDir = (lastPos - targetPos);

    viewDir.Normalise();

    float targetPitch = std::asin(viewDir.y) * (180.0f / PI);
    float targetYaw = std::atan2(viewDir.x, viewDir.z) * (180.0f / PI);

    if (targetYaw < 0)
        targetYaw += 360.0f;

    pitch = pitch + (targetPitch - pitch) * dt;
    yaw = yaw + (targetYaw - yaw) * dt;
}

Matrix4 Camera::BuildViewMatrix()
{
	return Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) * Matrix4::Rotation(-yaw, Vector3(0, 1, 0))
		* Matrix4::Translation(-position);
};


