#ifndef UNIT_H
#define UNIT_H

#include "Entity.h"
#include "SDL/include/SDL.h"

enum class unit_type {

	INFANTRY,
	UNKNOWN = 1
};

enum class faction_enum {

	COMMUNIST,
	CAPITALIST,
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
	NONE = -1,
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

	bool Update(float dt) override;
	bool Draw();
	void UpdateBlitOrder() override;

public:

	// Main Workflow
	void UnitWorkflow(float dt);		// State workflow depending on order issued
	void ApplyState();		// Add state effects, like current animation

	// Order calling
	void SetupOrder(iPoint destination);
	void StartHold();
	void StartMove(iPoint destination);
	void StartAttack(Unit* target);
	void StartMoveAndAttack(iPoint destination);
	void StartPatrol(iPoint destination);

	// Order processing
	void DoHold(float dt);
	void DoMove(float dt);
	void DoAttack(float dt);
	void DoMoveAndAttack(float dt);
	void DoPatrol(float dt);

	// Actions
	bool Move(float dt);	// Move unit position
	void AttackTarget();
	//void Kill();
	//void Hurt();

	// Get Data
	bool IsDead();
	bool IsVisible();	// Outside Fog of War
	bool NodeReached();
	bool DestinationReached();
	bool TargetDisplaced();

	// Unit calculations
	fVec2 SetupVecSpeed();
	Unit* EnemyInRange();
	bool TargetInRange();

public:

	faction_enum unitFaction;
	unit_type unitType;
	SDL_Rect unitRect = {(int)position.x,(int)position.y, 20, 20};
	SDL_Rect CheckInCamera;

	unit_state unitState = unit_state::IDLE;
	unit_orders unitOrders = unit_orders::HOLD;

	iPoint origin;
	iPoint destination;
	std::list<iPoint>::iterator currNode;
	std::list<iPoint> nodeList;	//TODO: LuchoAlert, use or change this however you like, I think the concept is understandable

	Unit* target = nullptr;
	std::list<Unit*>* hostileUnits = nullptr;

	float linSpeed = 100.0f;
	fVec2 vecSpeed;
	float damage = 2.0f;
	float visionRange = 10.0f;	//For enemy units this would be their aggro area
	float attackRange = 10.0f;
	//unsigned int life;
	//unsigned int maxLife;
	//unit_state status;
	//iPoint detectionRadius;
	//iPoint attackRange;
	//bool enemyDetected;
	//bool enemyInRange;

};

#endif
