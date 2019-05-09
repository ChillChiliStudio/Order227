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
#include "UserInterface.h"
#include "Image.h"

Entity_Manager::Entity_Manager()
{
	name.assign("entities");
	
}


Entity_Manager::~Entity_Manager()
{
}


bool Entity_Manager::Awake(pugi::xml_node& config)
{

	LOG("AWAKING ENTITY MANAGER");
	UnitsInitialSize = config.child("units_initial_size").attribute("value").as_int(0);

	times_per_sec = TIMES_PER_SEC;
	update_ms_cycle = 1.0f / (float)times_per_sec;
	
	//Pool Allocation
	AllocateEntityPool();
	AllocateObjectPool();
	AllocateBuildingPool();
	AllocateUnitPool();
	AllocateHitscanPool();
	AllocateRangedPool();
	AllocateTankPool();

	return true;
}


bool Entity_Manager::Start()
{
	//Load textures
	//infantryTextures[int(infantry_type::BASIC)] = myApp->tex->Load("textures/troops/allied/GI.png");
	loadTextures();
	lifeBar_tex = myApp->tex->Load("ui/Life_Icon.png");

	//Activate Buildings & Objects
	ActivateBuildings();
	ActivateObjects();

	LoadEntityData();

	//Set up stats of units
	SetupUnitStats();

	return true;
}

bool Entity_Manager::PreUpdate() {

	do_logic = false;
	return true;
}


bool Entity_Manager::Update(float dt)
{
	accumulated_time += dt;

	if (myApp->gui->MainMenuTemp_Image->active != true) {	//TODO: This is very hardcoded, we should have a scene workflow

		if (accumulated_time >= update_ms_cycle)
			do_logic = true;

		for (std::vector<Unit>::iterator item = unitPool.begin(); item != unitPool.end(); item = next(item)) {

			if (item->active == true)
				item->Update(dt);

			if (do_logic)
				item->FixUpdate(dt);
		}

		for (std::vector<Building>::iterator item = buildingPool.begin(); item != buildingPool.end(); item = next(item)) {

			if (item->active == true)
				item->Update(dt);
		}

		for (std::vector<Static_Object>::iterator item = objectPool.begin(); item != objectPool.end(); item = next(item)) {

			if (item->active == true)
				item->Update(dt);
		}

		accumulated_time -= update_ms_cycle;
		myApp->render->OrderBlit(myApp->render->OrderToRender);
	}

	return true;
}

bool Entity_Manager::CleanUp() {

	LOG("Clean Up Entity_Manager");

	//Clean Units
	//if (UnitsPool.size() > 0) {

	//	for (int i = 0; i < UnitsPool.size(); i++)
	//		RELEASE(UnitsPool[i]);

	//	ActiveCapitalistUnits.clear();
	//	ActiveCommunistUnits.clear();
	//	UnitsPool.clear();
	//}

	////Clean objects
	//if (ObjectsList.size() > 0) {

	//	std::list<Static_Object*>::iterator item = ObjectsList.begin();
	//	for (; (*item); item = next(item))
	//	{
	//		(*item)->CleanUp();
	//		RELEASE(*item);
	//	}

	//	ObjectsList.clear();
	//}

	////Clean Buildings
	//if (BuildingsList.size() > 0) {

	//	std::list<Building*>::iterator item2 = BuildingsList.begin();
	//	for (; (*item2); item2 = next(item2))
	//	{
	//		(*item2)->CleanUp();
	//		RELEASE(*item2);
	//	}

	//	BuildingsList.clear();
	//}

	////Finally, Clean Entities
	//EntitiesArray.clear();

	return true;
}

void Entity_Manager::AllocateEntityPool()
{
	entityPool.reserve(/*entityPoolSize*/UnitsInitialSize);

	for (std::vector<Entity>::iterator it = entityPool.begin(); it != entityPool.end(); it = next(it)) {
		
		
	}
}

void Entity_Manager::AllocateObjectPool()
{
	objectPool.reserve(/*objectPoolSize*/UnitsInitialSize);	//IMPROVE: This class should exist for all static objects, entity should be a base class with minimal members
	
	for (std::vector<Static_Object>::iterator it = objectPool.begin(); it != objectPool.end(); it = next(it)) {

		if ((*it).selected == true) {
			int a = 0;
		}
	}
}

void Entity_Manager::AllocateBuildingPool()
{
	buildingPool.reserve(/*buildingPoolSize*/UnitsInitialSize);
	
}

void Entity_Manager::AllocateUnitPool()
{
	unitPool.reserve(/*unitPoolSize*/UnitsInitialSize);
	
}

void Entity_Manager::AllocateHitscanPool()
{
	//hitscanPool.reserve(/*hitscanPoolSize*/UnitsInitialSize);
	
}

void Entity_Manager::AllocateRangedPool()
{
	//rangedPool.reserve(/*rangedPoolSize*/UnitsInitialSize);
}

void Entity_Manager::AllocateTankPool()
{
	//tankPool.reserve(/*tankPoolSize*/UnitsInitialSize);
}


Unit* Entity_Manager::ActivateUnit(fPoint position, infantry_type infantryType, entity_faction entityFaction)
{

	bool accomplished = false;

	for (std::vector<Unit>::iterator item = unitPool.begin(); item != unitPool.end(); item = next(item)) {

		if ((*item).active == false) {

			(*item).faction = entityFaction;
			(*item).position = position;
			(*item).infantryType = infantryType;
			(*item).texture = infantryTextures[int(infantryType)];
			(*item).stats = infantryStats[int(infantryType)];

			(*item).UnitSetup();
			(*item).currentAnimation = &myApp->entities->animationArray[int(infantryType)][int((*item).unitState)][int((*item).unitDirection)];

			if (entityFaction == entity_faction::CAPITALIST) {

				ActiveCapitalistUnits.push_back(UnitsPool[i]);
				(*item).hostileUnits = ActiveCommunistUnits;
				(*item).hostileBuildings = BuildingsList;

			}
			else if (entityFaction == entity_faction::COMMUNIST) {

				ActiveCommunistUnits.push_back(UnitsPool[i]);
				(*item).hostileUnits = ActiveCapitalistUnits;
			}

			accomplished = true;
			return UnitsPool[i];
		}
	}

	if (accomplished == false) {

		AllocateUnitsPool(RESIZE_VALUE);
		ActivateUnit(position, infantryType, entityFaction);
	}

	return nullptr;
}


bool Entity_Manager::DeActivateUnit(Unit* Unit) {

	Unit->stats = infantryStats[int(infantry_type::INFANTRY_NONE)];
	Unit->infantryType = infantry_type::INFANTRY_NONE;
	Unit->position = fPoint(0.0f, 0.0f);
	Unit->texture = nullptr;

	Unit->faction = entity_faction::NEUTRAL;

	Unit->active = false;
	Unit->selected = false;
	Unit->currentAnimation = &myApp->entities->animationArray[int(infantry_type::INFANTRY_NONE)][int(unit_state::NONE)][int(unit_directions::NONE)];


	if (Unit->faction == entity_faction::CAPITALIST) {

		Unit->hostileUnits.clear();
		Unit->hostileBuildings.clear();
		ActiveCapitalistUnits.remove(Unit);

	}
	else if (Unit->faction == entity_faction::COMMUNIST) {

		Unit->hostileUnits.clear();
		ActiveCommunistUnits.remove(Unit);
	}
	
	return true;
}


void Entity_Manager::ActivateBuildings()
{
	for (std::list<Building*>::iterator item = BuildingsList.begin(); (*item) && *item != nullptr; item = next(item)) {

		if ((*item)->buildingType != building_type::BUILDING_MAX && (*item)->buildingType != building_type::BUILDING_NONE) {

			if ((*item)->buildingType == building_type::MAIN_BASE) {

				(*item)->faction == entity_faction::COMMUNIST;
				mainBase = (*item);
			}
			else
				(*item)->faction == entity_faction::CAPITALIST;

			(*item)->active = true;
			(*item)->selected = false;
			(*item)->texture = buildingsTextures[int((*item)->buildingType)];

		}

		EntitiesArray.push_back(*item);
	}
}


void Entity_Manager::ActivateObjects()
{
	std::list<Static_Object*>::iterator item = ObjectsList.begin();
	for (; (*item); item = next(item)) {

		if ((*item)->objectType != object_type::OBJECT_NONE && (*item)->objectType != object_type::OBJECT_MAX) {

			(*item)->active = true;
			(*item)->selected = false;
			(*item)->texture = objectTextures[int((*item)->objectType)];

			if ((*item)->objectType == object_type::TREE)
				(*item)->UnitRect = SetupTreeType();

		}

		EntitiesArray.push_back(*item);
	}
}


bool Entity_Manager::loadTextures() {

	//TODO This needs to be charged by a XML
	infantryTextures[int(infantry_type::BASIC)] = myApp->tex->Load("textures/troops/allied/GI.png");
	infantryTextures[int(infantry_type::CONSCRIPT)] = myApp->tex->Load("textures/troops/soviet/InfanteriaSov.png");
	infantryTextures[int(infantry_type::BAZOOKA)] = myApp->tex->Load("textures/troops/allied/GI.png");
	buildingsTextures[int(building_type::MAIN_BASE)] = myApp->tex->Load("textures/buildings/mainbase.png");
	objectTextures[int(object_type::TREE)] = myApp->tex->Load("maps/Tree_Tileset.png");

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
		for (int i = 0; i < 2; i++)
		{
			for (pugi::xml_node Data = tilsetTexture.child("map").child("objectgroup").child("object"); Data && ret; Data = Data.next_sibling("object"))
			{
				temp.x = Data.attribute("x").as_int();
				temp.y = Data.attribute("y").as_int();
				temp.w = Data.attribute("width").as_int();
				temp.h = Data.attribute("height").as_int();
				
				//TODO: find a method to ajust the rects automatically

				std::string tempString = Data.attribute("name").as_string();
				int degreesToArray = Data.attribute("type").as_int() / 45;//DEGREES    HAVE IN ACCOUNT THAT THE TILES ARE DEFINED CONTERCLOCKWISE

				if (i == 0) {
					
					temp.x += 26;
					temp.y += 7;
					if (tempString != "DeathOne") {
						temp.w = 25;
						temp.h = 35;
					}
				}
				else {

					temp.x += 26;
					temp.y += 20;
					temp.w = 25;
					temp.h = 35;

				}
			
				if (tempString == "Pointing") {
					animationArray[i][int(unit_state::IDLE)][degreesToArray].PushBack(temp);
					animationArray[i][int(unit_state::IDLE)][degreesToArray].loop = true;
					animationArray[i][int(unit_state::IDLE)][degreesToArray].speed = 10.0f;
				}
				if (tempString == "Walking") {
					animationArray[i][int(unit_state::MOVING)][degreesToArray].PushBack(temp);
					animationArray[i][int(unit_state::MOVING)][degreesToArray].loop = true;
					animationArray[i][int(unit_state::MOVING)][degreesToArray].speed = 5.0f;
				}
				if (tempString == "Shot") {
					animationArray[i][int(unit_state::ATTACKING)][degreesToArray].PushBack(temp);
					animationArray[i][int(unit_state::ATTACKING)][degreesToArray].loop = true;
					animationArray[i][int(unit_state::ATTACKING)][degreesToArray].speed = 10.0f;
				}
				if (tempString == "DeathOne") {
					animationArray[i][int(unit_state::DEAD)][0].PushBack(temp);
					animationArray[i][int(unit_state::DEAD)][0].loop = false;
					if (i == 0) {
						animationArray[i][int(unit_state::DEAD)][0].speed = 3.0f;
					}else if(i==1)
						animationArray[i][int(unit_state::DEAD)][0].speed = 3.0f;
				}
			}

			result = tilsetTexture.load_file("textures/troops/soviet/InfantSov.tmx");
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

SDL_Rect Entity_Manager::SetupTreeType() {

	SDL_Rect ret;

	int Aux = rand() % 4;

	switch (Aux)
	{
	case ((int)Trees::DEFAULT):
		ret = { 0,0,60,96 };
		break;
	case ((int)Trees::FOREST_ONE):
		ret = { 420, 0, 60, 96 };
		break;
	case ((int)Trees::FOREST_TWO):
		ret = { 60, 96, 60, 96 };
		break;
	case ((int)Trees::PINE):
		ret = { 300, 0, 60, 96 };
		break;
	case ((int)Trees::DESERT):
		ret = { 300, 194, 60, 96 };
		break;
	}


	return ret;
}

void Entity_Manager::ReleasePools() {

	if (ActiveCommunistUnits.size() > 0) {

		std::list<Unit*>::iterator item = ActiveCommunistUnits.begin();

		for (; (*item); item = next(item))
			RELEASE(*item);

		ActiveCommunistUnits.clear();
	}

	if (ActiveCapitalistUnits.size() > 0) {

		std::list<Unit*>::iterator item = ActiveCapitalistUnits.begin();

		for (; (*item); item = next(item))
			RELEASE(*item);

		ActiveCapitalistUnits.clear();
	}

	if (ObjectsList.size() > 0) {

		std::list<Static_Object*>::iterator item = ObjectsList.begin();

		for (; (*item); item = next(item))
			RELEASE(*item);

		ObjectsList.clear();
	}

	if (BuildingsList.size() > 0) {

		std::list<Building*>::iterator item = BuildingsList.begin();

		for (; (*item); item = next(item))
			RELEASE(*item);

		BuildingsList.clear();
	}

	UnitsPool.clear();
	EntitiesArray.clear();
}