#include "MathUtils.h"

float Lerp(float start, float end, float t)
{
	return start + (end - start) * t;
}

Vector3 Lerp(const Vector3& start, const Vector3& end, float t)
{
	Vector3 result;
	result.x = start.x + (end.x - start.x) * t;
	result.y = start.y + (end.y - start.y) * t;
	result.z = start.z + (end.z - start.z) * t;
	return result;
}

float EaseInOutQuad(float t)
{
	return t < 0.5 ? 2 * t * t : -1 + (4 - 2 * t) * t;
}

float EaseInOutCubic(float t)
{
	return t < 0.5 ? 4 * t * t * t : 1 + (t - 1) * (4 * t - 4);
}

Vector3 CalculateLookAtRotation(const Vector3& from, const Vector3& to)
{
	Vector3 direction = to - from;
	direction.Normalize();
	float pitch = std::asin(-direction.y);
	float yaw = std::atan2(direction.x, direction.z);
	return Vector3(pitch, yaw, 0.0f);
}
