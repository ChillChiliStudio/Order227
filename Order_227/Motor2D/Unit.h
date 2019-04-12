#ifndef UNIT_H
#define UNIT_H

#include "Entity.h"

struct unit_stats
{
	int speed =0;
	int damage = 0;
	int healtPoints = 0;
	int visionRange;
	uint velocity = 0;
};

class Unit :public Entity
{
public:

	Unit(fPoint pos, entity_type Entitytype, entity_faction faction = entity_faction::NEUTRAL):Entity(pos, type, faction){}
	~Unit();

	virtual bool Move(float dt) { return true; }

public:

	unit_stats stats;
};




#endif // !UNIT_H

