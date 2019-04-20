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
#include "Textures.h"


Group::Group(){}


Entity_Manager::Entity_Manager()
{
	name.assign("entities");

	//Allocate Memory for Units
	int entitiesIterator = 0;

	for (int i = 0; i < INFANTRY_ARRAY_SIZE; ++i)
	{

		Infantry *InfCom = new Infantry({ 0,0 }, infantry_type::INFANTRY_NONE, entity_faction::COMMUNIST);
		InfCom->active = false;
		CommunistInfantryArray[i] = InfCom;
		CommunistUnitsArray[i] = (Unit*)InfCom;
		entitiesArray[entitiesIterator++] = (Entity*)InfCom;

		Infantry *InfCap = new Infantry({ 0,0 }, infantry_type::INFANTRY_NONE, entity_faction::COMMUNIST);
		InfCap->active = false;
		CapitalistInfantryArray[i] = InfCap;
		CapitalistUnitsArray[i] = (Unit*)InfCap;
		entitiesArray[entitiesIterator++] = (Entity*)InfCap;
	}

	//Allocate Memory for Objects
	for (int i = 0; i < OBJECTS_ARRAY_SIZE; ++i)
	{
		staticObjectsArray[i] = new Static_Object({ 0,0 }, object_type::OBJECT_NONE, entity_faction::NEUTRAL);
		staticObjectsArray[i]->active = false;
		entitiesArray[entitiesIterator++] = staticObjectsArray[i];
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
	//infantryTextures[int(infantry_type::BASIC)] = myApp->tex->Load("textures/troops/allied/GI.png");
	loadTextures();

	int entityIterator = (2 * INFANTRY_ARRAY_SIZE) + OBJECTS_ARRAY_SIZE;

	//Add Buildings as entities
	for (int i = 0; i < BUILDINGS_ARRAY_SIZE; ++i) {

		if (buildingsArray[i] == nullptr) {

			Building* newBuilding = new Building({ 0, 0 }, building_type::BUILDING_NONE, entity_faction::NEUTRAL);
			newBuilding->active = false;
			buildingsArray[i] = newBuilding;

		}
		else
			myApp->entities->ActivateBuilding(buildingsArray[i]->position, building_type::MAIN_BASE, entity_faction::COMMUNIST);


		entitiesArray[entityIterator++] = (Entity*)buildingsArray[i];

	}

	//Tell Units who their enemies are
	for (int i = 0; i < INFANTRY_ARRAY_SIZE; ++i) {

		CommunistUnitsArray[i]->hostileUnits = CapitalistUnitsArray;
		CapitalistUnitsArray[i]->hostileUnits = CommunistUnitsArray;
	}

	for (int i = 0; i < BUILDINGS_ARRAY_SIZE; ++i)
		buildingsArray[i]->Start();


	LoadEntityData();


	//Set up stats of units
	SetupUnitStats();

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

		if(CapitalistUnitsArray[i]->active==true)
			CapitalistUnitsArray[i]->Update(dt);

		if(CommunistUnitsArray[i]->active==true)
			CommunistUnitsArray[i]->Update(dt);

		if (do_logic)
		{
			CapitalistUnitsArray[i]->FixUpdate(dt);
			CommunistUnitsArray[i]->FixUpdate(dt);
		}
	}

	//for (int i = 0; i < INFANTRY_ARRAY_SIZE; ++i)
	//{
	//	CommunistInfantryArray[i]->Update(dt);
	//	CapitalistInfantryArray[i]->Update(dt);

	//	if (do_logic)
	//	{
	//		CommunistInfantryArray[i]->FixUpdate(dt);
	//		CapitalistInfantryArray[i]->FixUpdate(dt);
	//	}
	//}

	for (int i = 0; i < BUILDINGS_ARRAY_SIZE; ++i)
		if(buildingsArray[i]->active == true)
			buildingsArray[i]->Update(dt);


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
				CommunistUnitsArray[i]->infatryType = infantryType;
				//CommunistInfantryArray[i]->infantryType = infantryType;
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
				CapitalistUnitsArray[i]->infatryType = infantryType;
				//CapitalistInfantryArray[i]->infantryType = infantryType;
				//To implement:: Update animations

				return true;
			}
		}
	}

	return false;
}


bool Entity_Manager::ActivateBuilding(fPoint position, building_type buildingType, entity_faction entityFaction)
{
	for (int i = 0; i < BUILDINGS_ARRAY_SIZE; ++i)
	{
		if (buildingsArray[i]->active == false)
		{
			buildingsArray[i]->position = position;
			buildingsArray[i]->buildingType = buildingType;
			buildingsArray[i]->active = true;

			buildingsArray[i]->faction = entityFaction;
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
				//CommunistInfantryArray[i]->infantryType = infantry_type::INFANTRY_NONE;
				CommunistUnitsArray[i]->infatryType = infantry_type::INFANTRY_NONE;
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
				//CapitalistInfantryArray[i]->infantryType = infantry_type::INFANTRY_NONE;
				CapitalistUnitsArray[i]->infatryType = infantry_type::INFANTRY_NONE;
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

bool Entity_Manager::loadTextures() {

	//TODO This need to be charged by a XML
	infantryTextures[int(infantry_type::BASIC)] = myApp->tex->Load("textures/troops/allied/GI.png");
	infantryTextures[int(infantry_type::BAZOOKA)] = myApp->tex->Load("textures/troops/allied/GI.png");

	buildingsTextures[int(building_type::MAIN_BASE)] = myApp->tex->Load("textures/buildings/mainbase.png");

	return true;
}

bool Entity_Manager::LoadEntityData() {

	bool ret = true;
	pugi::xml_parse_result result = tilsetTexture.load_file("textures/troops/allied/IG.tmx");
	Animation a;
	SDL_Rect temp;

	if (result != NULL)
	{
		//TODO Create a MAX UNITS DEFINITON TO DESHARCODE
		for (int i = 0; i < 1; i++)
		{
			for (pugi::xml_node Data = tilsetTexture.child("map").child("objectgroup").child("object"); Data && ret; Data = Data.next_sibling("object"))
			{
				temp.x = Data.attribute("x").as_int();
				temp.y = Data.attribute("y").as_int();
				temp.w = Data.attribute("width").as_int();
				temp.h = Data.attribute("height").as_int();

				std::string tempString = Data.attribute("name").as_string();
				int degreesToArray = Data.attribute("type").as_int() / 45;//DEGREES    HAVE IN ACCOUNT THAT THE TILES ARE DEFINED CONTERCLOCKWISE

					if (tempString == "Pointing")
						animationArray[i][int(unit_state::IDLE)][degreesToArray].PushBack(temp);

					if (tempString == "Walking")
						animationArray[i][int(unit_state::MOVING)][degreesToArray].PushBack(temp);

					if (tempString == "Shot")
						animationArray[i][int(unit_state::ATTACKING)][degreesToArray].PushBack(temp);

					if( tempString=="DeathOne")
						animationArray[i][int(unit_state::DEAD)][0].PushBack(temp);

			}
		}
	}

	return ret;
}

bool Entity_Manager::SetupUnitStats() {

	bool ret = true;

	pugi::xml_parse_result result = unitsDocument.load_file("textures/troops/unitsDoc.xml");

	if (result != NULL)
	{

		int i = 0;

		for (pugi::xml_node Data = unitsDocument.child("Units_Document").child("Unit"); Data&&ret; Data = Data.next_sibling())
		{

			infantryStats[i].linSpeed = Data.attribute("speed").as_float();
			infantryStats[i].cadency = Data.attribute("cadency").as_float();
			infantryStats[i].damage = (uint)Data.attribute("damage").as_int();
			infantryStats[i].health = Data.attribute("health").as_float();

			infantryStats[i].visionRange = (uint)Data.attribute("visionRange").as_int();
			infantryStats[i].attackRange = (uint)Data.attribute("attackRange").as_int();

			infantryStats[i].cost = Data.attribute("cost").as_int();
			infantryStats[i].productionTime = Data.attribute("prodTime").as_int();
			infantryStats[i].unitThreat = Data.attribute("threat").as_int();

			i++;
		}
	}
	return ret;
}
