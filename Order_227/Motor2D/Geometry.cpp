#include "Geometry.h"

Vector2<float> GetVector2(fPoint A, fPoint B)
{
	fPoint tmp = B - A;
	Vector2<float> vec(tmp.x, tmp.y);
	return vec;
}

float GetDistance(fPoint A, fPoint B)
{
	Vector2<float> vec = GetVector2(A, B);
	return (float)vec.GetMagnitude();
}

bool InsideRadius(fPoint center, float radius, fPoint point)
{
	bool ret = false;

	float distance = GetDistance(center, point);
	
	if (distance < radius) {
		ret = true;
	}

	return ret;
}