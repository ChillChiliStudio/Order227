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
#include "Textures.h"
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
	soldierTextures[int(soldier_type::BASIC)] = myApp->tex->Load("textures/troops/allied/GI.png");

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



