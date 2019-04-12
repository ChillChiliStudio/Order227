#include "Geometry.h"
#include "Render.h"
#include "Textures.h"
#include "Scene.h"
#include "Pathfinding.h"
#include "EntityManager.h"
#include "Entity.h"
#include "Unit.h"

Unit::Unit(unit_type unitType, fPoint pos, faction_enum faction) : Entity(entity_type::UNIT, pos), unitFaction(faction), unitType(unitType)
{
	life = 1;

	if (faction == faction_enum::CAPITALIST) {

		switch (unitType) {

		case (unit_type::INFANTRY):
			texture = myApp->scene->TestTexture;
			break;
		case(unit_type::UNKNOWN):
			break;
		default:
			break;
		}

		//hostileUnits = &myApp->entities->enemiesList;
	}
	else if (faction == faction_enum::COMMUNIST) {

		switch (unitType) {

		case (unit_type::INFANTRY):
			texture = myApp->scene->TestTexture;
			break;
		case(unit_type::UNKNOWN):
			break;
		default:
			break;
		}
	}
}

Unit::~Unit()
{}

bool Unit::Update(float dt)
{
	UnitWorkflow(dt);

	unitRect = {12, 0, 55,47};
	CheckInCamera = {(int)position.x, (int)position.y, unitRect.w, unitRect.h };
//	Draw();

	if (life <= 0)	//TODO: This should be included inside the workflow AND must work with entity pools
		myApp->entities->DestroyEntity(this);

	if (myApp->render->InsideCamera(CheckInCamera) == true) {
		Draw();
	}

	if (life <= 0)	//TODO: This should be included inside the workflow AND must work with entity pools
		myApp->entities->DestroyEntity(this);
	
	return true;
}
  
bool Unit::Draw() 
{
	//myApp->render->Blit(texture, (int)position.x, (int)position.y,&UnitBlitRect);
	UpdateBlitOrder();

	myApp->render->Push(order, texture, position.x, position.y, &UnitBlitRect);

	if (selected) {
		myApp->render->DrawQuad(UnitRect, 255, 0, 0, 255, false);
	}
	
	return true;
}

void Unit::UpdateBlitOrder() 
{
	std::list<Entity*>::iterator item = myApp->entities->entities_list.begin();
	while (item != myApp->entities->entities_list.end()) {

		if ((*item) != this) {

			if (this->position.y > (*item)->position.y)
				order += 1;
			else
				order -= 1;
		}
    
		item = next(item);
	}
}

// Main workflow
void Unit::UnitWorkflow(float dt)
{
	unit_state prevState = unitState;

	switch (unitOrders) {
	case unit_orders::HOLD:
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
	}

	if (prevState != unitState) {
		ApplyState();
	}
}

void Unit::ApplyState()
{
	switch (unitState) {
	case unit_state::IDLE:
		//currentAnim = idleAnim;
		break;
	case unit_state::MOVING:
		//currentAnim = movingAnim;
		break;
	case unit_state::FIRING:
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
			AttackTarget();
		}
		break;
	case unit_state::FIRING:
		if (TargetInRange() && target->IsDead() == false) {
			AttackTarget();
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
	if (NodeReached() == false) {	//NOTE: Pathfinding should define destination as the tile where a specific unit should be even if it's in a group
		Move(dt);
	}
	else {
		currNode++;

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
	if (target->IsVisible() == true) {
		if (TargetDisplaced() == true) {
			origin = { (int)position.x, (int)position.y };
			destination = { (int)target->position.x, (int)target->position.y };
			//TODO (LuchoAlert): Recalculate unit pathfinding using origin and destination
		}
	}

	if (target->IsDead() == false) {
		if (TargetInRange() == false) {
			Move(dt);
		}
		else {
			AttackTarget();
		}
	}
	else {
		StartHold();
	}
}

void Unit::DoMoveAndAttack(float dt)
{
	if (NodeReached() == false) {	//NOTE: Pathfinding should define destination as the tile where a specific unit should be even if it's in a group
		if (unitState == unit_state::IDLE || unitState == unit_state::MOVING) {
			target = EnemyInRange();
			if (target != nullptr) {
				AttackTarget();
			}
			else {
				Move(dt);
			}
		}
		else if (unitState == unit_state::FIRING) {
			if (TargetInRange() && target->IsDead() == false) {
				AttackTarget();
			}
			else {
				target = nullptr;
				Move(dt);
				//TODO: Recalculate vecSpeed?
			}
		}
	}
	else {
		currNode++;

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
	if (NodeReached() == false) {	//NOTE: Pathfinding should define nextNode as the tile where a specific unit should be even if it's in a group
		if (unitState == unit_state::IDLE || unitState == unit_state::MOVING) {
			target = EnemyInRange();
			if (target != nullptr) {
				AttackTarget();
			}
			else {
				Move(dt);
			}
		}
		else if (unitState == unit_state::FIRING) {
			if (TargetInRange() && target->IsDead() == false) {
				AttackTarget();
			}
			else {
				target = nullptr;
				Move(dt);
				//TODO: Recalculate vecSpeed?
			}
		}
	}
	else {
		currNode++;

		if (DestinationReached() == false) {
			SetupVecSpeed();
		}
		else {
			StartPatrol(origin);
		}
	}
}

// Actions
bool Unit::Move(float dt)	//TODO: Make it so unit goes straight to the nextNode (divide speed into x and y, and use hipotenuse angle to decide how much it applies to each)
{
	position.x += (vecSpeed.x * dt);
	position.y += (vecSpeed.y * dt);

	unitState = unit_state::MOVING;
	return true;
}

void Unit::AttackTarget()
{
	//TODO: Enemy interaction
	unitState = unit_state::FIRING;
}

// Unit Data
bool Unit::IsDead()
{
	if (life <= 0) {
		return true;
	}
	else {
		return false;
	}
}

bool Unit::IsVisible()
{
	return true;	//TODO: Make function
}

bool Unit::NodeReached()
{
	bool ret = false;

	if (vecSpeed.x > 0.0f) {
		if (position.x >= (float)currNode->x) {
			ret = true;
		}
	}
	else if (vecSpeed.x < 0.0f) {
		if (position.x <= (float)currNode->x) {
			ret = true;
		}
	}

	if (vecSpeed.y > 0.0f) {
		if (position.y >= (float)currNode->y) {
			ret = true;
		}
	}
	else if (vecSpeed.y < 0.0f) {
		if (position.y <= (float)currNode->y) {
			ret = true;
		}
	}

	return ret;
}

bool Unit::DestinationReached()
{
	if (currNode == nodeList.end()) {
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

	/*for (std::list<Unit*>::iterator iter = hostileUnits->begin(); iter != hostileUnits->end(); ++iter) {
		if ((*iter)->position.x > position.x + attackRange || (*iter)->position.x < position.x - attackRange
			|| (*iter)->position.y > position.y + attackRange || (*iter)->position.y < position.y + attackRange) {
			continue;
		}
		else {
			if (InsideRadius(position, attackRange, (*iter)->position) == true) {
				ret = (*iter);
				break;
			 }
		}
	}*/

	return ret;
}

bool Unit::TargetInRange()
{
	return InsideRadius(position, attackRange, target->position);
}

fVec2 Unit::SetupVecSpeed()
{
	iPoint iPos = { (int)position.x, (int)position.y };

	vecSpeed = GetVector2(iPos, *currNode);
	vecSpeed = vecSpeed.GetUnitVector();
	vecSpeed *= linSpeed;
	return vecSpeed;
}

// Order calling
void Unit::OrderStandardSetup(iPoint destination)
{
	nodeList.clear();
	target = nullptr;

	origin = { (int)position.x, (int)position.y };
	this->destination = destination;

	myApp->pathfinding->CreatePath(origin, destination);
	nodeList = *myApp->pathfinding->GetLastPath();
	currNode = nodeList.begin();
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
	iPoint targetPos = { (int)target->position.x, (int)target->position.y };
	OrderStandardSetup(targetPos);

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