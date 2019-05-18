#include "Geometry.h"

iVec2 GetVector2(iPoint A, iPoint B)
{
	iPoint tmp = B - A;
	return { tmp.x, tmp.y };
}

fVec2 GetVector2(fPoint A, fPoint B)
{
	fPoint tmp = B - A;
	return { tmp.x, tmp.y };
}

float GetDistance(iPoint A, iPoint B)
{
	iVec2 vec = GetVector2(A, B);
	return (float)vec.GetMagnitude();
}

float GetDistance(fPoint A, fPoint B)
{
	fVec2 vec = GetVector2(A, B);
	return vec.GetMagnitude();
}

float RadsToDeg(float angle)
{
	return angle * 180.0f / PI;
}

float DegToRads(float angle)
{
	return angle * PI / 180.0f;
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

	if (point.x > center.x + radius || point.x < center.x - radius || point.y > center.y + radius || point.y < center.y - radius) {
		ret = false;
	}

	return ret;
}