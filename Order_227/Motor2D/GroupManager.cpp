#include "GroupManager.h"
#include "Log.h"
#include "Entity.h"
#include "Scene.h"
#include "Render.h"
#include "App.h"
#include <assert.h>
#include "Input.h"
#include "Pathfinding.h"


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

//--- SELECTION AND GROUPS SYSTEM ---
void GroupManager::SelectUnit(SDL_Rect rect) {

	for (int i = 0; i < INFANTRY_ARRAY_SIZE; i++) {
		if (myApp->entities->CommunistUnitsArray[i]->active == true) {

			SDL_Rect entityRect = myApp->entities->CommunistUnitsArray[i]->UnitRect;

			if (SDL_HasIntersection(&entityRect, &rect)) {
				myApp->entities->CommunistUnitsArray[i]->selected = true;
			}
			else if (!(SDL_HasIntersection(&entityRect, &rect)) && myApp->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_UP) {
				myApp->entities->CommunistUnitsArray[i]->selected = false;
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
	//		/*playerGroup.groupUnits_list.push_back((*it));*/
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

	for (int i = 0; i < INFANTRY_ARRAY_SIZE; i++) {

		if (myApp->entities->CommunistUnitsArray[i]->active == true) {
			SDL_Rect entityRect = myApp->entities->CommunistUnitsArray[i]->UnitRect;
		}

		if ((counter < 1) && pos.x > myApp->entities->CommunistUnitsArray[i]->UnitRect.x &&
			pos.x < myApp->entities->CommunistUnitsArray[i]->UnitRect.x + myApp->entities->CommunistUnitsArray[i]->UnitRect.w &&
			pos.y > myApp->entities->CommunistUnitsArray[i]->UnitRect.y &&
			pos.y < myApp->entities->CommunistUnitsArray[i]->UnitRect.y + myApp->entities->CommunistUnitsArray[i]->UnitRect.h) {
			
			myApp->entities->CommunistUnitsArray[i]->selected = true;
			counter++;
		}

		else if (myApp->input->GetKey(SDL_SCANCODE_LSHIFT) == SDL_RELEASED)
			myApp->entities->CommunistUnitsArray[i]->selected = false;
		
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
	//		/*playerGroup.groupUnits_list.push_back((*it));*/
	//		counter++;
	//	}
	//	//if we are not clicking it and the shift is not pressed
	//	else if (myApp->input->GetKey(SDL_SCANCODE_LSHIFT) == SDL_RELEASED)
	//		(*it)->selected = false;

	//}

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

	for (int i = 0; i < INFANTRY_ARRAY_SIZE; i++) {
		if (myApp->entities->CommunistUnitsArray[i]->selected == true) {
			playerGroup.groupUnits_list.push_back((myApp->entities->CommunistUnitsArray[i]));
			LOG("ADD UNIT TO THE GROUP");
		}
	}

	/*for (std::list<Unit*>::iterator iterator = myApp->entities->comunist_list.begin(); iterator != myApp->entities->comunist_list.end(); iterator++) {
		
		if ((*iterator)->selected == true) {
		
			playerGroup.groupUnits_list.push_back((*iterator));
			LOG("ADD UNIT TO THE GROUP");
		}
	}*/
}


void GroupManager::CreateDestination(iPoint SelectedPos, Group group)
{
	if (!myApp->pathfinding->IsWalkable(SelectedPos))
		return;

	std::vector<iPoint*> visited;
	std::queue<iPoint*> frontier;

	visited.push_back(&SelectedPos);
	frontier.push(&SelectedPos);

	std::list<Unit*>::iterator currentUnit = group.groupUnits_list.begin();

	(*currentUnit++)->destination = SelectedPos;
	LOG("%d %d", SelectedPos.x, SelectedPos.y );

	while (currentUnit != group.groupUnits_list.end())
	{
		iPoint* tile = frontier.front();
		frontier.pop();

		iPoint neighbors[4];
		neighbors[0].create(tile->x + 1, tile->y + 0);
		neighbors[1].create(tile->x + 0, tile->y + 1);
		neighbors[2].create(tile->x - 1, tile->y + 0);
		neighbors[3].create(tile->x + 0, tile->y - 1);

		for (int i = 0; i < 4; ++i) 
		{
			if (myApp->pathfinding->IsWalkable(neighbors[i]))
			{
				for (std::vector<iPoint*>::iterator ii = visited.begin(); ii != visited.end(); ++ii)
				{
					if ((*ii)->x == neighbors[i].x  && (*ii)->y == neighbors[i].y)
						break;
				}
				frontier.push(&neighbors[i]);
				visited.push_back(&neighbors[i]);

				(*currentUnit)->destination = neighbors[i];
				currentUnit++;
				LOG("%d %d", neighbors[i].x, neighbors[i].y);
				if (currentUnit == group.groupUnits_list.end())
					break;
			}
		}
	}

}

void GroupManager::CreateGroupPaths(std::list<Unit*> list, iPoint destination) {
	
	for (std::list<Unit*>::iterator iterator = list.begin(); iterator != list.end(); iterator++) {
		(*iterator)->origin.x = (*iterator)->UnitRect.x;
		(*iterator)->origin.y = (*iterator)->UnitRect.y;
		(*iterator)->destination = destination;
		{
			myApp->pathfinding->CreatePath((*iterator)->origin, (*iterator)->destination);
			(*iterator)->unitPath.clear();
			(*iterator)->unitPath = *myApp->pathfinding->GetLastPath();
		}		
	}	
}


