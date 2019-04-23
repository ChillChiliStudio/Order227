#ifndef BUILDING_H
#define BUILDING_H

#include "Entity.h"
#include "App.h"
#include "Animation.h"
#include "Timer.h"

#include "SDL/include/SDL.h"


struct SDL_Rect;

enum class building_type {

	BUILDING_NONE = -1,
	MAIN_BASE,
	TANK_FACTORY,
	RADAR,
	GOLDYARD,
	HTPC,
	PEOPLE_HEART,
	EPC,
	BUILDING_MAX
};


class Building :public Entity
{
public:

	Building(fPoint position, building_type baseType, entity_faction faction = entity_faction::NEUTRAL);
	~Building() {}

	bool Update(float dt);
	bool CleanUp();
	bool Start();

	bool Draw();
	void UpdateBlitOrder();



public:

	//ADD ANIMATION
	//Current_animation*
	SDL_Rect buildingBlitRect = {605, 1882, 212, 148 };
	building_type buildingType;


	
	float health = 0;
	int income = 0;
	Timer incomeTimer;

};
#endif 