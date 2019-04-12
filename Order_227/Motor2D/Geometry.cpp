#include <cmath>
#include "Geometry.h"

fPoint GetVector(fPoint A, fPoint B)
{
	return { B.x - A.x, B.y - A.y };
}

float GetDistance(fPoint A, fPoint B)
{
	return sqrt(pow((A.x - A.x), 2.0f) + pow((B.y - B.y), 2.0f));
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