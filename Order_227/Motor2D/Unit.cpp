#include "Geometry.h"
#include "Render.h"
#include "Textures.h"
#include "Scene.h"
#include "Pathfinding.h"
#include "Entity_Manager.h"
#include "Entity.h"
#include "Unit.h"

Unit::Unit(fPoint pos, entity_type entityType, entity_faction faction) : Entity(pos, entityType, faction)
{
//	LoadEntityData();
}

Unit::~Unit()
{}

bool Unit::Start()
{
	return true;
}

bool Unit::Update(float dt)
{

	UnitWorkflow(dt);
	currentAnimation = &myApp->entities->animationArray[int(infatryType)][int(unitState)][int(unitDirection)];

	if (mustDespawn) {

		mustDespawn = false;
		active = false;
		myApp->entities->DestroyEntity(this);	//TODO: This should work with entity pools
	}
	else {

		CheckInCamera = { (int)position.x,(int)position.y, UnitBlitRect.w, UnitBlitRect.h };

		if (myApp->render->InsideCamera(CheckInCamera) == true) {

			UpdateBlitOrder();
			myApp->render->Push(order, texture, position.x, position.y, &currentAnimation->GetCurrentFrame(dt));
		}

		if (selected) 
			myApp->render->DrawQuad(UnitBlitRect, 255, 0, 0, 255, false);
		
	}

	return true;
}

void Unit::UpdateBlitOrder()
{

	//for (int i = 0; i < ENTITY_POOL_SIZE; i++) {

	//	if (myApp->entities->enemySoldiersList[i] != this) {

	//		if (this->position.y > (*item)->position.y)
	//			order += 1;
	//		else
	//			order -= 1;
	//	}
	//	else if (myApp->entities->playerSoldiersList[i] != this) {


	//	}
	//}

	for (int i = 0; i < UNITS_ARRAY_SIZE; ++i) {

		if (myApp->entities->entitiesArray[i] != this) {

			if (this->position.y > myApp->entities->entitiesArray[i]->position.y)
				order += 1;
			else
				order -= 1;

		}
	}

}

bool Unit::Draw()
{
	return true;
}

// Main workflow
void Unit::UnitWorkflow(float dt)
{
	unit_state prevState = unitState;

	switch (unitOrders) {
	case unit_orders::HOLD:	// Default order
		DoHold(dt);
		break;
	case unit_orders::MOVE:
		DoMove(dt);
		break;
	case unit_orders::ATTACK:
		DoAttack(dt);
		break;
	case unit_orders::MOVE_AND_ATTACK:
		DoMoveAndAttack(dt);
		break;
	case unit_orders::PATROL:
		DoPatrol(dt);
		break;
	case unit_orders::NONE:	// It's dead
		if (despawnTimer.Read() > timeToDespawn) {
			mustDespawn = true;
		}
		break;
	}

	if (prevState != unitState) {
		ApplyState();
	}
}

// Change animation according to state
void Unit::ApplyState()
{
	switch (unitState) {
	case unit_state::IDLE:
		//currentAnim = idleAnim;
		break;
	case unit_state::MOVING:
		//currentAnim = movingAnim;
		break;
	case unit_state::ATTACKING:
		//currentAnim = firingAnim;
		break;
	case unit_state::DEAD:
		//currentAnim = deadAnim;
		break;
	}
}

// Order processing
void Unit::DoHold(float dt)
{
	switch (unitState) {

	case unit_state::IDLE:
		target = EnemyInRange();
		if (target != nullptr) {
			AttackTarget(dt);
		}
		break;
	case unit_state::ATTACKING:
		if (TargetInRange() && target->IsDead() == false) {
			AttackTarget(dt);
		}
		else {
			target = nullptr;
			unitState = unit_state::IDLE;
		}
		break;
	}
}

void Unit::DoMove(float dt)
{
	if (NodeReached() == false) {
		Move(dt);
	}
	else {
		currNode = next(currNode);;

		if (DestinationReached() == false) {
			SetupVecSpeed();
		}
		else {
			StartHold();
		}
	}
}

void Unit::DoAttack(float dt)
{
	if (target->IsDead() == false) {

		if (target->IsVisible() == false) {	// If target gets in Fog of War and comes out, restart Attack Order to target
			targetLost = true;
		}
		else if (targetLost == true && target->IsVisible() == true) {
			if (TargetDisplaced() == true) {
				StartAttack(target);
			}
			else {
				targetLost = false;
			}
		}

		if (TargetInRange() == false) {
			if (unitState == unit_state::ATTACKING) {	// If Unit encounters target, attacks, but target leaves, start new AttackOrder to unit
				StartAttack(target);
			}
			else if (NodeReached() == false) {
				Move(dt);
			}
			else {
				currNode = next(currNode);

				if (DestinationReached() == false) {
					SetupVecSpeed();
				}
				else {
					StartAttack(target);
				}
			}
		}
		else {
			AttackTarget(dt);
		}
	}
	else {
		StartHold();
	}
}

void Unit::DoMoveAndAttack(float dt)
{
	if (NodeReached() == false) {
		if (unitState == unit_state::IDLE || unitState == unit_state::MOVING) {
			target = EnemyInRange();
			if (target != nullptr) {
				AttackTarget(dt);
			}
			else {
				Move(dt);
			}
		}
		else if (unitState == unit_state::ATTACKING) {
			if (TargetInRange() && target->IsDead() == false) {
				AttackTarget(dt);
			}
			else {
				target = nullptr;
				Move(dt);
			}
		}
	}
	else {
		currNode = next(currNode);

		if (DestinationReached() == false) {
			SetupVecSpeed();
		}
		else {
			StartHold();
			unitState = unit_state::IDLE;
		}
	}
}

void Unit::DoPatrol(float dt)
{
	if (NodeReached() == false) {
		if (unitState == unit_state::IDLE || unitState == unit_state::MOVING) {
			target = EnemyInRange();
			if (target != nullptr) {
				AttackTarget(dt);
			}
			else {
				Move(dt);
			}
		}
		else if (unitState == unit_state::ATTACKING) {
			if (TargetInRange() && target->IsDead() == false) {
				AttackTarget(dt);
			}
			else {
				target = nullptr;
				Move(dt);
			}
		}
	}
	else {
		currNode = next(currNode);

		if (DestinationReached() == false) {
			SetupVecSpeed();
		}
		else {
			StartPatrol(origin);
		}
	}
}

// Actions
bool Unit::Move(float dt)
{
	position.x += (stats.vecSpeed.x * dt);
	position.y += (stats.vecSpeed.y * dt);

	unitState = unit_state::MOVING;
	return true;
}

void Unit::AttackTarget(float dt)
{
	target->Hurt((float)stats.damage * dt);

	unitState = unit_state::ATTACKING;
}

float Unit::Hurt(float damage)
{
	stats.health -= damage;

	if (stats.health <= 0.0f) {
		Die();
	}

	return stats.health;
}

void Unit::Die()
{
	//TODO: Set animation to death animation
	despawnTimer.Start();
	unitOrders = unit_orders::NONE;
	unitState = unit_state::DEAD;
}

// Unit Data
bool Unit::IsDead()
{
	if (stats.health <= 0.0f || unitState == unit_state::DEAD) {
		return true;
	}
	else {
		return false;
	}
}

bool Unit::IsVisible()
{
	return true;	//TODO: Make function with Fog of War
}

bool Unit::NodeReached()
{
	bool ret = false;

	if (stats.vecSpeed.x > 0.0f) {
		if (position.x >= (float)currNode->x) {
			ret = true;
		}
	}
	else if (stats.vecSpeed.x < 0.0f) {
		if (position.x <= (float)currNode->x) {
			ret = true;
		}
	}

	if (stats.vecSpeed.y > 0.0f) {
		if (position.y >= (float)currNode->y) {
			ret = true;
		}
	}
	else if (stats.vecSpeed.y < 0.0f) {
		if (position.y <= (float)currNode->y) {
			ret = true;
		}
	}

	return ret;
}

bool Unit::DestinationReached()
{
	if (currNode == unitPath.end()) {
		return true;
	}
	else {
		return false;
	}
}

bool Unit::TargetDisplaced()
{
	if (target->position.x != destination.x || target->position.y != destination.y) {
		return true;
	}
	else {
		return false;
	}
}

// Unit Calculations
Unit* Unit::EnemyInRange()
{
	Unit* ret = nullptr;

	//for (int i = 0; i >= SOLDIERS_LIST_SIZE; ++i) {	//TODO-Carles: This is real fucking messy and expensive on runtime, requires list of active units
	//	if (hostileUnits[i]->active == true) {
	//		if (InsideSquareRadius(position, (float)stats.attackRange, hostileUnits[i]->position) == false) {
	//			continue;
	//		}
	//		else {
	//			if (InsideRadius(position, (float)stats.attackRange, hostileUnits[i]->position) == true) {
	//				ret = hostileUnits[i];
	//				break;
	//			}
	//		}
	//	}
	//}

	return ret;
}

bool Unit::TargetInRange()
{
	return InsideRadius(position, stats.attackRange, target->position);
}

fVec2 Unit::SetupVecSpeed()
{
	iPoint iPos = { (int)position.x, (int)position.y };

	stats.vecSpeed = GetVector2(iPos, *currNode);
	stats.vecSpeed = stats.vecSpeed.GetUnitVector();
	stats.vecSpeed *= stats.linSpeed;
	return stats.vecSpeed;
}

// Order calling
void Unit::OrderStandardSetup(iPoint destination)
{
	unitPath.clear();
	target = nullptr;

	origin = { (int)position.x, (int)position.y };
	this->destination = destination;

	unitPath = *myApp->pathfinding->CreatePath(origin, destination);
	currNode = unitPath.begin();
	SetupVecSpeed();
}

void Unit::StartHold()
{
	target = nullptr;

	unitOrders = unit_orders::HOLD;
	unitState = unit_state::IDLE;
}

void Unit::StartMove(iPoint destination)
{
	OrderStandardSetup(destination);

	unitOrders = unit_orders::MOVE;
	unitState = unit_state::IDLE;
}

void Unit::StartAttack(Unit* target)
{
	unitPath.clear();
	this->target = target;
	targetLost = false;

	origin = { (int)position.x, (int)position.y };
	this->destination = { (int)target->position.x, (int)target->position.y };

	unitPath = *myApp->pathfinding->CreatePath(origin, destination);
	currNode = unitPath.begin();
	SetupVecSpeed();

	unitOrders = unit_orders::ATTACK;
	unitState = unit_state::IDLE;
}

void Unit::StartMoveAndAttack(iPoint destination)
{
	OrderStandardSetup(destination);

	unitOrders = unit_orders::MOVE_AND_ATTACK;
	unitState = unit_state::IDLE;
}

void Unit::StartPatrol(iPoint destination)
{
	OrderStandardSetup(destination);

	unitOrders = unit_orders::PATROL;
	unitState = unit_state::IDLE;
}
