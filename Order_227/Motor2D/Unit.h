#ifndef UNIT_H
#define UNIT_H

#include "Entity.h"
#include "SDL/include/SDL.h"

enum class unit_type {

	INFANTRY_DIVISION,
	UNKNOWN = 1
};

enum class faction_enum {

	FACTION_COMMUNIST,
	FACTION_CAPITALIST,
	UNKNOWN = 2
};


class Unit : public Entity {

public:

	Unit(unit_type unitType, fPoint pos, faction_enum faction);
	~Unit();

public:


	bool Update(float dt);
	bool Move(float dt);
	bool Draw();

public:


	// Get Data
	bool IsDead();
	bool IsVisible();	// Outside Fog of War

	// Unit calculations
	Unit* EnemyInRange();
	bool TargetInRange();
	void UpdateBlitOrder()override;

public:

	faction_enum UnitFaction;
	unit_type UnitType;
	SDL_Rect UnitRect = {(int)position.x,(int)position.y, 20, 20};
	SDL_Rect CheckInCamera;

	unit_state unitState = unit_state::IDLE;
	unit_orders unitOrders = unit_orders::HOLD;

	fPoint origin;
	fPoint destination;
	Unit* target = nullptr;
	std::list<Unit*>* hostileUnits = nullptr;

	float speed = 100.0f;
	float visionRange = 10.0f;	//For enemy units this would be their aggro area
	float attackRange = 10.0f;
	float damage = 2;
	
};

#endif
