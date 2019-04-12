
#include "Entity_Manager.h"
#include "Log.h"
#include "Entity.h"
#include "Scene.h"
#include "Render.h"

#include <cassert>

Entity_Manager::Entity_Manager()
{
	name.assign("entities");

	for (int i = 0; i < ENTITY_POOL_SIZE; ++i)
	{
		playerSoldiersList[i] = new Soldier({0,0}, soldier_type::SOLDIER_NONE, entity_faction::URSS);
		enemySoldiersList[i] = new Soldier({ 0,0 }, soldier_type::SOLDIER_NONE, entity_faction::URSS);
	}

	for (int i = 0; i < STATIC_OBJECTS; ++i)
	{
		staticObjectsList[i] = new Static_Object({ 0,0 }, object_type::OBJECT_NONE, entity_faction::NEUTRAL);
	}

	for (int i = 0; i < BASES; ++i)
	{
		basesList[i] = new Base({0,0}, base_type::BASE_NONE, entity_faction::URSS);
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

	//Clean soldiers
	for (int i = 0; i < ENTITY_POOL_SIZE; ++i)
	{
		playerSoldiersList[i]->CleanUp();
		playerSoldiersList[i] = nullptr;

		enemySoldiersList[i]->CleanUp();
		enemySoldiersList[i] = nullptr;

	}
	
	//Clean objects
	for (int i = 0; i < STATIC_OBJECTS; ++i)
	{
		staticObjectsList[i]->CleanUp();
		staticObjectsList[i] = nullptr;
	}

	//Clean bases
	for (int i = 0; i < BASES; ++i)
	{
		basesList[i]->CleanUp();
		basesList[i] = nullptr;
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

	for (int i = 0; i < ENTITY_POOL_SIZE; ++i)
	{
		playerSoldiersList[i]->Update(dt);
		enemySoldiersList[i]->Update(dt);
		if (do_logic)
		{
			playerSoldiersList[i]->FixUpdate(dt);
			enemySoldiersList[i]->FixUpdate(dt);
		}
	}

	for (int i = 0; i < BASES; ++i)
	{
		basesList[i]->Update();
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
		for (int i = 0; i < ENTITY_POOL_SIZE; ++i)
		{
			if (playerSoldiersList[i]->active == false)
			{
				playerSoldiersList[i]->position = position;
				playerSoldiersList[i]->texture = soldierTextures[int(soldierType)];
				playerSoldiersList[i]->stats = soldierStats[int(soldierType)];
				playerSoldiersList[i]->active = true;
				playerSoldiersList[i]->soldierType = soldierType;
				//To implement:: Update animations
				

				return true;
			}
		}
	}

	//Enemy troops
	else if (entityFaction == entity_faction::US)
	{
		for (int i = 0; i < ENTITY_POOL_SIZE; ++i)
		{
			if (enemySoldiersList[i]->active == false)
			{
				enemySoldiersList[i]->position = position;
				enemySoldiersList[i]->texture = soldierTextures[int(soldierType)];
				enemySoldiersList[i]->stats = soldierStats[int(soldierType)];
				enemySoldiersList[i]->active = true;
				enemySoldiersList[i]->soldierType = soldierType;
				//To implement:: Update animations

				return true;
			}
		}
	}

	return false;
}

bool Entity_Manager::CreateBase(fPoint position, base_type baseType)
{
	for (int i = 0; i < BASES; ++i)
	{
		if (basesList[i]->active == false)
		{
			basesList[i]->position = position;
			basesList[i]->baseType = baseType;
			basesList[i]->active = true;
			return true;
		}
	}
	return false;
}

bool Entity_Manager::CreateObject(fPoint position, object_type objectType)
{
	for (int i = 0; i < ENTITY_POOL_SIZE; ++i)
	{
		if (staticObjectsList[i]->active == false)
		{
			staticObjectsList[i]->position = position;
			staticObjectsList[i]->objectType = objectType;
			staticObjectsList[i]->active = true;
			return true;
		}
	}
	return false;
}

