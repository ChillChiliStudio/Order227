#ifndef UNIT_H
#define UNIT_H

#include "Entity.h"

struct unit_stats
{
	int damage = 0;
	int healtPoints = 0;
	uint velocity = 0;
};

class Unit :public Entity
{
public:

	Unit(fPoint pos, entity_type Entitytype, entity_faction faction = entity_faction::FACTION_NEUTRAL):Entity(pos, type, faction){}
	~Unit();

	virtual bool Update(float dt) { return true; }
	virtual bool Move(float dt) { return true; }

	virtual bool Draw(float dt) { return true; }

public:

	//virtual void UpdateBlitOrder() {}

public:

	unit_stats stats;
};




#endif // !UNIT_H

