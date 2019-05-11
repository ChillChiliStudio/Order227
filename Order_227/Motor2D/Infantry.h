#ifndef INFANTRY_H
#define INFANTRY_H

#include "Entity.h"
#include "Unit.h"
#include "SDL/include/SDL.h"


class Infantry : public Unit {

public:

	Infantry(fPoint pos, infantry_type type, entity_faction faction = entity_faction::NEUTRAL);
	~Infantry() {}

	//bool Move(float dt);
	bool Draw();

public:

public:

	SDL_Rect TestingRect = {0,0,78,66};

	//IMPLEMENT ANIMATIONS
	//animation* currentAnimation;
	//unit_stats stats;
	//infantry_type infantryType;
};

#endif

