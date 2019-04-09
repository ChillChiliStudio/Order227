#ifndef UNIT_H
#define UNIT_H

#include "Entity.h"
#include "SDL/include/SDL.h"

struct unit_stats
{
	int damage;
	int healtPoints;
	unit_faction faction;
	uint velocity;
};

enum class unit_faction {
	FACTION_NONE = -1,
	FACTION_URSS,
	FACTION_US,
	FACTION_NEUTRAL,
	FACTION_MAX
};

class Unit : public Entity {

public:

	Unit(fPoint pos, unit_faction faction);
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
