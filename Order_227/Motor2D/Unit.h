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
						
	HOLD,	// Defend your position/area
	MOVE,	// Move to marked position
	HUNT,	// Search and destroy a specific target
	PATROL,	// Move back and forward scouting a path
	
	MAX_ORDERS
};

enum class unit_aggro
{
	NONE = -1,

	PASSIVE,	// SHIFT: Don't attack hostiles
	DEFENSIVE,	// DEFAULT: Attack hostiles on your range radius
	AGRESSIVE,	// CONTROL: Go Attack hostiles on your vision radius

	MAX_AGGRO
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
	void StartHold();
	void StartMove(iPoint destination);
	void StartHunt(Unit* target);
	void StartAggroHunt(Unit* target);
	void StartPatrol(iPoint destination);
	void ResumePatrol();
	void ResumeLastOrder();	//Used when aggro has previously interrupted an order

	// Order processing
	void DoHold(float dt);
	void DoMove(float dt);
	void DoHunt(float dt);
	void DoAggroHunt(float dt);
	void DoPatrol(float dt);

	// Actions
	bool Move(float dt);				// Move unit
	bool FindEnemies(float dt);			// Find nearby enemies depending on aggro
	void AttackCurrTarget(float dt);	
	void AttackBase(float dt);	//TODO: Hardcoded shit, should work with FindEnemies/AttackCurrTarget
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
	bool TargetDisplaced(Unit* target);

	//Unit calculations
	void SetupPath(iPoint origin, iPoint destination);
	fVec2 SetupVecSpeed();
	Unit* EnemyInRadius(uint radius);
	bool BaseInRange();	//TODO: Should be "building in range" or directly be included inside EnemyInRange
	bool TargetInRange(Unit* target);

public:

	bool onCamera = false;

	// State flags
	unit_state unitState = unit_state::IDLE;
	unit_orders unitOrders = unit_orders::HOLD;		// Primary player-given orders
	unit_orders unitAuxOrders = unit_orders::NONE;	// Auxiliar self-given orders
	unit_aggro unitAggro = unit_aggro::DEFENSIVE;
	unit_directions unitDirection = unit_directions::SOUTH_EAST;

	// Animation
	SDL_Rect UnitBlitRect = { 12, 0, 55,47 }; //TODO desjarcodear
	Animation currentAnimation;

	//Sound
	uint attackSfxId;	// Attack sfx ID

	// Stats
	float health;		// Health
	uint damage;		// Damage inflicted on each attack
	float cadency;		// Miliseconds/Attack (Miliseconds between each attack)
	Timer attackTimer;	// Attack timer

	// Radius
	uint attackRadius;	//Distance to attack
	uint visionRadius;	//Distance to see

	// Speed
	float linSpeed;	// Absolute speed
	fVec2 vecSpeed;	// Vectorial speed
	float vecAngle;	// Vector angle in reference with North-directed reference vector

	// Pathfinding
	iPoint origin;							// Origin of path
	iPoint destination;						// Destination of path
	std::vector<iPoint> unitPath;			// Unit path in nodes
	std::vector<iPoint>::iterator currNode;	// Current node to move to

	//Aggro
	iPoint aggroDestination;		// Destination of secondary aggro path
	unit_orders prevOrder;			// Order given before aggro
	Unit* aggroTarget;				// Aggro-created target
	bool aggroTriggered = false;	// Aggro flag

	// Attack
	Unit** hostileUnits = nullptr;			// List of hostile units
	Building** hostileBuildings = nullptr;	// List of enemy buildings
	Unit* currTarget = nullptr;				// Currently attacking target
	Unit* huntTarget = nullptr;				// Fixed hunt target
	bool targetLost;						// Marks lost vision of hunt target

	// Death
	uint32 timeToDespawn = 5000;	//TODO: Hardcoded value, should be read through xml
	Timer despawnTimer;
	bool mustDespawn = false;
	
	// Spawn
	int cost;
	int productionTime;
	int unitThreat;

	//TODO: Shit to erase
	infantry_type infantryType;
};

#endif //UNIT_H
