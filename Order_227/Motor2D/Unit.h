#ifndef UNIT_H
#define UNIT_H

#include <vector>
#include "Entity.h"
#include "SDL/include/SDL.h"
#include "Pathfinding.h"

<<<<<<< HEAD
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
=======
enum class unit_state {
	NONE = -1,
>>>>>>> master

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
<<<<<<< HEAD

	Unit(unit_type unit_Type, fPoint pos, faction_enum faction);
	~Unit();

public:

	//void Move();
	//void Attack();
=======

	Unit(fPoint pos, entity_type Entitytype, entity_faction faction = entity_faction::NEUTRAL);
	~Unit();
>>>>>>> master
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

<<<<<<< HEAD

	bool LoadEntityData() override;
	// Actions
	bool Move(float dt);	// Move unit position
=======
	//Actions
	virtual bool Move(float dt);
>>>>>>> master
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
<<<<<<< HEAD

	faction_enum unitFaction;
	unit_type unitType;
=======
>>>>>>> master

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
	SDL_Rect selectionRect = { 0,0,0,0 };

	float linSpeed=0.0f;
	fVec2 vecSpeed;
<<<<<<< HEAD
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
=======

>>>>>>> master
};

#endif // !UNIT_H