


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

void GroupManager::MoveUnits(std::list<Unit*> list, iPoint origin, iPoint destination) {
	myApp->pathfinding->CreatePath(origin, destination);
}

void GroupManager::MoveUnitsGroup() {

}

