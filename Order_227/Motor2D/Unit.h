#ifndef UNIT_H
#define UNIT_H

#include <vector>
#include "Entity.h"
#include "SDL/include/SDL.h"
#include "Pathfinding.h"
#include "Animation.h"

class Building;

enum class infantry_type {	// TODO: This should be a single enum with ALL units saved on it, NO ramifications

	INFANTRY_NONE = -1,
	BASIC,
	CONSCRIPT,
	BAZOOKA,
	MACHINE_GUN,
	INFANTRY_MAX
};

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

enum class unit_directions {

	NONE = -1,
	NORTH,
	NORTH_WEST,
	WEST,
	SOUTH_WEST,
	SOUTH,
	SOUTH_EAST,
	EAST,
	NORTH_EAST,

	MAX_DIRECTIONS

};

struct unit_stats
{
	float health;
	uint damage;
	float cadency;

	uint attackRange;
	uint visionRange;

	float linSpeed;
	fVec2 vecSpeed;
	float vecAngle;

	int cost;
	int productionTime;
	int unitThreat;
};

class Unit :public Entity
{
public:

	Unit(fPoint pos, entity_type entityType, entity_faction faction = entity_faction::NEUTRAL);
	~Unit();

	bool Start() override;
	bool Update(float dt) override;
	void UpdateBlitOrder() override;
	bool Draw();

public:

	// Debug Tools
	bool DebugDraw();
	void DrawPath();

	// Main Workflow
	void UnitWorkflow(float dt);	// State workflow depending on order issued
	void UpdateAnimation();				// Add state effects, like current animation
	unit_directions CheckDirection(fVec2 direction);

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
	void AttackBase(float dt);	//TODO: Hardcoded shit, should work with AttackTarget
	float Hurt(float damage);
	void Die();
	//void Kill();
	//void Hurt();

	//Get Data
	bool IsDead();
	bool InsideCamera();
	bool IsVisible();
	bool NodeReached();
	bool DestinationReached();
	bool TargetDisplaced();

	//Unit calculations
	void SetupPath();
	fVec2 SetupVecSpeed();
	Unit* EnemyInRange();
	bool BaseInRange();	//TODO: Should be "building in range" or directly be included inside EnemyInRange
	bool TargetInRange();

public:

	bool onCamera = false;

	unit_state unitState = unit_state::IDLE;
	unit_orders unitOrders = unit_orders::HOLD;
	unit_directions unitDirection = unit_directions::SOUTH_EAST;
	SDL_Rect UnitBlitRect = { 12, 0, 55,47 }; //TODO desjarcodear

	Animation currentAnimation;

	iPoint origin;
	iPoint destination;
	std::vector<iPoint> unitPath;
	std::vector<iPoint>::iterator currNode;

	std::vector <Unit*> hostileUnits;
	std::list<Building*> hostileBuildings;
	Unit* target = nullptr;
	bool targetLost;	// Used when there's a specific target to Search & Destroy which sight of can be lost

	infantry_type infantryType;
	unit_stats stats;
	SDL_Rect selectionRect = { 0, 0, 0, 0 };

	uint32 timeToDespawn = 5000;	//TODO: Hardcoded value, should be read through xml
	Timer despawnTimer;
	bool mustDespawn = false;
};

#endif //UNIT_H
