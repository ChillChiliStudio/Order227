#ifndef INFANTRY_H
#define INFANTRY_H

#include "Entity.h"
#include "Unit.h"
#include "SDL/include/SDL.h"

enum class infantry_type {

	INFANTRY_NONE = -1,
	BASIC,
	BAZOOKA,
	MACHINE_GUN,
	INFANTRY_MAX
};


class Infantry : public Unit {

public:

	Infantry(fPoint pos, infantry_type type, entity_faction faction = entity_faction::NEUTRAL);
	~Infantry() {}

	bool Move(float dt);
	bool Draw();

public:

	void UpdateBlitOrder() override;

public:

	SDL_Rect TestingRect = {0,0,78,66};

	//IMPLEMENT ANIMATIONS
	//animation* currentAnimation;
	unit_stats stats;
	infantry_type infantryType;
};

#endif

