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

				if (lastDirection != CheckDirection(vecSpeed)) {
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
	vecAngle = direction.GetAngle({ 0.0f, -1.0f });
	vecAngle = RadsToDeg(vecAngle);
	
	if (vecAngle > 337.5f || vecAngle <= 22.5f) {
		unitDirection = unit_directions::NORTH;
	}
	else if (vecAngle > 292.5f) {
		unitDirection = unit_directions::NORTH_EAST;
	}
	else if (vecAngle > 247.5f) {
		unitDirection = unit_directions::EAST;
	}
	else if (vecAngle > 202.5f) {
		unitDirection = unit_directions::SOUTH_EAST;
	}
	else if (vecAngle > 157.5f) {
		unitDirection = unit_directions::SOUTH;
	}
	else if (vecAngle > 112.5f) {
		unitDirection = unit_directions::SOUTH_WEST;
	}
	else if (vecAngle > 67.5f) {
		unitDirection = unit_directions::WEST;
	}
	else if (vecAngle > 22.5f) {
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
	if (unitState == unit_state::ATTACKING) {
		if (TargetInRange(currTarget) && currTarget->IsDead() == false) {
			AttackTarget(dt);
		}
		else {
			currTarget = nullptr;
			unitState = unit_state::IDLE;
		}
	}
	else if (unitState < unit_state::ATTACKING && unitAggro > unit_aggro::PASSIVE) {
		currTarget = EnemyInRadius(attackRadius);
		if (currTarget != nullptr) {
			AttackTarget(dt);
		}
		else if (unitAggro == unit_aggro::AGRESSIVE) {
			currTarget = EnemyInRadius(visionRadius);
			if (currTarget != nullptr) {
				StartAggroHunt(currTarget);
			}
		}
	}

	else if (faction == entity_faction::CAPITALIST && BaseInRange()) {	//TODO-Carles: Checking it's faction inside the worflow to do stuff is bad
		AttackBase(dt);	//TODO: Should be TryAttack, not base specifically
	}
}

void Unit::DoMove(float dt)
{
	if (unitState == unit_state::ATTACKING) {
		if (TargetInRange(currTarget) && currTarget->IsDead() == false) {
			AttackTarget(dt);
		}
		else {
			currTarget = nullptr;
			unitState = unit_state::IDLE;
		}
	}
	else if (unitState < unit_state::ATTACKING && unitAggro > unit_aggro::PASSIVE) {
		FindEnemies(dt);
	}

	else {
		if (NodeReached() == false) {
			Move(dt);
		}
		else {
			currNode = next(currNode);

			if (DestinationReached() == false) {
				SetupVecSpeed();
			}
			else {
				StartHold();
			}
		}
	}
}

void Unit::DoHunt(float dt)
{
	if (huntTarget->IsDead() == false) {
		if (huntTarget->IsVisible() == false) {	// If target gets in Fog of War and comes out, restart Attack Order to target
			targetLost = true;
		}
		else if (targetLost == true && huntTarget->IsVisible() == true) {
			if (TargetDisplaced(huntTarget) == true) {
				StartHunt(huntTarget);
			}
			else {
				targetLost = false;
			}
		}
		
		if (TargetInRange(huntTarget) == false) {
			
			if (unitState == unit_state::ATTACKING) {
				if (TargetInRange(currTarget) && currTarget->IsDead() == false) {
					AttackTarget(dt);
				}
				else {
					currTarget = nullptr;
					unitState = unit_state::IDLE;
				}
			}
			else if (unitState < unit_state::ATTACKING && unitAggro > unit_aggro::PASSIVE) {
				currTarget = EnemyInRadius(attackRadius);
				if (currTarget != nullptr) {
					AttackTarget(dt);
				}
				else if (unitAggro == unit_aggro::AGRESSIVE) {
					currTarget = EnemyInRadius(visionRadius);
					if (currTarget != nullptr) {
						StartAggroHunt(currTarget);
					}
				}
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
					StartHunt(currTarget);
				}
			}
		}
		else {

			if (aggroTriggered) {
				switch (prevOrder) {
				case unit_orders::HOLD:
					break;
				case unit_orders::HOLD:
					break;
				case unit_orders::HOLD:
					break;
				case unit_orders::HOLD:
					break;
				case unit_orders::HOLD:
					break;
				}
			}

			unit_orders prevOrder;			// Order given before aggro
			bool aggroTriggered = false;	// Aggro flag

			currTarget = huntTarget;
			TryAttack(dt);
		}
	}
	else {
		if ()
		huntTarget = nullptr;
		StartHold();
	}
}

void Unit::DoPatrol(float dt)
{
	if (NodeReached() == false) {
		if (unitState == unit_state::IDLE || unitState == unit_state::MOVING) {
			currTarget = EnemyInRange();
			if (currTarget != nullptr) {
				TryAttack(dt);
			}
			else if (faction == entity_faction::CAPITALIST && BaseInRange()) {	//TODO-Carles: Checking it's faction inside the worflow to do stuff is bad
				AttackBase(dt);	//TODO: Should be TryAttack, not base specifically
			}
			else {
				Move(dt);
			}
		}
		else if (unitState == unit_state::ATTACKING) {
			if (TargetInRange() && currTarget->IsDead() == false) {
				TryAttack(dt);
			}
			else if (faction == entity_faction::CAPITALIST && BaseInRange()) {	//TODO-Carles: Checking it's faction inside the worflow to do stuff is bad
				AttackBase(dt);	//TODO: Should be TryAttack, not base specifically
			}
			else {
				currTarget = nullptr;
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
	position.x += (vecSpeed.x * dt);
	position.y += (vecSpeed.y * dt);
	centerPos.x += (vecSpeed.x * dt);
	centerPos.y += (vecSpeed.y * dt);

	unitState = unit_state::MOVING;
	return true;
}

void Unit::PerformAttack(float dt)
{

}

bool Unit::FindEnemies(float dt)
{
	bool ret = false;

	currTarget = EnemyInRadius(attackRadius);
	if (currTarget != nullptr) {
		AttackTarget(dt);
		ret = true;
	}
	else if (unitAggro == unit_aggro::AGRESSIVE) {
		currTarget = EnemyInRadius(visionRadius);
		if (currTarget != nullptr) {
			//TODO: Prepare Hunt
			ret = true;
		}
	}

	return ret;
}

void Unit::AttackTarget(float dt)
{
	fVec2 targetDirection = GetVector2(centerPos, currTarget->centerPos);
	unit_directions lastDirection = unitDirection;
	CheckDirection(targetDirection);

	if (lastDirection != unitDirection) {
		UpdateAnimation();
	}

	currTarget->Hurt((float)damage * dt);

	if () {
		myApp->audio->PlayFx(myApp->audio->SoundFX_Array[(int)infantryType][(int)faction][(int)type_sounds::SHOT][2]);	//TODO: Make variable dependable
		
	}

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

	myApp->entities->mainBase->health -= (float)(damage * dt);

	if (myApp->entities->mainBase->health < 0) {
		//TODO: Lose flag
	}

	if (unitState != unit_state::ATTACKING) {
		myApp->audio->PlayFx(myApp->audio->SoundFX_Array[(int)infantryType][(int)faction][(int)type_sounds::SHOT][2]);
		unitState = unit_state::ATTACKING;
	}
}

float Unit::Hurt(float damage)
{
	health -= damage;

	if (health <= 0.0f) {
		Die();
	}

	return health;
}

void Unit::Die()
{
	despawnTimer.Start();
	unitOrders = unit_orders::NONE;
	unitState = unit_state::DEAD;
	currentAnimation = (&myApp->entities->animationArray[int(infantryType)][int(unitState)][0]);

	myApp->audio->PlayFx(myApp->audio->SoundFX_Array[(int)infantryType][(int)faction][(int)type_sounds::HURT][rand() % 2]);
}

// Unit Data
bool Unit::IsDead()
{
	if (health <= 0.0f || unitState == unit_state::DEAD) {
		return true;
	}
	else {
		return false;
	}
}

bool Unit::InsideCamera()
{
	bool ret = false;

	if (myApp->render->InsideCamera({ (int)centerPos.x,(int)centerPos.y, UnitBlitRect.w, UnitBlitRect.h }) == true) {
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

	if (vecSpeed.x > 0.0f) {
		if (centerPos.x >= (float)currNode->x) {
			ret = true;
		}
	}
	else if (vecSpeed.x < 0.0f) {
		if (centerPos.x <= (float)currNode->x) {
			ret = true;
		}
	}

	if (vecSpeed.y > 0.0f) {
		if (centerPos.y >= (float)currNode->y) {
			ret = true;
		}
	}
	else if (vecSpeed.y < 0.0f) {
		if (centerPos.y <= (float)currNode->y) {
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

bool Unit::TargetDisplaced(Unit* target)
{
	bool ret = false;

	if (target->position.x != destination.x || target->position.y != destination.y) {
		ret = true;
	}

	return ret;
}

// Unit Calculations
Unit* Unit::EnemyInRadius(uint radius)
{
	Unit* ret = nullptr;

	for (int i = 0; i < INFANTRY_ARRAY_SIZE; ++i) {	//TODO-Carles: This is real fucking messy and expensive on runtime, requires list of active units, one for each side
		if (hostileUnits[i]->active == true && hostileUnits[i]->IsDead() == false) {

			if (InsideSquareRadius(position, (float)radius, hostileUnits[i]->position)
				&& InsideRadius(position, (float)radius, hostileUnits[i]->position))
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
				if (InsideSquareRadius(position, (float)attackRange, hostileUnits[i]->position) == false) {
					continue;
				}
				else {
					if (InsideRadius(position, (float)attackRange, hostileUnits[i]->position) == true) {
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
	if (InsideSquareRadius(position, (float)attackRadius, trueBasePos)
		&& InsideRadius(position, (float)attackRadius, trueBasePos))
	{
		ret = true;
	}

	return ret;
}

bool Unit::TargetInRange(Unit* target)
{
	return InsideRadius(position, (float)attackRadius, target->position);
}

void Unit::SetupPath(iPoint origin, iPoint destination)
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

	vecSpeed = GetVector2(position, nodePos);
	vecSpeed = vecSpeed.GetUnitVector();
	vecSpeed *= linSpeed;
	return vecSpeed;
}

// Order calling
void Unit::StartHold()
{
	origin = destination = { (int)centerPos.x, (int)centerPos.y };

	unitOrders = unit_orders::HOLD;
	unitState = unit_state::IDLE;

	UpdateAnimation();
}

void Unit::StartMove(iPoint destination)
{
	origin = { (int)centerPos.x, (int)centerPos.y };
	this->destination = destination;

	SetupPath(origin, destination);

	unitOrders = unit_orders::MOVE;
	unitState = unit_state::IDLE;

	UpdateAnimation();
}

void Unit::StartHunt(Unit* target)
{
	this->huntTarget = target;
	targetLost = false;

	origin = { (int)centerPos.x, (int)centerPos.y };
	this->destination = { (int)target->centerPos.x, (int)target->centerPos.y };

	SetupPath(origin, destination);

	unitOrders = unit_orders::HUNT;
	unitState = unit_state::IDLE;

	UpdateAnimation();
}

void Unit::StartAggroHunt(Unit* target)
{
	this->huntTarget = target;
	targetLost = false;

	aggroDestination = { (int)target->centerPos.x, (int)target->centerPos.y };
	aggroTriggered = true;

	SetupPath({ centerPos.x, centerPos.y }, aggroDestination);

	prevOrder = unitOrders;
	unitOrders = unit_orders::HUNT;
	unitState = unit_state::IDLE;

	UpdateAnimation();
}

void Unit::StartPatrol(iPoint destination)
{
	origin = { (int)centerPos.x, (int)centerPos.y };
	this->destination = destination;

	SetupPath(origin, destination);

	unitOrders = unit_orders::PATROL;
	unitState = unit_state::IDLE;

	UpdateAnimation();
}