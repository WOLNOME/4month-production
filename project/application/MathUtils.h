#pragma once
#include "Vector3.h"

float EaseInOutQuad(float t);

Vector3 CalculateLookAtRotation(const Vector3& from, const Vector3& to);