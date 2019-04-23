#include "Geometry.h"
#include "Render.h"
#include "Textures.h"
#include "Scene.h"
#include "Map.h"
#include "Pathfinding.h"
#include "Entity_Manager.h"
#include "Entity.h"
#include "Unit.h"
#include "Audio.h"


Unit::Unit(fPoint pos, entity_type entityType, entity_faction faction) : Entity(pos, entityType, faction)
{
//	LoadEntityData();
	
}

Unit::~Unit()
{}

bool Unit::Start()
{
	currentAnimation = (&myApp->entities->animationArray[int(infantryType)][int(unitState)][int(unitDirection)]);

	unitState = unit_state::IDLE;
	unitOrders = unit_orders::HOLD;
	unitDirection = unit_directions::SOUTH_EAST;
	currentAnimation = (&myApp->entities->animationArray[int(infantryType)][int(unitState)][int(unitDirection)]);

	return true;
}

bool Unit::Update(float dt)
{
	onCamera = InsideCamera();

	UnitWorkflow(dt);

	UnitRect.x = position.x-10;
	UnitRect.y = position.y-10;

	if (mustDespawn) {
		mustDespawn = false;
		myApp->entities->DeActivateInfantry(this);	//TODO: Can't use "deactivate" because it only works with Infantry classes
	}
	else {
		if (myApp->entities->entitiesDebugDraw && currNode != unitPath.end()) {
			DrawPath();
		}

		currentAnimation.AdvanceAnimation(dt);	// Animation must continue even if outside camera

		if (onCamera) {	// If inside camera

			if (unitState == unit_state::MOVING) {	// State changes are always updated on animation,
				unit_directions lastDirection = unitDirection;

				if (lastDirection != CheckDirection(stats.vecSpeed)) {
					UpdateAnimation();
				}
			}

			UpdateBlitOrder();
			Draw();

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

bool Unit::Draw()
{
	myApp->render->Push(order, texture, (int)position.x, (int)position.y, &currentAnimation.GetTheActualCurrentFrame());

	return true;
}

bool Unit::DebugDraw()
{
	/*if (selected) {
		myApp->render->DrawQuad(UnitRect, 0, 255, 0, 255, false);
	}
	else {*/
	if (selected == false) {
		Uint8 rgb[3] = { 0, 0, 0 };

		switch (faction) {	//TODO-Carles: Checking it's faction inside the worflow to do stuff is bad
		case entity_faction::COMMUNIST:
			rgb[0] = 255;	//Red
			break;
		case entity_faction::CAPITALIST:
			rgb[2] = 255;	//Blue
			break;
		case entity_faction::NEUTRAL:
			rgb[0] = 255;
			rgb[2] = 255;	//Magenta
			break;
		}

		myApp->render->DrawQuad(UnitRect, rgb[0], rgb[1], rgb[2], 255, false);
	}

	return true;
}

void Unit::DrawPath()
{
	Uint8 rgb[3] = { 0, 0, 0 };

	switch (faction) {	//TODO-Carles: Checking it's faction inside the worflow to do stuff is bad
	case entity_faction::COMMUNIST:
		rgb[0] = 255;	//Red
		break;
	case entity_faction::CAPITALIST:
		rgb[2] = 255;	//Blue
		break;
	case entity_faction::NEUTRAL:
		rgb[0] = 255;
		rgb[2] = 255;	//Magenta
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
		UpdateAnimation();
	}
}

unit_directions Unit::CheckDirection(fVec2 direction)
{
	stats.vecAngle = direction.GetAngle({ 0.0f, -1.0f });
	stats.vecAngle = RadsToDeg(stats.vecAngle);
	
	if (stats.vecAngle > 337.5f || stats.vecAngle <= 22.5f) {
		unitDirection = unit_directions::NORTH;
	}
	else if (stats.vecAngle > 292.5f) {
		unitDirection = unit_directions::NORTH_EAST;
	}
	else if (stats.vecAngle > 247.5f) {
		unitDirection = unit_directions::EAST;
	}
	else if (stats.vecAngle > 202.5f) {
		unitDirection = unit_directions::SOUTH_EAST;
	}
	else if (stats.vecAngle > 157.5f) {
		unitDirection = unit_directions::SOUTH;
	}
	else if (stats.vecAngle > 112.5f) {
		unitDirection = unit_directions::SOUTH_WEST;
	}
	else if (stats.vecAngle > 67.5f) {
		unitDirection = unit_directions::WEST;
	}
	else if (stats.vecAngle > 22.5f) {
		unitDirection = unit_directions::NORTH_WEST;
	}

	return unitDirection;
}

// Change animation according to state
void Unit::UpdateAnimation()
{
	currentAnimation = (&myApp->entities->animationArray[int(infantryType)][int(unitState)][int(unitDirection)]);
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
		else if (faction == entity_faction::CAPITALIST && BaseInRange()) {	//TODO-Carles: Checking it's faction inside the worflow to do stuff is bad
			AttackBase(dt);	//TODO: Should be AttackTarget, not base specifically
		}
		break;
	case unit_state::ATTACKING:
		if (faction == entity_faction::CAPITALIST && BaseInRange()) {	//TODO-Carles: Checking it's faction inside the worflow to do stuff is bad
			AttackBase(dt);	//TODO: Should be AttackTarget, not base specifically
		}
		else if (TargetInRange() && target->IsDead() == false) {
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
			else if (faction == entity_faction::CAPITALIST && BaseInRange()) {	//TODO-Carles: Checking it's faction inside the worflow to do stuff is bad
				AttackBase(dt);	//TODO: Should be AttackTarget, not base specifically
			}
			else {
				Move(dt);
			}
		}
		else if (unitState == unit_state::ATTACKING) {
			if (faction == entity_faction::CAPITALIST && BaseInRange()) {	//TODO-Carles: Checking it's faction inside the worflow to do stuff is bad
				AttackBase(dt);	//TODO: Should be AttackTarget, not base specifically
			}
			else if (TargetInRange() && target->IsDead() == false) {
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
			else if (faction == entity_faction::CAPITALIST && BaseInRange()) {	//TODO-Carles: Checking it's faction inside the worflow to do stuff is bad
				AttackBase(dt);	//TODO: Should be AttackTarget, not base specifically
			}
			else {
				Move(dt);
			}
		}
		else if (unitState == unit_state::ATTACKING) {
			if (TargetInRange() && target->IsDead() == false) {
				AttackTarget(dt);
			}
			else if (faction == entity_faction::CAPITALIST && BaseInRange()) {	//TODO-Carles: Checking it's faction inside the worflow to do stuff is bad
				AttackBase(dt);	//TODO: Should be AttackTarget, not base specifically
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
	
 //Shoot sound

	/*int i = rand() % 3;
	myApp->audio->PlayFx(myApp->audio->SoundFX_Array[(int)infantryType][SOV][(int)type_sounds::SHOT][i],0);*/


	fVec2 targetDirection = GetVector2(position, target->position);

	unit_directions lastDirection = unitDirection;

	CheckDirection(targetDirection);

	if (lastDirection != unitDirection) {
		UpdateAnimation();
	}

	target->Hurt((float)stats.damage * dt);

	unitState = unit_state::ATTACKING;
}

void Unit::AttackBase(float dt)
{
	fPoint trueBasePos = { myApp->entities->mainBase->position.x + 100, myApp->entities->mainBase->position.y + 70 };

	fVec2 targetDirection = GetVector2(position, trueBasePos);

	unit_directions lastDirection = unitDirection;

	CheckDirection(targetDirection);

	if (lastDirection != unitDirection) {
		UpdateAnimation();
	}

	myApp->entities->mainBase->health -= (float)(stats.damage * dt);

	if (myApp->entities->mainBase->health < 0) {
		//TODO: Lose flag
	}

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
	despawnTimer.Start();
	unitOrders = unit_orders::NONE;
	unitState = unit_state::DEAD;
	currentAnimation = (&myApp->entities->animationArray[int(infantryType)][int(unitState)][0]);
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

bool Unit::InsideCamera()
{
	bool ret = false;

	if (myApp->render->InsideCamera({ (int)position.x,(int)position.y, UnitBlitRect.w, UnitBlitRect.h }) == true) {
		ret = true;
	}

	return ret;
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
		if (hostileUnits[i]->active == true && hostileUnits[i]->IsDead() == false) {

			if (InsideSquareRadius(position, (float)stats.attackRange, hostileUnits[i]->position)
				&& InsideRadius(position, (float)stats.attackRange, hostileUnits[i]->position))
			{
				ret = hostileUnits[i];
				break;
			}
		}
	}

	return ret;
}

bool Unit::BaseInRange()
{
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

	bool ret = false;

	fPoint trueBasePos = { myApp->entities->mainBase->position.x + 100, myApp->entities->mainBase->position.y + 70 };

	//TODO: Hardcoded bullshit, building should be generic, subclasses called "StrategicPoint", "MainBase", "Turret", etc
	if (InsideSquareRadius(position, (float)stats.attackRange, trueBasePos)
		&& InsideRadius(position, (float)stats.attackRange, trueBasePos))
	{
		ret = true;
	}

	return ret;
}

bool Unit::TargetInRange()
{
	return InsideRadius(position, (float)stats.attackRange, target->position);
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
	fPoint nodePos = { (float)(currNode->x), (float)(currNode->y) };

	stats.vecSpeed = GetVector2(position, nodePos);
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

	UpdateAnimation();
}

void Unit::StartMove(iPoint destination)
{
	OrderStandardSetup(destination);

	unitOrders = unit_orders::MOVE;
	unitState = unit_state::IDLE;

	UpdateAnimation();
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

	UpdateAnimation();
}

void Unit::StartMoveAndAttack(iPoint destination)
{
	OrderStandardSetup(destination);

	unitOrders = unit_orders::MOVE_AND_ATTACK;
	unitState = unit_state::IDLE;

	UpdateAnimation();
}

void Unit::StartPatrol(iPoint destination)
{
	OrderStandardSetup(destination);

	unitOrders = unit_orders::PATROL;
	unitState = unit_state::IDLE;

	UpdateAnimation();
}
