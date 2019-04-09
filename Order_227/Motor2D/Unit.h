#ifndef UNIT_H
#define UNIT_H

#include "Entity.h"
#include "SDL/include/SDL.h"

struct unit_stats
{
	int damage=0;
	int healtPoints=0;
	uint velocity=0;
};

class Unit : public Entity {

public:

	Unit(fPoint pos,entity_type type, entity_faction faction = entity_faction::FACTION_NEUTRAL);
	~Unit();

	bool Update(float dt);
	bool Move(float dt);
	bool Draw();

public:

	void UpdateBlitOrder()override;

public:

	unit_stats stats;
};

#endif
