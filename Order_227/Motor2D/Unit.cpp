#include "Geometry.h"
#include "Render.h"
#include "Textures.h"
#include "Scene.h"
#include "Map.h"
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

	UnitRect.x = position.x;
	UnitRect.y = position.y;

	if (mustDespawn) {

		mustDespawn = false;
		myApp->entities->DeActivateInfantry(this);	//TODO: Can't use "deactivate" because it only works with Infantry classes
	}
	else {
		if (myApp->entities->entitiesDebugDraw && currNode != unitPath.end()) {
			DrawPath();
		}

		CheckInCamera = { (int)position.x,(int)position.y, UnitBlitRect.w, UnitBlitRect.h };

		if (myApp->render->InsideCamera(CheckInCamera) == true) {	// If inside camera

			UpdateBlitOrder();
			Draw(dt);

			if (selected) {	//TODO: This should be as a debug functionality, but for now it'll do
				myApp->render->DrawQuad(UnitRect, 0, 255, 0, 255, false);
			}

			if (myApp->entities->entitiesDebugDraw) {
				DebugDraw();
			}
		}
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

bool Unit::Draw(float dt)
{
	myApp->render->Push(order, texture, position.x, position.y, &currentAnimation->GetCurrentFrame(dt));

	return true;
}

bool Unit::DebugDraw()
{
	/*if (selected) {
		myApp->render->DrawQuad(UnitRect, 0, 255, 0, 255, false);
	}
	else {*/
	if (selected == false) {
		myApp->render->DrawQuad(UnitRect, 255, 0, 0, 255, false);
	}

	return true;
}

void Unit::DrawPath()
{
	Uint8 rgb[3] = { 0, 0, 0 };

	switch (faction) {	//TODO-Carles: Checking it's faction inside the worflow to do stuff is bad
	case entity_faction::COMMUNIST:
		rgb[1] = 255;	//Green
		break;
	case entity_faction::CAPITALIST:
		rgb[0] = 255;	//Red
		break;
	case entity_faction::NEUTRAL:
		rgb[2] = 255;	//Blue
		break;
	}

	myApp->render->DrawLine((int)position.x, (int)position.y, (*currNode).x, (*currNode).y, rgb[0], rgb[1], rgb[2], 255, true);

	if (unitPath.size() > 1) {
		for (std::vector<iPoint>::iterator it = currNode; next(it) != unitPath.end(); it = next(it)) {
			myApp->render->DrawLine((*it).x, (*it).y, (*next(it)).x, (*next(it)).y, rgb[0], rgb[1], rgb[2], 255, true);
		}
	}
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

	for (int i = 0; i < INFANTRY_ARRAY_SIZE; ++i) {	//TODO-Carles: This is real fucking messy and expensive on runtime, requires list of active units, one for each side
		if (hostileUnits[i]->active == true) {

			if (InsideSquareRadius(position, (float)stats.attackRange, hostileUnits[i]->position)
				&& InsideRadius(position, (float)stats.attackRange, hostileUnits[i]->position))
			{
				ret = hostileUnits[i];
				break;
			}
		}
	}

	// TODO: Capitalist should iterate an "attackable buildings" list
	/*if (ret == nullptr && hostileBuildings != nullptr) {
		for (int i = 0; i < BUILDINGS_ARRAY_SIZE; ++i) {	//TODO-Carles: This is real fucking messy and expensive on runtime, requires list of active units, one for each side
			if (hostileUnits[i]->active == true) {
				if (InsideSquareRadius(position, (float)stats.attackRange, hostileUnits[i]->position) == false) {
					continue;
				}
				else {
					if (InsideRadius(position, (float)stats.attackRange, hostileUnits[i]->position) == true) {
						ret = hostileUnits[i];
						break;
					}
				}
			}
		}
	}*/

	if (ret == nullptr && faction == entity_faction::CAPITALIST) {	//TODO: Hardcoded bullshit, building should be generic, subclasses called "StrategicPoint", "MainBase", "Turret", etc
		if (InsideSquareRadius(position, (float)stats.attackRange, myApp->entities->mainBase->position)
			&& InsideRadius(position, (float)stats.attackRange, myApp->entities->mainBase->position))
		{
			myApp->entities->mainBase->health -= (float)stats.damage * myApp->GetDT();
			
			if (myApp->entities->mainBase->health < 0) {
				//TODO: Lose flag
			}
			
			unitState = unit_state::ATTACKING;
		}
	}

	return ret;
}

bool Unit::TargetInRange()
{
	return InsideRadius(position, stats.attackRange, target->position);
}

void Unit::SetupPath()
{
	unitPath.clear();

	iPoint mapOrigin = myApp->map->WorldToMap(origin.x, origin.y);
	iPoint mapDestination = myApp->map->WorldToMap(destination.x, destination.y);

	myApp->pathfinding->CreatePath(mapOrigin, mapDestination);
	unitPath = *myApp->pathfinding->GetLastPath();

	for (std::vector<iPoint>::iterator it = unitPath.begin(); it != unitPath.end(); it = next(it)) {
		*it = myApp->map->MapToWorld(it->x, it->y);
	}

	currNode = unitPath.begin();
	SetupVecSpeed();
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
	target = nullptr;

	origin = { (int)position.x, (int)position.y };
	this->destination = destination;

	SetupPath();
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
	this->target = target;
	targetLost = false;

	origin = { (int)position.x, (int)position.y };
	this->destination = { (int)target->position.x, (int)target->position.y };

	SetupPath();

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
