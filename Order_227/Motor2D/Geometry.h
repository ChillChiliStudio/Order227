#ifndef GEOMETRY_H
#define GEOMETRY_H	//IMPROVE: Implement with std::vector?

#include "Point.h"
#include "Vector.h"

Vector2<float> GetVector2(fPoint A, fPoint B);
float GetDistance(fPoint A, fPoint B);

bool InsideRadius(fPoint center, float radius, fPoint point);

#endif	//GEOMETRY_H