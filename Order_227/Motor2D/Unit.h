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

enum class unit_state {
	IDLE,	//Default state
	MOVING,
	FIRING,
	DEAD,

	MAX_STATES
};

enum class unit_orders {
	HOLD,	//Default order
	MOVE,
	ATTACK,
	MOVE_AND_ATTACK,
	PATROL,

	MAX_ORDERS
};

class Unit : public Entity {

public:

	Unit(unit_type unitType, fPoint pos, faction_enum faction);
	~Unit();

public:

	//void Move();
	//void Attack();
	bool Update(float dt);
	bool Draw();

public:

	void CheckOrders();		// Check for new orders
	void CheckState();		// Check current player state
	void ApplyState();		// Add state effects
	bool Move(float dt);	// Move unit position

	//void Kill();
	//void Hurt();
	//bool IsDead();


	void UpdateBlitOrder()override;

public:

	faction_enum UnitFaction;
	unit_type UnitType;
	SDL_Rect UnitRect = { (int)position.x,(int)position.y, 20, 20 };

	unit_state status = unit_state::IDLE;
	unit_orders orders = unit_orders::HOLD;
	bool newOrder = false;

	float speed = 100.0f;
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
