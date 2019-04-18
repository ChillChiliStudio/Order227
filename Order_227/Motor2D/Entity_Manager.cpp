//@lucho1: Be extremely careful with shitty STL, to iterate a list,
//if you start it with list.begin() and then check if *item is nullptr,
//it will not "detect" that *item is nullptr and will enter wherever wants to enter
//with an empty list.
//Just check, before creating the iterator, that list.size() > 0

//Also, note that I think there is some memory leak in this module, but
//I couldn't track them, I don't exactly know why the fuck there is a memLeak
//if I load NOTHING 

#include "Entity_Manager.h"
#include "Log.h"
#include "Entity.h"
#include "Scene.h"
#include "Render.h"
#include "App.h"
#include <assert.h>
#include "Input.h"
#include "Pathfinding.h"


Group::Group(){}


Entity_Manager::Entity_Manager()
{
	name.assign("entities");
	int entitiesIterator = 0;

	for (int i = 0; i < SOLDIERS_LIST_SIZE; ++i)
	{
		urssSoldiersArray[i] = new Soldier({ 0,0 }, soldier_type::SOLDIER_NONE, entity_faction::URSS);
		urssUnitsArray[i] = urssSoldiersArray[i];
		entitiesArray[entitiesIterator++] = urssSoldiersArray[i];
		
		eeuuSoldiersArray[i] = new Soldier({ 0,0 }, soldier_type::SOLDIER_NONE, entity_faction::URSS);
		eeuuUnitsArray[i] = eeuuSoldiersArray[i];
		entitiesArray[entitiesIterator++] = eeuuSoldiersArray[i];
	}

	for (int i = 0; i < OBJECTS_LIST_SIZE; ++i)
	{
		staticObjectsArray[i] = new Static_Object({ 0,0 }, object_type::OBJECT_NONE, entity_faction::NEUTRAL);
		entitiesArray[entitiesIterator++] = staticObjectsArray[i];
	}

	for (int i = 0; i < BASES_LIST_SIZE; ++i)
	{
		basesArray[i] = new Base({ 0,0 }, base_type::BASE_NONE, entity_faction::URSS);
		entitiesArray[entitiesIterator++] = basesArray[i];
	}

}

Entity_Manager::~Entity_Manager()
{
}

bool Entity_Manager::Awake()
{
	LOG("AWAKING ENTITY MANAGER");
	times_per_sec = TIMES_PER_SEC;
	update_ms_cycle = 1.0f / (float)times_per_sec;

	return true;
}

bool Entity_Manager::Start()
{
	//Load textures

	return true;
}

bool Entity_Manager::CleanUp() {

	LOG("Clean Up Entity_Manager");

	int entitiesIterator = 0;

	//Clean soldiers
	for (int i = 0; i < SOLDIERS_LIST_SIZE; ++i)
	{
		urssSoldiersArray[i]->CleanUp();
		urssSoldiersArray[i] = nullptr;
		urssUnitsArray[i] = nullptr;
		entitiesArray[entitiesIterator++] = nullptr;

		eeuuSoldiersArray[i]->CleanUp();
		eeuuSoldiersArray[i] = nullptr;
		eeuuUnitsArray[i] = nullptr;
		entitiesArray[entitiesIterator++] = nullptr;
	}

	//Clean objects
	for (int i = 0; i < OBJECTS_LIST_SIZE; ++i)
	{
		staticObjectsArray[i]->CleanUp();
		staticObjectsArray[i] = nullptr;
		entitiesArray[entitiesIterator++] = nullptr;
	}

	//Clean bases
	for (int i = 0; i < BASES_LIST_SIZE; ++i)
	{
		basesArray[i]->CleanUp();
		basesArray[i] = nullptr;
		entitiesArray[entitiesIterator++] = nullptr;
	}

	return true;
}

bool Entity_Manager::PreUpdate() {

	do_logic = false;
	return true;
}

bool Entity_Manager::Update(float dt)
{

	accumulated_time += dt;

	if (accumulated_time >= update_ms_cycle)
		do_logic = true;

	for (int i = 0; i < SOLDIERS_LIST_SIZE; ++i)
	{
		urssSoldiersArray[i]->Update(dt);
		eeuuSoldiersArray[i]->Update(dt);
		if (do_logic)
		{
			urssSoldiersArray[i]->FixUpdate(dt);
			eeuuSoldiersArray[i]->FixUpdate(dt);
		}
	}

	for (int i = 0; i < BASES_LIST_SIZE; ++i)
	{
		basesArray[i]->Update();
	}

	accumulated_time -= update_ms_cycle;

	myApp->render->OrderBlit(myApp->render->OrderToRender);

	return true;
}

bool Entity_Manager::CreateSoldier(fPoint position, soldier_type soldierType, entity_faction entityFaction)
{
	//Player troops
	if (entityFaction == entity_faction::URSS)
	{
		for (int i = 0; i < SOLDIERS_LIST_SIZE; ++i)
		{
			if (urssSoldiersArray[i]->active == false)
			{
				urssSoldiersArray[i]->position = position;
				urssSoldiersArray[i]->texture = soldierTextures[int(soldierType)];
				urssSoldiersArray[i]->stats = soldierStats[int(soldierType)];
				urssSoldiersArray[i]->active = true;
				urssSoldiersArray[i]->soldierType = soldierType;
				//To implement:: Update animations


				return true;
			}
		}
	}

	//Enemy troops
	else if (entityFaction == entity_faction::US)
	{
		for (int i = 0; i < SOLDIERS_LIST_SIZE; ++i)
		{
			if (eeuuSoldiersArray[i]->active == false)
			{
				eeuuSoldiersArray[i]->position = position;
				eeuuSoldiersArray[i]->texture = soldierTextures[int(soldierType)];
				eeuuSoldiersArray[i]->stats = soldierStats[int(soldierType)];
				eeuuSoldiersArray[i]->active = true;
				eeuuSoldiersArray[i]->soldierType = soldierType;
				//To implement:: Update animations

				return true;
			}
		}
	}

	return false;
}

bool Entity_Manager::CreateBase(fPoint position, base_type baseType)
{
	for (int i = 0; i < BASES_LIST_SIZE; ++i)
	{
		if (basesArray[i]->active == false)
		{
			basesArray[i]->position = position;
			basesArray[i]->baseType = baseType;
			basesArray[i]->active = true;
			return true;
		}
	}
	return false;
}

bool Entity_Manager::CreateObject(fPoint position, object_type objectType)
{
	for (int i = 0; i < SOLDIERS_LIST_SIZE; ++i)
	{
		if (staticObjectsArray[i]->active == false)
		{
			staticObjectsArray[i]->position = position;
			staticObjectsArray[i]->objectType = objectType;
			staticObjectsArray[i]->active = true;
			return true;
		}
	}
	return false;
}


// SELECTION AND GROUPS SYSTEM BELLOW__________________________________________________________________________________

void Entity_Manager::SelectUnit(SDL_Rect rect) {
	for (int i = 0; i < SOLDIERS_LIST_SIZE; ++i)
	{
		if (urssSoldiersArray[i]->active)
		{
			if (SDL_HasIntersection(&urssSoldiersArray[i]->selectionRect, &rect))
			{
				urssSoldiersArray[i]->selected = true;
			}
			else if (!SDL_HasIntersection(&urssSoldiersArray[i]->selectionRect, &rect) && myApp->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_UP) {
				urssSoldiersArray[i]->selected = false;
			}

		}
		
	}
	
}

void Entity_Manager::SelectUnit(iPoint pos) {
	//RECODE
	//int counter = 0;
	//for (int i = 0; i < ENTITY_POOL_SIZE; ++i)
	//{
	//	if (playerSoldiersList[i]->active)

	//}

	//for (std::list<Unit*>::iterator it = comunist_list.begin(); it != comunist_list.end(); it++) {
	//	SDL_Rect entityRect = (*it)->unitRect;
	//	//if we are here is because we are selecting a single unit with a single click.
	//	if ((counter < 1) && pos.x > (*it)->unitRect.x && pos.x < (*it)->unitRect.x + (*it)->unitRect.w && pos.y >(*it)->unitRect.y && pos.y < (*it)->unitRect.y + (*it)->unitRect.h) {
	//		(*it)->selected = true;
	//		/*playerGroup.groupUnits_list.push_back((*it));*/ 
	//		counter++;
	//	}//if we are not clicking it and the shift is not pressed
	//	else if(myApp->input->GetKey(SDL_SCANCODE_LSHIFT) == SDL_RELEASED)
	//			(*it)->selected = false;
	//	
	//}
				
}

void Entity_Manager::CreateGroupForPlayer() {

	//RECODE
	//if (playerGroup.groupUnits_list.size() != 0 && myApp->input->GetKey(SDL_SCANCODE_LSHIFT) == SDL_RELEASED) { //no shift pressed and try to select
	//	LOG("LETS DELETE THE CURRENT GROUP");
	//	EmptyPlayerGroup();
	//}
	//
	//if (myApp->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) { //shift pressed and try to select
	//	LOG("LETS ADD UNITS TO THE GROUP");
	//	myApp->entities->AddUnitsPlayerGroup();
	//}

	//else if (myApp->input->GetKey(SDL_SCANCODE_LSHIFT) == SDL_RELEASED && playerGroup.groupUnits_list.size() == 0) { 
	//	LOG("LETS CREATE AND FILL A NEW GROUP IF THERE IS A SELECTION");
	//	AddUnitsPlayerGroup();
	//}
	
}

void Entity_Manager::EmptyPlayerGroup() {
	//RECODE
	/*std::list<Unit*>::iterator item = playerGroup.groupUnits_list.begin();
	if (*item != nullptr) {
		playerGroup.groupUnits_list.clear();
		LOG("DELETE THE GROUP");
	}*/
}

void Entity_Manager::AddUnitsPlayerGroup() {
	//RECODE
	/*for (std::list<Unit*>::iterator iterator = comunist_list.begin(); iterator != comunist_list.end(); iterator++) {
		if ((*iterator)->selected == true) {
			playerGroup.groupUnits_list.push_back((*iterator));
			LOG("ADD UNIT TO THE GROUP");
		}
	}*/
}

void Entity_Manager::MoveUnits(std::list<Unit*> list, iPoint origin,iPoint destination) {
	myApp->pathfinding->CreatePath(origin, destination);
}

