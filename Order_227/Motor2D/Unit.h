#ifndef UNIT_H
#define UNIT_H	//@Carles

#include <vector>
#include "Entity.h"
#include "SDL/include/SDL.h"
#include "Pathfinding.h"
#include "Animation.h"

class Building;

enum class infantry_type	// TODO: This should be a single enum with ALL units saved on it, NO ramifications
{
	INFANTRY_NONE = -1,

	BASIC,
	CONSCRIPT,
	BAZOOKA,  //Unit of type Launcher
	DESOLATOR,
	CHRONO,
	SNIPER,
	DOG,
	COW,
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
	AGGRESSIVE,	// CONTROL: Go Attack hostiles on your vision radius

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

struct unit_stats {	// Data imported through xml and depends on Unit type	//IMPROVE: Move inside Unit

	// Stats
	float health;		// Health
	uint damage;		// Damage inflicted on each attack
	float cadency;		// Miliseconds/Attack (Miliseconds between each attack)

	// Radius
	uint attackRadius;	//Distance to attack
	uint visionRadius;	//Distance to see

	// Speed
	float linSpeed;	// Absolute speed

	//Sound
	uint attackSfxId;	// Attack sfx ID

	// Spawn
	int cost;
	int productionTime;
	int unitThreat;
};

class Unit :public Entity
{
public:

	Unit();
	Unit(fPoint pos, infantry_type infType, entity_faction faction = entity_faction::NEUTRAL);
	~Unit();

	bool Start() override;
	bool Update(float dt) override;
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
	bool StartHold();
	bool StartMove(iPoint destination);
	bool StartHunt(Entity* target);
	bool StartAggroHunt(Entity* target);
	bool StartPatrol(iPoint destination);
	bool ResumePatrol();
	bool ResumeLastOrder();	//Used when aggro has previously interrupted an order

	// Order processing
	void DoHold(float dt);
	void DoMove(float dt);
	void DoHunt(float dt);
	void DoAggroHunt(float dt);
	void DoPatrol(float dt);

	// Actions
	bool Move(float dt);				// Move unit
	bool FindEnemies(float dt);			// Find nearby enemies depending on aggro
	virtual void AttackCurrTarget(float dt);
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
	bool TargetDisplaced(Entity* target);
	//bool IsSelected();

	//Unit calculations
	bool SetupPath(iPoint origin, iPoint destination);
	fVec2 SetupVecSpeed();
	Entity* EnemyInRadius(uint radius);
	Unit* AttackingAllyInRadius(uint radius);
	bool TargetInRange(Entity* target);

public:
	infantry_type infantryType;
	bool onCamera = false;

	// Stats
	unit_stats stats;

	// State flags
	unit_state unitState = unit_state::IDLE;
	unit_orders unitOrders = unit_orders::HOLD;		// Primary player-given orders
	unit_orders unitAuxOrders = unit_orders::NONE;	// Auxiliar self-given orders
	unit_aggro unitAggro = unit_aggro::DEFENSIVE;
	unit_directions unitDirection = unit_directions::SOUTH_EAST;

	// Animation
	Animation currentAnimation;

	// Speed
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
	Entity* aggroTarget = nullptr;	// Aggro-created target
	bool aggroTriggered = false;	// Aggro flag

	// Attack
	Entity* currTarget = nullptr;	// Currently attacking target
	Entity* huntTarget = nullptr;	// Fixed hunt target
	bool targetLost;				// Marks lost vision of hunt target

	Timer attackTimer;	// Attack timer

	// Death
	uint32 timeToDespawn = 5000;	//TODO: Hardcoded value, should be read through xml
	Timer despawnTimer;
	bool mustDespawn = false;
};

#endif //UNIT_H
