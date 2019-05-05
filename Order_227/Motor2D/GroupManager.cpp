#include <cassert>
#include "App.h"
#include "Render.h"
#include "Log.h"
#include "GroupManager.h"
#include "Entity.h"
#include "Scene.h"
#include "Input.h"
#include "Map.h"
#include "Pathfinding.h"

GroupManager::GroupManager()
{}

GroupManager::~GroupManager()
{}

bool GroupManager::Awake()
{
	LOG("AWAKING GROUP MANAGER");

	return true;
}

bool GroupManager::Start()
{
	return true;
}

bool GroupManager::PreUpdate()
{
	return true;
}

bool GroupManager::Update(float dt)
{
	return true;
}

bool GroupManager::CleanUp() {

	LOG("Clean Up Group Manager");

	return true;
}

//--- SELECTION AND GROUPS SYSTEM ---
void GroupManager::SelectUnit(SDL_Rect rect) {

	std::list<Unit*>::iterator item = myApp->entities->ActiveCommunistUnits.begin();
	for (; (*item); item = next(item)) {

		if ((*item)->active == true && (*item)->IsDead() == false) {

			SDL_Rect entityRect = (*item)->UnitRect;

			if (SDL_HasIntersection(&entityRect, &rect)) {
				(*item)->selected = true;
			}
			else if (!SDL_HasIntersection(&entityRect, &rect) && myApp->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_IDLE) {
				(*item)->selected = false;
			}
		}
	}
	//NO BORRAR
	//for (std::list<Unit*>::iterator it = myApp->entities->comunist_list.begin(); it != myApp->entities->comunist_list.end(); it++) {
	//	
	//	SDL_Rect entityRect = (*it)->unitRect;
	//	
	//	//select the rectangle units
	//	if (SDL_HasIntersection(&entityRect, &rect)) {

	//		(*it)->selected = true;
	//		/*playerGroup.groupUnits.push_back((*it));*/
	//	}
	//	//deselect the outside rectangle units if shift button not pressed
	//	else if (!(SDL_HasIntersection(&entityRect, &rect)) && myApp->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_UP) {
	//		
	//		(*it)->selected = false;
	//	}
	//}
}

void GroupManager::SelectUnit(iPoint pos) {

	int counter = 0;

	std::list<Unit*>::iterator item = myApp->entities->ActiveCommunistUnits.begin();
	for (; (*item); item = next(item)) {

		if ((*item)->active == true)
			SDL_Rect entityRect = (*item)->UnitRect;

		if ((counter < 1) && pos.x > (*item)->UnitRect.x
			&& pos.x < (*item)->UnitRect.x + (*item)->UnitRect.w
			&& pos.y >(*item)->UnitRect.y
			&& pos.y < (*item)->UnitRect.y + (*item)->UnitRect.h) {
			
			(*item)->selected = true;
			counter++;
		}

		else if (myApp->input->GetKey(SDL_SCANCODE_LSHIFT) == SDL_RELEASED)
			(*item)->selected = false;
		
	}

	//NO BORRAR
	////int counter = 0;
	//for (std::list<Unit*>::iterator it = myApp->entities->comunist_list.begin(); it != myApp->entities->comunist_list.end(); it++) {
	//	
	//	SDL_Rect entityRect = (*it)->unitRect;
	//	
	//	//if we are here is because we are selecting a single unit with a single click.
	//	if ((counter < 1) && pos.x > (*it)->unitRect.x && pos.x < (*it)->unitRect.x + (*it)->unitRect.w && pos.y >(*it)->unitRect.y && pos.y < (*it)->unitRect.y + (*it)->unitRect.h) {
	//		
	//		(*it)->selected = true;
	//		/*playerGroup.groupUnits.push_back((*it));*/
	//		counter++;
	//	}
	//	//if we are not clicking it and the shift is not pressed
	//	else if (myApp->input->GetKey(SDL_SCANCODE_LSHIFT) == SDL_RELEASED)
	//		(*it)->selected = false;

	//}

}

void GroupManager::CreateGroupForPlayer() {

	if (playerGroup.groupUnits.size() > 0 && myApp->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_IDLE) { // If there was a previous selection and Shift isn't pressed, clear list
		
		LOG("LETS DELETE THE CURRENT GROUP");
		EmptyPlayerGroup();
	}
	
	AddUnitsPlayerGroup();
}

void GroupManager::EmptyPlayerGroup() {

	if (playerGroup.groupUnits.size() > 0) {
		for (std::list<Unit*>::iterator it = playerGroup.groupUnits.begin(); it != playerGroup.groupUnits.end(); it = next(it)) {
			(*it)->stored = false;
		}

		playerGroup.groupUnits.clear();
		LOG("DELETE THE GROUP");
	}
}

void GroupManager::AddUnitsPlayerGroup() {

	std::list<Unit*>::iterator item = myApp->entities->ActiveCommunistUnits.begin();
	for (; (*item); item = next(item)) {

		if ((*item)->selected == true && (*item)->stored == false) {

			playerGroup.groupUnits.push_back(*item);
			(*item)->stored = true;
			LOG("ADD UNIT TO THE GROUP");
		}
	}

	/*for (std::list<Unit*>::iterator iterator = myApp->entities->comunist_list.begin(); iterator != myApp->entities->comunist_list.end(); iterator++) {
		
		if ((*iterator)->selected == true) {
		
			playerGroup.groupUnits.push_back((*iterator));
			LOG("ADD UNIT TO THE GROUP");
		}
	}*/
}

// LEGACY CODE, NOW IN GROUP.CPP
//void GroupManager::SpreadDestinations(iPoint origDest, Group group)
//{
//	if (!myApp->pathfinding->IsWalkable(origDest))
//		return;
//
//	std::vector<iPoint*> visited;
//	std::queue<iPoint*> frontier;
//
//	visited.push_back(&origDest);
//	frontier.push(&origDest);
//
//	std::list<Unit*>::iterator currentUnit = group.groupUnits.begin();
//
//	(*currentUnit++)->destination = myApp->map->MapToWorld(origDest);
//	LOG("%d %d", origDest.x, origDest.y );
//
//	while (currentUnit != group.groupUnits.end())
//	{
//		iPoint* tile = frontier.front();
//		frontier.pop();
//
//		iPoint neighbors[4];
//		neighbors[0].create(tile->x + 1, tile->y + 0);
//		neighbors[1].create(tile->x + 0, tile->y + 1);
//		neighbors[2].create(tile->x - 1, tile->y + 0);
//		neighbors[3].create(tile->x + 0, tile->y - 1);
//
//		for (int i = 0; i < 4; ++i) 
//		{
//			if (myApp->pathfinding->IsWalkable(neighbors[i]))
//			{
//				for (std::vector<iPoint*>::iterator ii = visited.begin(); ii != visited.end(); ++ii)
//				{
//					if ((*ii)->x == neighbors[i].x  && (*ii)->y == neighbors[i].y)
//						break;
//				}
//				frontier.push(&neighbors[i]);
//				visited.push_back(&neighbors[i]);
//
//				(*currentUnit)->destination = myApp->map->MapToWorld(neighbors[i]);
//				currentUnit++;
//				LOG("%d %d", neighbors[i].x, neighbors[i].y);
//				if (currentUnit == group.groupUnits.end())
//					break;
//			}
//		}
//	}
//
//}
//
//void GroupManager::CreateGroupPaths(std::list<Unit*> list, iPoint destination)
//{
//	for (std::list<Unit*>::iterator iterator = list.begin(); iterator != list.end(); iterator++) {
//		(*iterator)->origin.x = (*iterator)->UnitRect.x;
//		(*iterator)->origin.y = (*iterator)->UnitRect.y;
//		(*iterator)->destination = destination;
//		{
//			myApp->pathfinding->CreatePath((*iterator)->origin, (*iterator)->destination);
//			(*iterator)->unitPath.clear();
//			(*iterator)->unitPath = *myApp->pathfinding->GetLastPath();
//		}		
//	}	
//}