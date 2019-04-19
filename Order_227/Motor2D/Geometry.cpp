#include "Geometry.h"

Vector2<float> GetVector2(iPoint A, iPoint B)
{
	iPoint tmp = B - A;
	Vector2<float> vec((float)tmp.x, (float)tmp.y);
	return vec;
}

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

bool InsideRadius(fPoint center, float radius, fPoint point)	//Uses circle, Accurate, Slow
{
	bool ret = false;

	float distance = GetDistance(center, point);
	
	if (distance < radius) {
		ret = true;
	}

	return ret;
}

bool InsideSquareRadius(fPoint center, float radius, fPoint point)	//Uses square, Inaccurate, Fast
{
	bool ret = true;

	if (point.x > center.x + radius || point.x < center.x - radius
		|| point.y > center.y + radius || point.y < center.y + radius) {
		ret = false;
	}

	return ret;
}