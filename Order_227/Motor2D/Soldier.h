#ifndef SOLDIER_H
#define SOLDIER_H

#include "Entity.h"
#include "Unit.h"
#include "SDL/include/SDL.h"

enum class soldier_type {

	SOLDIER_NONE = -1,
	BASIC,
	BAZOOKA,
	MACHINE_GUN,
	SOLDIER_MAX
};


class Soldier : public Unit {

public:

	Soldier(fPoint pos, soldier_type type, entity_faction faction = entity_faction::NEUTRAL);
	~Soldier() {}

	bool Update(float dt);
	bool Move(float dt);
	bool Draw();

public:

	void UpdateBlitOrder() override;

public:

	//IMPLEMENT ANIMATIONS
	//animation* currentAnimation;
	unit_stats stats;
	soldier_type soldierType;
};

#endif

