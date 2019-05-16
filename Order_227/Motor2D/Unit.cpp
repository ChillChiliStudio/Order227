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
#include "UserInterface.h"
#include "Image.h"
#include "Brofiler/Brofiler.h"
Unit::Unit()
{};

Unit::Unit(fPoint pos, infantry_type infType, entity_faction faction) : Entity(pos, entity_type::INFANTRY, faction)
{
//	LoadEntityData();
	infantryType = infType;
	
}

Unit::~Unit()
{}

bool Unit::Start()
{
	entityRect = { (int)position.x, (int)position.y, 25, 35 };	//Todo: Use XML values for rect width, not hardcoded ones (but take these as reference)

	centerPos = { position.x + entityRect.w / 2, position.y + entityRect.h / 2 };
	groundPos = { position.x + entityRect.w / 2, position.y + entityRect.h };

	unitState = unit_state::IDLE;
	unitOrders = unit_orders::HOLD;
	unitDirection = unit_directions::SOUTH_EAST;

	currentAnimation = (&myApp->entities->animationArray[int(infantryType)][int(unitState)][int(unitDirection)][(int)faction]);
	stats.attackSfxId = myApp->audio->SoundFX_Array[(int)infantryType][(int)faction][(int)type_sounds::SHOT][2];	//TODO: Hardcoded audio value, this should be get by an XML

	myApp->gui->CreateLifeBar(fPoint(centerPos.x, position.y), this, myApp->entities->lifeBar_tex);

	active = true;
	selected = false;

	return true;
}

bool Unit::Update(float dt)
{
	BROFILER_CATEGORY("Unit.cpp Update()-DarkGreen", Profiler::Color::DarkGreen);
	onCamera = InsideCamera();

	entityRect.x = position.x;
	entityRect.y = position.y;

	UnitWorkflow(dt);

	if (mustDespawn) {
		/*if (currentAnimation.Finished() == true) {*/
			mustDespawn = false;
			myApp->entities->DeActivateUnit(this);
		/*}*/
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

			Draw();

			if (selected) {	//TODO: This should be as a debug functionality, but for now it'll do
				myApp->render->DrawQuad(entityRect, 0, 255, 0, 255, false);
			}

			if (myApp->entities->entitiesDebugDraw) {
				DebugDraw();
			}
		}
	}

	return true;
}

bool Unit::Draw()
{
	spriteRect = currentAnimation.GetTheActualCurrentFrame();	//TODO: CARLESTODO Mark of blit update
	myApp->render->Push(order, texture, (int)position.x, (int)position.y, &spriteRect);

	return true;
}

bool Unit::DebugDraw()
{
	Uint8 rgb[3] = { 0, 0, 0 };

	switch (faction) {	//TODO-Carles: Checking it's faction inside the worflow to do stuff is bad
	case entity_faction::COMMUNIST:
		rgb[2] = 255;	//Blue
		break;
	case entity_faction::CAPITALIST:
		rgb[0] = 255;	//Red
		break;
	case entity_faction::NEUTRAL:
		rgb[0] = 255;
		rgb[2] = 255;	//Magenta
		break;
	}

	if (selected == false) {
		myApp->render->DrawQuad(entityRect, rgb[0], rgb[1], rgb[2], 255, false);
	}

	myApp->render->DrawCircle(centerPos.x, centerPos.y, stats.attackRadius, rgb[0], rgb[1], rgb[2], 127, true);
	myApp->render->DrawCircle(centerPos.x, centerPos.y, stats.visionRadius, rgb[0], rgb[1], rgb[2], 255, true);

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

	myApp->render->DrawLine((int)centerPos.x, (int)centerPos.y, (*currNode).x, (*currNode).y, rgb[0], rgb[1], rgb[2], 255, true);

	if (unitPath.size() > 1) {
		for (std::vector<iPoint>::iterator it = currNode; next(it) != unitPath.end(); it = next(it)) {
			myApp->render->DrawLine((*it).x, (*it).y, (*next(it)).x, (*next(it)).y, rgb[0], rgb[1], rgb[2], 255, true);
		}
	}
}

// Main workflow
void Unit::UnitWorkflow(float dt)
{
	BROFILER_CATEGORY("Unit.cpp UnitWorkflow()-Green", Profiler::Color::Green);
	unit_state prevState = unitState;

	switch (unitOrders) {
	case unit_orders::HOLD:	// Default order
		DoHold(dt);
		break;
	case unit_orders::MOVE:
		DoMove(dt);
		break;
	case unit_orders::HUNT:
		if (aggroTriggered) {
			DoAggroHunt(dt);
		}
		else {
			DoHunt(dt);
		}
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
	currentAnimation = (&myApp->entities->animationArray[int(infantryType)][int(unitState)][int(unitDirection)][(int)faction]);
}

// Order processing
void Unit::DoHold(float dt)
{
	if (unitState == unit_state::ATTACKING) {	//IMPROVE: Lots of repeated code. It works, but using a single function with parameter variations would be cleaner
		if (TargetInRange(currTarget) && currTarget->IsDead() == false) {
			AttackCurrTarget(dt);
		}
		else {
			currTarget = nullptr;
			unitState = unit_state::IDLE;
		}
	}
	else {
		FindEnemies(dt);
	}
}

void Unit::DoMove(float dt)
{
	if (unitState == unit_state::ATTACKING) {
		if (TargetInRange(currTarget) && currTarget->IsDead() == false) {
			AttackCurrTarget(dt);
		}
		else {
			currTarget = nullptr;
			unitState = unit_state::IDLE;
		}
	}
	else if (FindEnemies(dt) == false) {
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
	if (huntTarget != nullptr && huntTarget->IsDead() == false) {
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
					AttackCurrTarget(dt);
				}
				else {
					currTarget = nullptr;
					unitState = unit_state::IDLE;
				}
			}
			else if (FindEnemies(dt) == false) {
				if (NodeReached() == false) {
					Move(dt);
				}
				else {
					currNode = next(currNode);

					if (DestinationReached() == false) {
						SetupVecSpeed();
					}
					else {
						StartHunt(huntTarget);
					}
				}
			}
		}
		else {
			currTarget = huntTarget;
			AttackCurrTarget(dt);
		}
	}
	else {
		huntTarget = nullptr;
		StartHold();
	}
}

void Unit::DoAggroHunt(float dt)
{
	if (aggroTarget != nullptr && aggroTarget->IsDead() == false) {
		if (aggroTarget->IsVisible() == false) {	// If target gets in Fog of War and comes out, restart Attack Order to target
			ResumeLastOrder();
		}
		else if (TargetInRange(aggroTarget) == false) {

			if (unitState == unit_state::ATTACKING) {
				if (TargetInRange(currTarget) && currTarget->IsDead() == false) {
					AttackCurrTarget(dt);
				}
				else {
					currTarget = nullptr;
					unitState = unit_state::IDLE;
				}
			}
			else if (FindEnemies(dt) == false) {
				if (NodeReached() == false) {
					Move(dt);
				}
				else {
					currNode = next(currNode);

					if (DestinationReached() == false) {
						SetupVecSpeed();
					}
					else {
						StartAggroHunt(aggroTarget);
					}
				}
			}
		}
		else {
			currTarget = aggroTarget;
			AttackCurrTarget(dt);
		}
	}
	else {
		aggroTarget = nullptr;
		ResumeLastOrder();
	}
}

void Unit::DoPatrol(float dt)
{
	if (unitState == unit_state::ATTACKING) {
		if (TargetInRange(currTarget) && currTarget->IsDead() == false) {
			AttackCurrTarget(dt);
		}
		else {
			currTarget = nullptr;
			unitState = unit_state::IDLE;
		}
	}
	else if (FindEnemies(dt) == false) {
		if (NodeReached() == false) {
			Move(dt);
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
}

 //Actions
bool Unit::Move(float dt)
{
	position.x += (vecSpeed.x * dt);
	position.y += (vecSpeed.y * dt);
	centerPos.x += (vecSpeed.x * dt);
	centerPos.y += (vecSpeed.y * dt);
	groundPos.x += (vecSpeed.x * dt);
	groundPos.y += (vecSpeed.y * dt);

	unitState = unit_state::MOVING;
	return true;
}

bool Unit::FindEnemies(float dt)
{
	bool ret = false;

	if (unitAggro > unit_aggro::PASSIVE) {
		currTarget = EnemyInRadius(stats.attackRadius);
		if (currTarget != nullptr) {
			AttackCurrTarget(dt);
			ret = true;
		}
		else if (unitAggro == unit_aggro::AGRESSIVE && aggroTriggered == false) {
			aggroTarget = EnemyInRadius(stats.visionRadius);
			if (aggroTarget != nullptr) {
				StartAggroHunt(aggroTarget);
				ret = true;
			}
		}
	}

	return ret;
}

void Unit::AttackCurrTarget(float dt)
{
	fVec2 targetDirection = GetVector2(centerPos, currTarget->centerPos);
	unit_directions lastDirection = unitDirection;
	CheckDirection(targetDirection);

	if (lastDirection != unitDirection) {
		UpdateAnimation();
	}

	currTarget->Hurt((float)stats.damage * dt);

	if (unitState != unit_state::ATTACKING) {
		attackTimer.Start();
		unitState = unit_state::ATTACKING;
	}
	else if (attackTimer.Read() > stats.cadency) {
		myApp->audio->PlayFx(stats.attackSfxId);
		attackTimer.Start();
	}
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
	selected = false;

	despawnTimer.Start();
	unitOrders = unit_orders::NONE;
	unitState = unit_state::DEAD;

	
	currentAnimation = (&myApp->entities->animationArray[int(infantryType)][int(unitState)][0][(int)faction]);

	myApp->audio->PlayFx(myApp->audio->SoundFX_Array[(int)infantryType][(int)faction][(int)type_sounds::HURT][rand() % 2]);
}

 //Unit Data
bool Unit::IsDead()
{
	bool ret = false;

	if (unitState == unit_state::DEAD || stats.health <= 0.0f) {
		ret = true;
	}

	return ret;
}

bool Unit::InsideCamera()
{
	bool ret = false;

	if (myApp->render->InsideCamera(entityRect) == true) {
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

bool Unit::TargetDisplaced(Entity* target)
{
	bool ret = false;

	if (target->centerPos.x != destination.x || target->centerPos.y != destination.y) {
		ret = true;
	}

	return ret;
}

 //Unit Calculations
Entity* Unit::EnemyInRadius(uint radius)
{
	Entity* ret = nullptr;

	for (std::vector<Unit>::iterator item = myApp->entities->unitPool.begin(); item != myApp->entities->unitPool.end(); item = next(item)) {
		if ((*item).active == true && (*item).IsDead() == false && (*item).faction != faction) {

			if (InsideSquareRadius(centerPos, (float)stats.attackRadius, (*item).centerPos)
				&& InsideRadius(centerPos, (float)stats.attackRadius, (*item).centerPos))
			{
				ret = (Entity*)&(*item);
				break;
			}
		}
	}
	for (std::vector<Launcher>::iterator item = myApp->entities->launcherPool.begin(); item != myApp->entities->launcherPool.end(); item = next(item)) {
		if ((*item).active == true && (*item).IsDead() == false && (*item).faction != faction) {

			if (InsideSquareRadius(centerPos, (float)stats.attackRadius, (*item).centerPos)
				&& InsideRadius(centerPos, (float)stats.attackRadius, (*item).centerPos))
			{
				ret = (Entity*)&(*item);
				break;
			}
		}
	}
	if (ret == nullptr && faction == entity_faction::CAPITALIST) {
		for (std::vector<Building>::iterator item = myApp->entities->buildingsArray.begin(); item != myApp->entities->buildingsArray.end(); item = next(item)) {
			if ((*item).active == true && (*item).IsDead() == false) {

				if (InsideSquareRadius(centerPos, (float)stats.attackRadius, (*item).centerPos)
					&& InsideRadius(centerPos, (float)stats.attackRadius, (*item).centerPos))
				{
					ret = (Entity*)&(*item);
					break;
				}
			}
		}
	}

	return ret;
}

bool Unit::TargetInRange(Entity* target)
{
	bool ret = false;

	if (target != nullptr) {
		ret = InsideRadius(centerPos, (float)stats.attackRadius, target->centerPos);
	}

	return ret;
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

	vecSpeed = GetVector2(centerPos, nodePos);
	vecSpeed = vecSpeed.GetUnitVector();
	vecSpeed *= stats.linSpeed;
	return vecSpeed;
}

 //Order calling
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

void Unit::StartHunt(Entity* target)
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

void Unit::StartAggroHunt(Entity* target)	//NOTE: We don't touch origin or destination, as it's info related to the previous order we'll use to resume it later
{
	this->aggroTarget = target;
	targetLost = false;

	aggroDestination = { (int)target->centerPos.x, (int)target->centerPos.y };

	SetupPath({ (int)centerPos.x, (int)centerPos.y }, aggroDestination);

	if (aggroTriggered == false) {	// If it's the fist time aggro-hunting, save previous order
		aggroTriggered = true;
		prevOrder = unitOrders;
	}

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

void Unit::ResumePatrol()
{
	SetupPath({ (int)centerPos.x, (int)centerPos.y }, destination);

	unitOrders = unit_orders::PATROL;
	unitState = unit_state::IDLE;

	UpdateAnimation();
}

void Unit::ResumeLastOrder()
{
	aggroTriggered = false;
	unitOrders = prevOrder;

	switch (unitOrders) {
	case unit_orders::HOLD:
		StartMove(origin);
		break;
	case unit_orders::MOVE:
		StartMove(destination);
		break;
	case unit_orders::HUNT:
		StartHunt(huntTarget);
		break;
	case unit_orders::PATROL:
		ResumePatrol();
		break;
	}

	prevOrder = unitOrders;
	unitOrders = unit_orders::HUNT;
	unitState = unit_state::IDLE;

	SetupPath({ (int)centerPos.x, (int)centerPos.y }, aggroDestination);

	UpdateAnimation();

	prevOrder = unit_orders::NONE;
}
