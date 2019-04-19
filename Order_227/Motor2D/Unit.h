#ifndef UNIT_H
#define UNIT_H

#include <vector>
#include "Entity.h"
#include "SDL/include/SDL.h"
#include "Pathfinding.h"


enum class unit_state
{
	NONE = -1,

	IDLE,	//Default state
	MOVING,
	ATTACKING,
	DEAD,

	MAX_STATES
};

enum class unit_orders
{
	NONE = -1,

	HOLD,	//Default order
	MOVE,
	ATTACK,
	MOVE_AND_ATTACK,
	PATROL,

	MAX_ORDERS
};

struct unit_stats
{
	float health = 0;
	uint damage = 0;

	uint attackRange = 0;
	uint visionRange = 0;

	float linSpeed = 100.0f;
	fVec2 vecSpeed;
};

class Unit :public Entity
{
public:

	Unit(fPoint pos, entity_type entityType, entity_faction faction = entity_faction::NEUTRAL);
	~Unit();

	bool Update(float dt) override;
	void UpdateBlitOrder() override;
	bool Draw();

public:

	//Setup
	bool LoadEntityData() override;

	// Main Workflow
	void UnitWorkflow(float dt);	// State workflow depending on order issued
	void ApplyState();				// Add state effects, like current animation

	//Order calling
	void OrderStandardSetup(iPoint destination);
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
	void AttackTarget(float dt);
	float Hurt(float damage);
	void Die();
	//void Kill();
	//void Hurt();

	//Get Data
	bool IsDead();
	bool IsVisible();
	bool NodeReached();
	bool DestinationReached();
	bool TargetDisplaced();

	//Unit calculations
	fVec2 SetupVecSpeed();
	Unit* EnemyInRange();
	bool  TargetInRange();

public:

	SDL_Rect CheckInCamera;
	unit_state unitState = unit_state::IDLE;
	unit_orders unitOrders = unit_orders::HOLD;
	SDL_Rect UnitBlitRect = { 12, 0, 55,47 }; //TODO-Jaume: Hardcoded

	iPoint origin;
	iPoint destination;
	std::vector<iPoint> unitPath;
	std::vector<iPoint>::iterator currNode;

	Unit** hostileUnits = nullptr;
	Unit* target = nullptr;
	bool targetLost;	// Used when there's a specific target to Search & Destroy which sight of can be lost

	unit_stats stats;
	SDL_Rect selectionRect = { 0, 0, 0, 0 };

	uint32 timeToDespawn = 5000;	//TODO: Hardcoded value, should be read through xml
	Timer despawnTimer;
	bool mustDespawn = false;
};

#endif //UNIT_H
