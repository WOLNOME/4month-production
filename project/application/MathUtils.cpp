#include "MathUtils.h"

float EaseInOutQuad(float t)
{
	return t < 0.5 ? 2 * t * t : -1 + (4 - 2 * t) * t;
}

Vector3 CalculateLookAtRotation(const Vector3& from, const Vector3& to)
{
	Vector3 direction = to - from;
	direction.Normalize();
	float pitch = std::asin(-direction.y);
	float yaw = std::atan2(direction.x, direction.z);
	return Vector3(pitch, yaw, 0.0f);
}