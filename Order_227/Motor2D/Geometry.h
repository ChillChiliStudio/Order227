#ifndef GEOMETRY_H
#define GEOMETRY_H	//IMPROVE: Implement with std::vector?

#include "Point.h"
#include "Vector.h"

iVec2 GetVector2(iPoint A, iPoint B);
fVec2 GetVector2(fPoint A, fPoint B);
float GetDistance(fPoint A, fPoint B);

float RadsToDeg(float angle);
float DegToRads(float angle);

bool InsideRadius(fPoint center, float radius, fPoint point);
bool InsideSquareRadius(fPoint center, float radius, fPoint point);

#endif	//GEOMETRY_H