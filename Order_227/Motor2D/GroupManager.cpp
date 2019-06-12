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
#include "Entity_Manager.h"

GroupManager::GroupManager()
{
	name.assign("group_manager");
}

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

	for (std::vector<Unit>::iterator item = myApp->entities->unitPool.begin(); item != myApp->entities->unitPool.end(); item = next(item)) {

		if ((*item).active == true && (*item).faction == entity_faction::COMMUNIST && (*item).IsDead() == false) {

			if (SDL_HasIntersection(&(*item).entityRect, &rect)) {
				(*item).selected = true;
			}
			else if (!SDL_HasIntersection(&(*item).entityRect, &rect) && myApp->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_IDLE) {
				(*item).selected = false;
			}
		}
	}

	for (std::vector<Launcher>::iterator item = myApp->entities->launcherPool.begin(); item != myApp->entities->launcherPool.end(); item = next(item)) {

		if ((*item).active == true && (*item).faction == entity_faction::COMMUNIST && (*item).IsDead() == false) {

			if (SDL_HasIntersection(&(*item).entityRect, &rect)) {
				(*item).selected = true;
			}
			else if (!SDL_HasIntersection(&(*item).entityRect, &rect) && myApp->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_IDLE) {
				(*item).selected = false;
			}
		}
	}

}

void GroupManager::SelectUnit(iPoint pos) {

	int counter = 0;

	for (std::vector<Unit>::iterator item = myApp->entities->unitPool.begin(); item != myApp->entities->unitPool.end(); item = next(item)) {	//TODO-Jaume: Check that this works

		if ((*item).active == true && (*item).faction == entity_faction::COMMUNIST) {
			if ((counter < 1) && pos.x > (*item).entityRect.x
				&& pos.x < (*item).entityRect.x + (*item).entityRect.w
				&& pos.y >(*item).entityRect.y
				&& pos.y < (*item).entityRect.y + (*item).entityRect.h) {

				(*item).selected = true;
				counter++;
			}
			else if (myApp->input->GetKey(SDL_SCANCODE_LSHIFT) == SDL_RELEASED) {
				(*item).selected = false;
			}
		}
	}

	for (std::vector<Launcher>::iterator item = myApp->entities->launcherPool.begin(); item != myApp->entities->launcherPool.end(); item = next(item)) {	//TODO-Jaume: Check that this works

		if ((*item).active == true && (*item).faction == entity_faction::COMMUNIST) {
			if ((counter < 1) && pos.x > (*item).entityRect.x
				&& pos.x < (*item).entityRect.x + (*item).entityRect.w
				&& pos.y >(*item).entityRect.y
				&& pos.y < (*item).entityRect.y + (*item).entityRect.h) {

				(*item).selected = true;
				counter++;
			}
			else if (myApp->input->GetKey(SDL_SCANCODE_LSHIFT) == SDL_RELEASED) {
				(*item).selected = false;
			}
		}
	}

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
	int i = playerGroup.groupUnits.size();
}

void GroupManager::AddUnitsPlayerGroup() {

	for (std::vector<Unit>::iterator item = myApp->entities->unitPool.begin(); item != myApp->entities->unitPool.end(); item = next(item)) {

		if ((*item).selected == true && (*item).stored == false) {

			playerGroup.groupUnits.push_back(&(*item));
			(*item).stored = true;
			LOG("ADD UNIT TO THE GROUP");
		}
	}
	for (std::vector<Launcher>::iterator item = myApp->entities->launcherPool.begin(); item != myApp->entities->launcherPool.end(); item = next(item)) {

		if ((*item).selected == true && (*item).stored == false) {

			playerGroup.groupUnits.push_back(&(*item));
			(*item).stored = true;
			LOG("ADD UNIT TO THE GROUP");
		}
	}

}

