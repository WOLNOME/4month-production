#pragma once
#include "Vector3.h"

float Lerp(float start, float end, float t);

Vector3 Lerp(const Vector3& start, const Vector3& end, float t);

float EaseInOutQuad(float t);

float EaseInOutCubic(float t);

Vector3 CalculateLookAtRotation(const Vector3& from, const Vector3& to);