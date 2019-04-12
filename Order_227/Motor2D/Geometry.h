#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "Point.h"

fPoint GetVector(fPoint A, fPoint B);
float GetDistance(fPoint A, fPoint B);
bool InsideRadius(fPoint center, float radius, fPoint point);

#endif	//GEOMETRY_H