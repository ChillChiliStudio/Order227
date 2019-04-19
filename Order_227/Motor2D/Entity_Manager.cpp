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

	//Allocate Memory for Units
	int entitiesIterator = 0;

	for (int i = 0; i < INFANTRY_ARRAY_SIZE; ++i)
	{

		Infantry *InfCom = new Infantry({ 0,0 }, infantry_type::INFANTRY_NONE, entity_faction::COMMUNIST);
		CommunistInfantryArray[i] = InfCom;
		CommunistUnitsArray[i] = (Unit*)InfCom;
		entitiesArray[entitiesIterator++] = (Entity*)InfCom;

		Infantry *InfCap = new Infantry({ 0,0 }, infantry_type::INFANTRY_NONE, entity_faction::COMMUNIST);
		CapitalistInfantryArray[i] = InfCap;
		CapitalistUnitsArray[i] = (Unit*)InfCap;
		entitiesArray[entitiesIterator++] = (Entity*)InfCap;
	}

	//Allocate Memory for Objects
	for (int i = 0; i < OBJECTS_ARRAY_SIZE; ++i)
	{
		staticObjectsArray[i] = new Static_Object({ 0,0 }, object_type::OBJECT_NONE, entity_faction::NEUTRAL);
		entitiesArray[entitiesIterator++] = staticObjectsArray[i];
	}

	//Allocate Memory for Buildings
	for (int i = 0; i < BUILDINGS_ARRAY_SIZE; ++i)
	{
		buildingsArray[i] = new Building({ 0,0 }, building_type::BUILDING_NONE, entity_faction::COMMUNIST);
		entitiesArray[entitiesIterator++] = buildingsArray[i];
	}

	return true;
}


bool Entity_Manager::CleanUp() {

	LOG("Clean Up Entity_Manager");

	int entitiesIterator = 0;

	//Clean soldiers
	for (int i = 0; i < INFANTRY_ARRAY_SIZE; ++i)
	{
		CommunistInfantryArray[i]->CleanUp();
		CommunistInfantryArray[i] = nullptr;
		CommunistUnitsArray[i] = nullptr;
		entitiesArray[entitiesIterator++] = nullptr;

		CapitalistInfantryArray[i]->CleanUp();
		CapitalistInfantryArray[i] = nullptr;
		CapitalistUnitsArray[i] = nullptr;
		entitiesArray[entitiesIterator++] = nullptr;
	}

	//Clean objects
	for (int i = 0; i < OBJECTS_ARRAY_SIZE; ++i)
	{
		staticObjectsArray[i]->CleanUp();
		staticObjectsArray[i] = nullptr;
		entitiesArray[entitiesIterator++] = nullptr;
	}

	//Clean bases
	for (int i = 0; i < BUILDINGS_ARRAY_SIZE; ++i)
	{
		buildingsArray[i]->CleanUp();
		buildingsArray[i] = nullptr;
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

	for (int i = 0; i < UNITS_ARRAY_SIZE; ++i) {

		CapitalistUnitsArray[i]->Update(dt);
		CommunistUnitsArray[i]->Update(dt);

		if (do_logic)
		{
			CapitalistUnitsArray[i]->FixUpdate(dt);
			CommunistUnitsArray[i]->FixUpdate(dt);
		}
	}

	for (int i = 0; i < INFANTRY_ARRAY_SIZE; ++i)
	{
		CommunistInfantryArray[i]->Update(dt);
		CapitalistInfantryArray[i]->Update(dt);

		if (do_logic)
		{
			CommunistInfantryArray[i]->FixUpdate(dt);
			CapitalistInfantryArray[i]->FixUpdate(dt);
		}
	}

	for (int i = 0; i < BUILDINGS_ARRAY_SIZE; ++i)
		buildingsArray[i]->Update();
	

	for (int i = 0; i < OBJECTS_ARRAY_SIZE; ++i)
		staticObjectsArray[i]->Update();


	accumulated_time -= update_ms_cycle;
	myApp->render->OrderBlit(myApp->render->OrderToRender);

	return true;
}


bool Entity_Manager::ActivateInfantry(fPoint position, infantry_type infantryType, entity_faction entityFaction)
{
	//Player troops
	if (entityFaction == entity_faction::COMMUNIST)
	{
		for (int i = 0; i < INFANTRY_ARRAY_SIZE; ++i)
		{
			if (CommunistInfantryArray[i]->active == false)
			{

				CommunistInfantryArray[i]->position = position;
				CommunistInfantryArray[i]->texture = infantryTextures[int(infantryType)];
				CommunistInfantryArray[i]->stats = infantryStats[int(infantryType)];
				CommunistInfantryArray[i]->active = true;
				CommunistInfantryArray[i]->infantryType = infantryType;
				//To implement:: Update animations
				
				return true;
			}
		}
	}

	//Enemy troops
	else if (entityFaction == entity_faction::CAPITALIST)
	{
		for (int i = 0; i < INFANTRY_ARRAY_SIZE; ++i)
		{
			if (CapitalistInfantryArray[i]->active == false)
			{

				CapitalistInfantryArray[i]->position = position;
				CapitalistInfantryArray[i]->texture = infantryTextures[int(infantryType)];
				CapitalistInfantryArray[i]->stats = infantryStats[int(infantryType)];
				CapitalistInfantryArray[i]->active = true;
				CapitalistInfantryArray[i]->infantryType = infantryType;
				//To implement:: Update animations

				return true;
			}
		}
	}

	return false;
}

bool Entity_Manager::ActivateBuilding(fPoint position, building_type buildingType)
{
	for (int i = 0; i < BUILDINGS_ARRAY_SIZE; ++i)
	{
		if (buildingsArray[i]->active == false)
		{
			buildingsArray[i]->position = position;
			buildingsArray[i]->buildingType = buildingType;
			buildingsArray[i]->active = true;

			buildingsArray[i]->faction = entity_faction::NEUTRAL;
			buildingsArray[i]->selected = false;
			buildingsArray[i]->texture = buildingsTextures[int(buildingType)];
			return true;
		}
	}
	return false;
}

bool Entity_Manager::ActivateObject(fPoint position, object_type objectType)
{
	for (int i = 0; i < OBJECTS_ARRAY_SIZE; ++i)
	{
		if (staticObjectsArray[i]->active == false)
		{
			staticObjectsArray[i]->position = position;
			staticObjectsArray[i]->objectType = objectType;
			staticObjectsArray[i]->active = true;
			staticObjectsArray[i]->texture = objectTextures[int(objectType)];
			staticObjectsArray[i]->selected = false;

			return true;

		}
	}
	return false;
}


bool Entity_Manager::DeActivateInfantry(Infantry* infantry)
{

	if (infantry->faction == entity_faction::COMMUNIST) {

		for (int i = 0; i < INFANTRY_ARRAY_SIZE; ++i) {
		
			if (CommunistInfantryArray[i] == infantry) {

				CommunistInfantryArray[i]->position = fPoint(0.0f, 0.0f);
				CommunistInfantryArray[i]->texture = nullptr;
				CommunistInfantryArray[i]->stats = infantryStats[int(infantry_type::INFANTRY_NONE)];
				CommunistInfantryArray[i]->active = false;
				CommunistInfantryArray[i]->infantryType = infantry_type::INFANTRY_NONE;
				//To implement:: Update animations

				return true;
			}
		}
	}

	//Enemy troops
	else if (infantry->faction == entity_faction::CAPITALIST) {

		for (int i = 0; i < INFANTRY_ARRAY_SIZE; ++i) {

			if (CapitalistInfantryArray[i]->active == false) {

				CapitalistInfantryArray[i]->position = fPoint(0.0f, 0.0f);
				CapitalistInfantryArray[i]->texture = nullptr;
				CapitalistInfantryArray[i]->stats = infantryStats[int(infantry_type::INFANTRY_NONE)];
				CapitalistInfantryArray[i]->active = false;
				CapitalistInfantryArray[i]->infantryType = infantry_type::INFANTRY_NONE;
				//To implement:: Update animations

				return true;
			}
		}
	}

	return false;
}

bool Entity_Manager::DeActivateBuilding(Building* building)
{

	for (int i = 0; i < BUILDINGS_ARRAY_SIZE; ++i) {

		if (buildingsArray[i] == building) {

			buildingsArray[i]->position = fPoint(0.0f, 0.0f);
			buildingsArray[i]->buildingType = building_type::BUILDING_NONE;
			buildingsArray[i]->active = false;

			buildingsArray[i]->faction = entity_faction::NEUTRAL;
			buildingsArray[i]->selected = false;
			buildingsArray[i]->texture = nullptr;
		}

	}

	return false;
}

bool Entity_Manager::DeActivateObject(Static_Object* object)
{

	for (int i = 0; i < OBJECTS_ARRAY_SIZE; ++i) {

		if (staticObjectsArray[i] == object) {

			staticObjectsArray[i]->position = fPoint(0.0f, 0.0f);
			staticObjectsArray[i]->faction = entity_faction::NEUTRAL;
			staticObjectsArray[i]->objectType = object_type::OBJECT_NONE;
			staticObjectsArray[i]->texture = nullptr;

			return true;
		}
	}

	return false;
}