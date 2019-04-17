


#include "GroupManager.h"
#include "Log.h"
#include "Entity.h"
#include "Scene.h"
#include "Render.h"
#include "App.h"
#include <assert.h>
#include "Input.h"
#include "Pathfinding.h"
#include "Player.h"
#include "Map.h"

GroupManager::~GroupManager() {

}

GroupManager::GroupManager() {

}

bool GroupManager::Awake() {

	LOG("AWAKING GROUP MANAGER");


	return true;
}


bool GroupManager::Start() {

	return true;
}


bool GroupManager::PreUpdate() {

	
	return true;
}

bool GroupManager::Update(float dt) {

	return true;
}


bool GroupManager::CleanUp() {

	LOG("Clean Up Group Manager");

	

	return true;
}

// SELECTION AND GROUPS SYSTEM BELLOW__________________________________________________________________________________

void GroupManager::SelectUnit(SDL_Rect rect) {
	for (std::list<Unit*>::iterator it = myApp->entities->comunist_list.begin(); it != myApp->entities->comunist_list.end(); it++) {
		SDL_Rect entityRect = (*it)->unitRect;
		//select the rectangle units
		if (SDL_HasIntersection(&entityRect, &rect)) {
			(*it)->selected = true;
			/*playerGroup.groupUnits_list.push_back((*it));*/
		}//deselect the outside rectangle units if shift button not pressed
		else if (!(SDL_HasIntersection(&entityRect, &rect)) && myApp->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_UP) {
			(*it)->selected = false;
		}

		
	}
}

void GroupManager::SelectUnit(iPoint pos) {
	int counter = 0;
	for (std::list<Unit*>::iterator it = myApp->entities->comunist_list.begin(); it != myApp->entities->comunist_list.end(); it++) {
		SDL_Rect entityRect = (*it)->unitRect;
		//if we are here is because we are selecting a single unit with a single click.
		if ((counter < 1) && pos.x > (*it)->unitRect.x && pos.x < (*it)->unitRect.x + (*it)->unitRect.w && pos.y >(*it)->unitRect.y && pos.y < (*it)->unitRect.y + (*it)->unitRect.h) {
			(*it)->selected = true;
			/*playerGroup.groupUnits_list.push_back((*it));*/
			counter++;
		}//if we are not clicking it and the shift is not pressed
		else if (myApp->input->GetKey(SDL_SCANCODE_LSHIFT) == SDL_RELEASED)
			(*it)->selected = false;

	}

}

void GroupManager::CreateGroupForPlayer() {

	if (playerGroup.groupUnits_list.size() != 0 && myApp->input->GetKey(SDL_SCANCODE_LSHIFT) == SDL_RELEASED) { //no shift pressed and try to select
		LOG("LETS DELETE THE CURRENT GROUP");
		EmptyPlayerGroup();
	}

	if (myApp->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) { //shift pressed and try to select
		LOG("LETS ADD UNITS TO THE GROUP");
		AddUnitsPlayerGroup();
	}

	else if (myApp->input->GetKey(SDL_SCANCODE_LSHIFT) == SDL_RELEASED && playerGroup.groupUnits_list.size() == 0) {
		LOG("LETS CREATE AND FILL A NEW GROUP IF THERE IS A SELECTION");
		AddUnitsPlayerGroup();
	}

}

void GroupManager::EmptyPlayerGroup() {

	std::list<Unit*>::iterator item = playerGroup.groupUnits_list.begin();
	if (*item != nullptr) {
		playerGroup.groupUnits_list.clear();
		LOG("DELETE THE GROUP");
	}
}

void GroupManager::AddUnitsPlayerGroup() {
	for (std::list<Unit*>::iterator iterator = myApp->entities->comunist_list.begin(); iterator != myApp->entities->comunist_list.end(); iterator++) {
		if ((*iterator)->selected == true) {
			playerGroup.groupUnits_list.push_back((*iterator));
			LOG("ADD UNIT TO THE GROUP");
		}
	}
}




void GroupManager::CreateDestination(iPoint SelectedPos) {
	iPoint curr;
	if (playerGroup.destinations_list.erase())
	{
		std::queue
		RELEASE(curr);
		iPoint neighbors[4];
		neighbors[0].create(curr.x + 1, curr.y + 0);
		neighbors[1].create(curr.x + 0, curr.y + 1);
		neighbors[2].create(curr.x - 1, curr.y + 0);
		neighbors[3].create(curr.x + 0, curr.y - 1);

		for (uint i = 0; i < 4; ++i)
		{
			if (MovementCost(neighbors[i].x, neighbors[i].y) > 0)
			{
				if (visited.find(neighbors[i]) == -1)
				{
					frontier.Push(neighbors[i], 0);
					visited.add(neighbors[i]);
				}
			}
		}
	}
}

void GroupManager::CreateGroupPaths(std::list<Unit*> list, iPoint destination) {
	for (std::list<Unit*>::iterator iterator = list.begin(); iterator != list.end(); iterator++) {
		(*iterator)->origin.x = (*iterator)->unitRect.x;
		(*iterator)->origin.y = (*iterator)->unitRect.y;
		(*iterator)->destination = destination;
		{
			myApp->pathfinding->CreatePath((*iterator)->origin, (*iterator)->destination);
			(*iterator)->Path.clear();
			(*iterator)->Path = myApp->pathfinding->last_path;
		}		
	}	
}


//void GroupManager::Move( float dt)
//{
//	//BROFILER_CATEGORY("GroupMovement::Move", Profiler::Color::Gold);
//
//	//group es playerGroup;
//	//movement_State
//	std::list <Unit*>::const_iterator unit = playerGroup.groupUnits_list.begin(); 
//
//	LOG("On Move Function");
//
//	iPoint Map_Entityposition;
//	fPoint distanceToNextTile;
//	iPoint next_tile_world;
//	float DirectDistance;
//	fPoint to_fPoint;
//	iPoint goal_world;
//
//	// --- We get the map coords of the mouse ---
//	iPoint Map_mouseposition;
//	Map_mouseposition = myApp->map->WorldToMap((int)myApp->player->mouseClickPos.x, (int)myApp->player->mouseClickPos.y);
//
//
//	while (unit != playerGroup.groupUnits_list.end())
//	{
//		// --- We Get the map coords of the Entity ---
//		
//		Map_Entityposition.x = (*unit)->position.x;
//		Map_Entityposition.y = (*unit)->position.y;
//		Map_Entityposition = myApp->map->WorldToMap(Map_Entityposition.x, Map_Entityposition.y);
//
//		if (myApp->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN && (*unit)->selected == true)
//			(*unit)->UnitMovementState = movement_State::MovementState_NoState;
//
//		switch ((*unit)->UnitMovementState)
//		{
//
//		case movement_State::MovementState_NoState:
//
//			// --- On call to Move, Units will request a path to the destination ---
//
//			if (myApp->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN && (*unit)->selected == true)
//			{
//
//				if (playerGroup.groupUnits_list.front() == (*unit)/*Group->IsGroupLead((*unit)) == false*/)
//				{
//					// --- If any other unit of the group has the same goal, change the goal tile ---
//					SetUnitGoalTile((*unit));
//				}
//				else
//				{
//					// --- Clear previous path request occupied goal tiles ---
//					playerGroup.occupied_list.clear();
//					(*unit)->GoalTile = Map_mouseposition;
//					playerGroup.occupied_list.push_back(&(*unit)->GoalTile);
//				}
//
//				if (myApp->pathfinding->CreatePath(Map_Entityposition, (*unit)->GoalTile) != -1)
//				{
//					(*unit)->Current_path = *myApp->pathfinding->GetLastPath();
//					(*unit)->Current_path.erase((*unit)->Current_path.begin());
//					(*unit)->Current_path.erase((*unit)->Current_path.begin());
//
//					(*unit)->UnitMovementState = movement_State::MovementState_NextStep;
//				}
//				else
//					 stop_iteration = true;
//			}
//
//			break;
//
//		case MovementState::MovementState_Wait:
//
//			// --- Whenever the unit faces an obstacle of any type during a scheduled path, overcome it ---
//
//			break;
//
//		case MovementState::MovementState_FollowPath:
//
//			// --- If a path is created, the unit will start following it ---
//
//			next_tile_world = App->map->MapToWorld((*unit)->info.next_tile.x, (*unit)->info.next_tile.y);
//
//			distanceToNextTile = { (float)next_tile_world.x - (*unit)->Entityinfo.position.x,(float)next_tile_world.y - (*unit)->Entityinfo.position.y };
//
//			// --- We compute the module of our vector ---
//			DirectDistance = sqrtf(pow(distanceToNextTile.x, 2.0f) + pow(distanceToNextTile.y, 2.0f));
//
//			//LOG("Next tile pos : x = %i y= %i", next_tile_world.x, next_tile_world.y);
//
//			// --- We want a unitary vector to update the unit's direction/position ---
//			if (DirectDistance > 0.0f)
//			{
//				distanceToNextTile.x /= DirectDistance;
//				distanceToNextTile.y /= DirectDistance;
//			}
//
//			// --- Now we Apply the unit's Speed and the dt to the unitary vector  ---
//			distanceToNextTile.x *= (*unit)->Entityinfo.Speed*dt;
//			distanceToNextTile.y *= (*unit)->Entityinfo.Speed*dt;
//
//			// --- We convert an iPoint to fPoint for comparing purposes ---
//			to_fPoint.x = next_tile_world.x;
//			to_fPoint.y = next_tile_world.y;
//
//			if ((*unit)->Entityinfo.position.DistanceTo(to_fPoint) < 3)
//			{
//				(*unit)->Entityinfo.position.x = next_tile_world.x;
//				(*unit)->Entityinfo.position.y = next_tile_world.y;
//
//				(*unit)->info.UnitMovementState = MovementState::MovementState_NextStep;
//			}
//
//			else
//			{
//				(*unit)->Entityinfo.position.x += distanceToNextTile.x;
//				(*unit)->Entityinfo.position.y += distanceToNextTile.y;
//			}
//
//			// --- Blit Unit's goal tile ---
//			goal_world = App->map->MapToWorld((*unit)->info.goal_tile.x, (*unit)->info.goal_tile.y);
//			App->render->Blit(App->scene->debug_tex2, goal_world.x, goal_world.y);
//
//			break;
//
//		case MovementState::MovementState_NextStep:
//
//			// --- If a path is being followed, the unit will get the next tile in the path ---
//
//			if ((*unit)->info.Current_path.size() > 0)
//			{
//				(*unit)->info.next_tile = (*unit)->info.Current_path.front();
//				(*unit)->info.Current_path.erase((*unit)->info.Current_path.begin());
//
//				(*unit)->info.UnitMovementState = MovementState::MovementState_FollowPath;
//			}
//			else
//			{
//				(*unit)->info.UnitMovementState = MovementState::MovementState_DestinationReached;
//			}
//
//			break;
//
//		case MovementState::MovementState_DestinationReached:
//
//			// --- The unit reaches the end of the path, thus stopping and returning to NoState ---
//
//			(*unit)->info.UnitMovementState = MovementState::MovementState_NoState;
//
//			break;
//		}
//
//
//		if (stop_iteration)
//		{
//			stop_iteration = false;
//			break;
//		}
//
//		unit++;
//
//	}
//
//}

//void GroupManager::SetUnitGoalTile(Unit* entity)
//{
//	std::list <Unit*>::iterator it = playerGroup.groupUnits_list.begin();
//
//	bool Goal_found = false;
//
//	while (it != playerGroup.groupUnits_list.end())
//	{
//		if (*it == entity)
//		{
//			it++;
//			continue;
//		}
//
//		Goal_found = FindFreeAdjacents(&(*it)->GoalTile);
//
//		if (Goal_found)
//		{
//			entity->GoalTile = (*it)->LastGoal;
//			break;
//		}
//
//		it++;
//	}
//
//}
//
//bool GroupManager::FindFreeAdjacents(iPoint * base_tile)
//{
//	bool ret = false;
//
//	// --- Check if any adjacent tile to the base_tile is free ---
//
//	iPoint cell;
//
//	// north
//	cell.create(base_tile->x, base_tile->y + 1);
//	if (App->pathfinding->IsWalkable(cell) && IsTileFree(&cell))
//	{
//		ret = true;
//		last_goal = cell;
//		return ret;
//	}
//
//	// north east
//	cell.create(base_tile->x + 1, base_tile->y + 1);
//	if (App->pathfinding->IsWalkable(cell) && IsTileFree(&cell))
//	{
//		ret = true;
//		last_goal = cell;
//		return ret;
//	}
//
//	// north west
//	cell.create(base_tile->x - 1, base_tile->y + 1);
//	if (App->pathfinding->IsWalkable(cell) && IsTileFree(&cell))
//	{
//		ret = true;
//		last_goal = cell;
//		return ret;
//	}
//
//	// south
//	cell.create(base_tile->x, base_tile->y - 1);
//	if (App->pathfinding->IsWalkable(cell) && IsTileFree(&cell))
//	{
//		ret = true;
//		last_goal = cell;
//		return ret;
//	}
//
//	// south east
//	cell.create(base_tile->x + 1, base_tile->y - 1);
//	if (App->pathfinding->IsWalkable(cell) && IsTileFree(&cell))
//	{
//		ret = true;
//		last_goal = cell;
//		return ret;
//	}
//
//	// south west
//	cell.create(base_tile->x - 1, base_tile->y - 1);
//	if (App->pathfinding->IsWalkable(cell) && IsTileFree(&cell))
//	{
//		ret = true;
//		last_goal = cell;
//		return ret;
//	}
//
//	// east
//	cell.create(base_tile->x + 1, base_tile->y);
//	if (App->pathfinding->IsWalkable(cell) && IsTileFree(&cell))
//	{
//		ret = true;
//		last_goal = cell;
//		return ret;
//	}
//
//	// west
//	cell.create(base_tile->x - 1, base_tile->y);
//	if (App->pathfinding->IsWalkable(cell) && IsTileFree(&cell))
//	{
//		ret = true;
//		last_goal = cell;
//		return ret;
//	}
//
//
//	return ret;
//}
//
//
//
