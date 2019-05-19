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

	currNode = unitPath.end();

	currentAnimation = (&myApp->entities->animationArray[int(infantryType)][int(unitState)][int(unitDirection)][(int)faction]);

	if (faction == entity_faction::COMMUNIST) {

		int Aux = myApp->audio->VarsXsound[int(infantryType)][(int)type_sounds::SPAWN];
		myApp->audio->PlayFx(myApp->audio->SoundFX_Array[(int)infantryType][(int)type_sounds::SPAWN][rand() % Aux], 0, CHANNEL_SPAWN);
	}


	myApp->gui->CreateLifeBar(fPoint(centerPos.x, position.y), this, myApp->entities->lifeBar_tex);

	active = true;
	selected = false;

	return true;
}

bool Unit::Update(float dt)
{
	BROFILER_CATEGORY("Unit Update", Profiler::Color::Orange);

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
		if (currNode != unitPath.end()) {
			if (myApp->entities->entitiesDebugDraw || faction == entity_faction::COMMUNIST) {
				DrawPath();
			}
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

	myApp->render->DrawLine((int)groundPos.x, (int)groundPos.y, (*currNode).x, (*currNode).y, rgb[0], rgb[1], rgb[2], 255, true);

	if (unitPath.size() > 1) {
		for (std::vector<iPoint>::iterator it = currNode; next(it) != unitPath.end(); it = next(it)) {
			myApp->render->DrawLine((*it).x, (*it).y, (*next(it)).x, (*next(it)).y, rgb[0], rgb[1], rgb[2], 255, true);
		}
	}
}

// Main workflow
void Unit::UnitWorkflow(float dt)
{
	BROFILER_CATEGORY("Unit Workflow", Profiler::Color::ForestGreen);

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
		if (targetLost == false && huntTarget->IsVisible() == false) {	// If target gets in Fog of War and comes out, restart Attack Order to target
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
		else if (TargetInRange(huntTarget) == false) {

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
		currTarget = nullptr;
		huntTarget = nullptr;
		StartHold();
	}
}

void Unit::DoAggroHunt(float dt)
{
	if (aggroTarget != nullptr && aggroTarget->IsDead() == false && aggroTarget->IsVisible() == true) {
		if (TargetInRange(aggroTarget) == true) {
			currTarget = aggroTarget;
			AttackCurrTarget(dt);
		}
		else if (unitState == unit_state::ATTACKING) {
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
		currTarget = nullptr;
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
	fVec2 distanceMoved = { vecSpeed.x * dt, vecSpeed.y * dt };

	position.x += distanceMoved.x;
	position.y += distanceMoved.y;
	centerPos.x += distanceMoved.x;
	centerPos.y += distanceMoved.y;
	groundPos.x += distanceMoved.x;
	groundPos.y += distanceMoved.y;

	unitState = unit_state::MOVING;

	return true;
}

bool Unit::FindEnemies(float dt)
{
	BROFILER_CATEGORY("Unit FindEnemies", Profiler::Color::Aqua);

	bool ret = false;

	if (unitAggro > unit_aggro::PASSIVE) {
		currTarget = EnemyInRadius(stats.attackRadius);
		if (currTarget != nullptr) {
			AttackCurrTarget(dt);
			ret = true;
		}
		else if (aggroTriggered == false) {
			Unit* tmpPtr;

			tmpPtr = AttackingAllyInRadius(stats.attackRadius);
			if (tmpPtr != nullptr) {
				StartAggroHunt(tmpPtr->currTarget);
				ret = true;
			}
			else if (unitAggro == unit_aggro::AGGRESSIVE) {
				aggroTarget = EnemyInRadius(stats.visionRadius);
				if (aggroTarget != nullptr) {
					StartAggroHunt(aggroTarget);
					ret = true;
				}
				else {
					tmpPtr = AttackingAllyInRadius(stats.visionRadius);
					if (tmpPtr != nullptr) {
						StartAggroHunt(tmpPtr->currTarget);
						ret = true;
					}
				}
			}
		}
	}

	return ret;
}

//Optimization Alternative (DO NOT DELETE)
//bool Unit::FindEnemies(float dt)
//{
//	BROFILER_CATEGORY("Unit FindEnemies", Profiler::Color::Aqua);
//
//	bool ret = false;
//
//	if (unitAggro > unit_aggro::PASSIVE) {
//		currTarget = UnitInRadius(stats.attackRadius);
//		if (currTarget != nullptr) {
//			AttackCurrTarget(dt);
//			ret = true;
//		}
//		else if (aggroTriggered == false && unitAggro == unit_aggro::AGGRESSIVE) {
//
//			Entity* tmp = nullptr;
//
//			tmp = UnitInRadius(stats.visionRadius);
//			if (tmp != nullptr) {
//				StartAggroHunt(tmp);
//				ret = true;
//			}
//		}
//	}
//
//	return ret;
//}

void Unit::AttackCurrTarget(float dt)
{
	fVec2 targetDirection = GetVector2(centerPos, currTarget->centerPos);
	unit_directions lastDirection = unitDirection;
	CheckDirection(targetDirection);

	if (lastDirection != unitDirection) {
		UpdateAnimation();
	}

	if (unitState != unit_state::ATTACKING) {
		attackTimer.Start();
		unitState = unit_state::ATTACKING;
	}
	else if (attackTimer.Read() > stats.cadency) {

		currTarget->Hurt((float)stats.damage * dt);

		int Aux = myApp->audio->VarsXsound[int(infantryType)][(int)type_sounds::SHOT];
		myApp->audio->PlayFx(myApp->audio->SoundFX_Array[(int)infantryType][(int)type_sounds::SHOT][rand() % Aux], 0, CHANNEL_SHOT, centerPos, true);

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

	int Aux = myApp->audio->VarsXsound[int(infantryType)][(int)type_sounds::HURT];
	myApp->audio->PlayFx(myApp->audio->SoundFX_Array[(int)infantryType][(int)type_sounds::HURT][rand() % Aux], 0, CHANNEL_HURT, centerPos, true);
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
		if (groundPos.x >= (float)currNode->x) {
			ret = true;
		}
	}
	else if (vecSpeed.x < 0.0f) {
		if (groundPos.x <= (float)currNode->x) {
			ret = true;
		}
	}

	if (vecSpeed.y > 0.0f) {
		if (groundPos.y >= (float)currNode->y) {
			ret = true;
		}
	}
	else if (vecSpeed.y < 0.0f) {
		if (groundPos.y <= (float)currNode->y) {
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

	if (target->groundPos.x != destination.x || target->groundPos.y != destination.y) {
		ret = true;
	}

	return ret;
}

 //Unit Calculations
Entity* Unit::EnemyInRadius(uint radius)
{
	Entity* ret = nullptr;

	int numActives;

	//Units
	numActives = myApp->entities->activeUnits;
	for (std::vector<Unit>::iterator item = myApp->entities->unitPool.begin(); numActives > 0; item = next(item)) {
		if ((*item).active) {
			numActives--;

			if ((*item).IsDead() == false && (*item).faction != faction) {

				if (InsideSquareRadius(centerPos, (float)radius, (*item).centerPos)
					&& InsideRadius(centerPos, (float)radius, (*item).centerPos))
				{
					ret = (Entity*)&(*item);
					break;
				}
			}
		}
	}

	//Launchers
	if (ret == nullptr) {
		numActives = myApp->entities->activeLaunchers;

		for (std::vector<Launcher>::iterator item = myApp->entities->launcherPool.begin(); numActives > 0; item = next(item)) {
			if ((*item).active) {
				numActives--;

				if ((*item).IsDead() == false && (*item).faction != faction) {

					if (InsideSquareRadius(centerPos, (float)radius, (*item).centerPos)
						&& InsideRadius(centerPos, (float)radius, (*item).centerPos))
					{
						ret = (Entity*)&(*item);
						break;
					}
				}
			}
		}

		//Buildings
		if (ret == nullptr && faction == entity_faction::CAPITALIST) {
			numActives = myApp->entities->activeBuildings;

			for (std::vector<Building>::iterator item = myApp->entities->buildingsArray.begin(); numActives > 0; item = next(item)) {
				if ((*item).active) {
					numActives--;

					if ((*item).IsDead() == false) {

						if (InsideSquareRadius(centerPos, (float)radius, (*item).centerPos)
							&& InsideRadius(centerPos, (float)radius, (*item).centerPos))
						{
							ret = (Entity*)&(*item);
							break;
						}
					}
				}
			}
		}
	}

	return ret;
}

//Optimization Alternative (DO NOT DELETE)
//Entity* Unit::UnitInRadius(uint radius)	//If enemy in radius, return it. If ally, hunt target if any and return nullptr
//{
//	Entity* ret = nullptr;
//
//	int numActives = myApp->entities->activeUnits;
//
//	//Units
//	for (std::vector<Unit>::iterator item = myApp->entities->unitPool.begin(); numActives > 0; item = next(item)) {
//		if ((*item).active) {
//			numActives--;
//
//			if ((*item).IsDead() == false) {
//
//				if (InsideSquareRadius(centerPos, (float)radius, (*item).centerPos)	// Unit inside radius
//					&& InsideRadius(centerPos, (float)radius, (*item).centerPos))
//				{
//					if ((*item).faction != faction) {	// If enemy, it becomes currTarget
//						ret = (Entity*)&(*item);
//						break;
//
//					}
//					else if (aggroTriggered == false && (*item).currTarget != nullptr) {	// If ally attacking enemy unit, start hunt against enemy unit
//						StartAggroHunt((*item).currTarget);
//						return ret;
//					}
//				}
//			}
//		}
//	}
//
//	//Buildings
//	if (ret == nullptr && faction == entity_faction::CAPITALIST) {
//		numActives = myApp->entities->activeBuildings;
//
//		for (std::vector<Building>::iterator item = myApp->entities->buildingsArray.begin(); numActives > 0; item = next(item)) {
//			if ((*item).active) {
//				numActives--;
//
//				if ((*item).IsDead() == false) {
//
//					if (InsideSquareRadius(centerPos, (float)radius, (*item).centerPos)
//						&& InsideRadius(centerPos, (float)radius, (*item).centerPos))
//					{
//						ret = (Entity*)&(*item);
//						break;
//					}
//				}
//			}
//		}
//	}
//
//	return ret;
//}

Unit* Unit::AttackingAllyInRadius(uint radius)
{
	Unit* ret = nullptr;

	for (std::vector<Unit>::iterator item = myApp->entities->unitPool.begin(); item != myApp->entities->unitPool.end(); item = next(item)) {
		if ((*item).active == true && (*item).IsDead() == false && (*item).faction == faction && (*item).currTarget != nullptr) {

			if (InsideSquareRadius(centerPos, (float)radius, (*item).centerPos)
				&& InsideRadius(centerPos, (float)radius, (*item).centerPos))
			{
				ret = &(*item);
				break;
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

bool Unit::SetupPath(iPoint origin, iPoint destination)
{
	bool ret = true;

	unitPath.clear();

	iPoint mapOrigin = myApp->map->WorldToMap(origin.x, origin.y);
	iPoint mapDestination = myApp->map->WorldToMap(destination.x, destination.y);

	if (mapOrigin != mapDestination) {

		destination = myApp->map->MapToWorld(mapDestination);	//Change destination to mapDestination tile center's world pos
		destination.y += 15;

		if (TryLinearPath(origin, destination)) {
			unitPath.push_back(origin);
			unitPath.push_back(destination);
		}
		else {
			myApp->pathfinding->CreatePath(mapOrigin, mapDestination);	//Create path
			unitPath = *myApp->pathfinding->GetLastPath();

			for (int i = 0; i < unitPath.size(); i++) {					//Translate and correct all in-between nodes
				unitPath[i] = myApp->map->MapToWorld(unitPath[i].x, unitPath[i].y);
				unitPath[i].y += 15;	//Move nodes to the center of the tile (15 = tile_height / 2)
			}
		}

		currNode = next(unitPath.begin());	// Unit should move directly to 2nd node, as 1st is curr position
		SetupVecSpeed();
	}
	else {	//If origin == destination, do nothing
		ret = false;
	}

	return ret;
}

bool Unit::TryLinearPath(iPoint origin, iPoint destination)
{
	bool ret = true;

	iVec2 vec(GetVector2(origin, destination));

	if (vec.GetMagnitude() < myApp->pathfinding->GetLinPathRadius()) {

		fVec2 unitVec = vec.GetUnitVector();
		fPoint pathChecker = { (float)origin.x, (float)origin.y };

		for (int i = 0; i < myApp->pathfinding->GetLinPathRadius(); i += 30) {	//30 is the height of a tile, going in 30 intervals secures that no tile is skipped
			pathChecker.x += 30.0f * unitVec.x;
			pathChecker.y += 30.0f * unitVec.y;

			if (!myApp->pathfinding->IsWalkable(myApp->map->WorldToMap((int)pathChecker.x, (int)pathChecker.y))) {
				ret = false;
				break;
			}
		}
	}
	else {
		ret = false;
	}

	return ret;
}

fVec2 Unit::SetupVecSpeed()
{
	fPoint nodePos = { (float)(currNode->x), (float)(currNode->y) };

	vecSpeed = GetVector2(groundPos, nodePos);
	vecSpeed = vecSpeed.GetUnitVector();
	vecSpeed *= stats.linSpeed;
	return vecSpeed;
}

// Order calling
bool Unit::StartHold()
{
	if (unitAggro == unit_aggro::PASSIVE) {
		unitAggro = unit_aggro::DEFENSIVE;
	}

	origin = destination = { (int)groundPos.x, (int)groundPos.y };

	unitOrders = unit_orders::HOLD;
	unitState = unit_state::IDLE;

	UpdateAnimation();

	return true;
}

bool Unit::StartMove(iPoint destination)
{
	bool ret = false;

	if (SetupPath({ (int)groundPos.x, (int)groundPos.y }, destination)) {

		origin = { (int)groundPos.x, (int)groundPos.y };
		this->destination = destination;

		unitOrders = unit_orders::MOVE;
		unitState = unit_state::IDLE;

		UpdateAnimation();

		ret = true;
	}

	return ret;
}

bool Unit::StartHunt(Entity* target)
{
	this->huntTarget = target;
	targetLost = false;

	origin = { (int)groundPos.x, (int)groundPos.y };
	this->destination = { (int)target->groundPos.x, (int)target->groundPos.y };

	SetupPath(origin, destination);

	unitOrders = unit_orders::HUNT;
	unitState = unit_state::IDLE;

	UpdateAnimation();

	return true;
}

bool Unit::StartAggroHunt(Entity* target)	//NOTE: We don't touch origin or destination, as it's info related to the previous order we'll use to resume it later
{
	this->aggroTarget = target;
	targetLost = false;

	aggroDestination = { (int)target->groundPos.x, (int)target->groundPos.y };

	SetupPath({ (int)groundPos.x, (int)groundPos.y }, aggroDestination);

	if (aggroTriggered == false) {	// If it's the fist time aggro-hunting, save previous order
		aggroTriggered = true;
		prevOrder = unitOrders;
	}

	unitOrders = unit_orders::HUNT;
	unitState = unit_state::IDLE;

	UpdateAnimation();

	return true;
}

bool Unit::StartPatrol(iPoint destination)
{
	bool ret = StartMove(destination);

	if (ret) {
		unitOrders = unit_orders::PATROL;
	}

	return ret;
}

bool Unit::ResumePatrol()
{
	SetupPath({ (int)groundPos.x, (int)groundPos.y }, destination);

	unitOrders = unit_orders::PATROL;
	unitState = unit_state::IDLE;

	UpdateAnimation();

	return true;
}

bool Unit::ResumeLastOrder()
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

	UpdateAnimation();

	prevOrder = unit_orders::NONE;

	return true;
}
