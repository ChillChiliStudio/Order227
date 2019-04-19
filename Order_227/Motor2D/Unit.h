#ifndef UNIT_H
#define UNIT_H

#include <vector>
#include "Entity.h"
#include "SDL/include/SDL.h"
#include "Pathfinding.h"


enum class unit_state {

	NONE = -1,

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

struct unit_stats
{

	int speed = 0;
	int damage = 0;
	int healtPoints = 0;
	float visionRange = 0;
	float attackRange = 0;
	uint velocity = 0;
};

class Unit :public Entity
{
public:

	Unit(fPoint pos, entity_type Entitytype, entity_faction faction = entity_faction::NEUTRAL);
	~Unit();

	bool Start() override;
	bool Update(float dt) override;
	bool Draw();
	void UpdateBlitOrder() override;

public:

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


	bool LoadEntityData() override;

	// Actions
	bool Move(float dt);	// Move unit position
	void AttackTarget();
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
	SDL_Rect UnitBlitRect = { 12, 0, 55,47 }; //TODO desjarcodear

	iPoint origin;
	iPoint destination;
	std::vector<iPoint> Path;
	std::vector<iPoint>::iterator currNode;
	std::vector<iPoint> nodeList;

	Unit* target = nullptr;
	std::list<Unit*>* hostileUnits = nullptr;

	uint currentHealth = 0;
	unit_stats stats;
	SDL_Rect selectionRect = { 0, 0, 0, 0 };

	float linSpeed=0.0f;
	fVec2 vecSpeed;

};

#endif // !UNIT_H
