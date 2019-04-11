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

	//void Move();
	//void Attack();
	bool Update(float dt);
	bool Move(float dt);
	bool Draw();

public:

	//void Kill();
	//void Hurt();
	//bool IsDead();
	void UpdateBlitOrder()override;

public:

	faction_enum UnitFaction;
	unit_type UnitType;

	SDL_Rect UnitBlitRect = { 12, 0, 55,47 };
	SDL_Rect UnitRect = { (int)position.x,(int)position.y, 50, 50 };;
	float speed =0.0f;

	SDL_Rect CheckInCamera;

	float damage = 2;
	
	//unsigned int life;
	//unsigned int maxLife;
	//unit_state status;
	//iPoint detectionRadius;
	//iPoint attackRange;
	//bool enemyDetected;
	//bool enemyInRange;
};



#endif
