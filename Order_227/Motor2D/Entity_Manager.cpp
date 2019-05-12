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
#include "Brofiler/Brofiler.h"

#include <algorithm>

Entity_Manager::Entity_Manager()
{
	name.assign("entities");
}


Entity_Manager::~Entity_Manager()
{}

bool Entity_Manager::Awake(pugi::xml_node& config)
{

	LOG("AWAKING ENTITY MANAGER");
	unitsPoolSize = config.child("units_initial_size").attribute("value").as_int(0);

	times_per_sec = TIMES_PER_SEC;
	update_ms_cycle = 1.0f / (float)times_per_sec;

	//Pool Allocation
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

	AllocateEntityPool();

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
	BROFILER_CATEGORY("Entity_Manager Update()-Brown", Profiler::Color::Brown);
	accumulated_time += dt;

	if (myApp->gui->MainMenuTemp_Image->active != true) {	//TODO: This is very hardcoded, we should have a scene workflow

		if (accumulated_time >= update_ms_cycle)
			do_logic = true;

		for (int i = 0; i < unitsPoolSize; ++i) {

			if (unitPool[i].active) {

				unitPool[i].Update(dt);

				if (do_logic)
					unitPool[i].FixUpdate(dt);
			}
		}

		for(int i = 0; i < buildingsArray.size(); i++)
			buildingsArray[i].Update(dt);

		for (int i = 0; i < objectsArray.size(); i++)
			objectsArray[i].Update(dt);

		/*for (std::vector<Unit>::iterator item = unitPool.begin(); item != unitPool.end(); item = next(item)) {

			if (item->active == true)
				item->Update(dt);

			if (do_logic)
				item->FixUpdate(dt);
		}

		for (std::vector<Building>::iterator item = buildingsArray.begin(); item != buildingsArray.end(); item = next(item)) {

			if (item->active == true)
				item->Update(dt);
		}

		for (std::vector<Static_Object>::iterator item = objectsArray.begin(); item != objectsArray.end(); item = next(item)) {

			if (item->active == true)
				item->Update(dt);
		}*/

		//OLD:
		//myApp->render->OrderBlit(myApp->render->OrderToRender);

		//Blit Ordering that actually works
		UpdateBlitOrdering();
		BlitEntities();

		accumulated_time -= update_ms_cycle;
	}

	return true;
}

bool BlitSort(Entity* i, Entity* j)
{
	bool ret = false;

	if (i != nullptr && j != nullptr) {
		ret = i->groundPos.y > j->groundPos.y;
	}

	return ret;
}

void Entity_Manager::UpdateBlitOrdering()
{
	std::sort(entitiesVector.begin(), entitiesVector.end(), BlitSort);
}

void Entity_Manager::BlitEntities()
{
	for (int i = entitiesVector.size() - 1; i >= 0; i--) {
		if (entitiesVector[i] != nullptr && entitiesVector[i]->texture != nullptr) {
			myApp->render->Blit(entitiesVector[i]->texture, entitiesVector[i]->position.x, entitiesVector[i]->position.y, &entitiesVector[i]->spriteRect);
		}
	}
}

bool Entity_Manager::CleanUp()
{
	LOG("Clean Up Entity_Manager");

	for (int i = 0; i < entitiesVector.size(); i++) {

		if (entitiesVector[i] != nullptr)
			entitiesVector[i] = nullptr;
	}

	entitiesVector.clear();

	ReleasePools();
	objectsArray.clear();
	buildingsArray.clear();

	return true;
}

void Entity_Manager::AllocateEntityPool()
{
	entitiesVector.resize(unitsPoolSize + buildingsArray.size() + objectsArray.size());

	for (int i = 0; i < entitiesVector.size(); i++)
		entitiesVector[i] = nullptr;
}

void Entity_Manager::AllocateUnitPool()
{
	unitPool.resize(/*unitPoolSize*/unitsPoolSize);
}

void Entity_Manager::AllocateHitscanPool()
{
	//hitscanPool.resize(/*hitscanPoolSize*/unitsPoolSize);

}

void Entity_Manager::AllocateRangedPool()
{
	//rangedPool.resize(/*rangedPoolSize*/unitsPoolSize);
}

void Entity_Manager::AllocateTankPool()
{
	//tankPool.resize(/*tankPoolSize*/unitsPoolSize);
}


Unit* Entity_Manager::ActivateUnit(fPoint position, infantry_type infantryType, entity_faction entityFaction)
{
	Unit* ret = nullptr;

	for (std::vector<Unit>::iterator item = unitPool.begin(); item != unitPool.end(); item = next(item)) {

		if ((*item).active == false) {

			(*item).faction = entityFaction;
			(*item).position = position;
			(*item).infantryType = infantryType;
			(*item).texture = infantryTextures[int(infantryType)];
			(*item).stats = infantryStats[int(infantryType)];
			(*item).Start();

			for (int i = 0; i < entitiesVector.size(); i++) {

				if (entitiesVector[i] == nullptr) {
					entitiesVector[i] = (Entity*)(&(*item));
					break;
				}
			}

			/*if (entityFaction == entity_faction::CAPITALIST) {	//TODO-Carles: I'll handle this later

				ActiveCapitalistUnits.push_back(UnitsPool[i]);
				(*item).hostileUnits = ActiveCommunistUnits;
				(*item).hostileBuildings = BuildingsList;

			}
			else if (entityFaction == entity_faction::COMMUNIST) {

				ActiveCommunistUnits.push_back(UnitsPool[i]);
				(*item).hostileUnits = ActiveCapitalistUnits;
			}*/

			ret = &(*item);
			break;
		}
	}

	if (ret == nullptr) {

		unitsPoolSize += RESIZE_VALUE;
		AllocateUnitPool();
		ActivateUnit(position, infantryType, entityFaction);
	}

	return ret;
}

bool Entity_Manager::DeActivateUnit(Unit* _Unit) {	//TODO: Reseting values shouldn't be necessary as non-active elements are not iterated at any point, and if they become active again these values are or should be overwritten

	_Unit->stats = infantryStats[int(infantry_type::INFANTRY_NONE)];
	_Unit->infantryType = infantry_type::INFANTRY_NONE;
	_Unit->position = fPoint(0.0f, 0.0f);
	_Unit->texture = nullptr;

	_Unit->faction = entity_faction::NEUTRAL;

	_Unit->active = false;
	_Unit->selected = false;
	//_Unit->currentAnimation = &myApp->entities->animationArray[int(infantry_type::INFANTRY_NONE)][int(unit_state::NONE)][int(unit_directions::NONE)];	//TODO: This caused bugs (Carles: Not sure)

	std::vector<Entity*>::iterator it = entitiesVector.begin();
	for (int i = 0; i < entitiesVector.size(); i++) {

		if (entitiesVector[i] == (Entity*)_Unit) {
			entitiesVector[i] = nullptr;
			break;
		}
	}

	/*if (Unit->faction == entity_faction::CAPITALIST) {	//TODO-Carles: I'll handle this later

		Unit->hostileUnits.clear();
		Unit->hostileBuildings.clear();
		ActiveCapitalistUnits.remove(Unit);

	}
	else if (Unit->faction == entity_faction::COMMUNIST) {

		Unit->hostileUnits.clear();
		ActiveCommunistUnits.remove(Unit);
	}*/

	return true;
}


void Entity_Manager::ActivateBuildings()
{
	for (std::vector<Building>::iterator item = buildingsArray.begin(); item != buildingsArray.end(); item = next(item)) {	//TODO: This entire thing is a workaround, needs to be ready to work with different building types

		if ((*item).buildingType != building_type::BUILDING_MAX && (*item).buildingType != building_type::BUILDING_NONE) {

			if ((*item).buildingType == building_type::MAIN_BASE) {

				(*item).faction == entity_faction::COMMUNIST;
				mainBase = &(*item);
				(*item).spriteRect = { 605, 1882, 212, 148 }; //TODO: Desharcodear
				(*item).entityRect.w = (*item).spriteRect.w;
				(*item).entityRect.h = (*item).spriteRect.h;

				(*item).centerPos.x = (*item).position.x + (*item).spriteRect.w / 2;	//TODO: Desharcodear
				(*item).centerPos.y = (*item).position.y + (*item).spriteRect.h / 2;
				(*item).groundPos.x = (*item).position.x + (*item).spriteRect.w / 2;
				(*item).groundPos.y = (*item).position.y + (*item).spriteRect.h;
			}
			else
				(*item).faction == entity_faction::CAPITALIST;	//TODO: if not Main_Base then enemy building???

			(*item).active = true;
			(*item).selected = false;
			(*item).texture = buildingsTextures[int((*item).buildingType)];

			for (int i = 0; i < entitiesVector.size(); i++) {

				if (entitiesVector[i] == nullptr) {
					entitiesVector[i] = (Entity*)(&(*item));
					break;
				}
			}
		}
	}
}

void Entity_Manager::ActivateObjects()
{
	for (std::vector<Static_Object>::iterator item = objectsArray.begin(); item != objectsArray.end(); item = next(item)) {

		if ((*item).objectType != object_type::OBJECT_NONE && (*item).objectType != object_type::OBJECT_MAX) {

			(*item).active = true;
			(*item).selected = false;
			(*item).texture = objectTextures[int((*item).objectType)];

			if ((*item).objectType == object_type::TREE) {
				(*item).spriteRect = SetupTreeType();	//TODO: CARLESTODO
				(*item).entityRect.w = (*item).spriteRect.w;
				(*item).entityRect.h = (*item).spriteRect.h;

				(*item).centerPos.x = (*item).position.x + (*item).spriteRect.w / 2;	//TODO: Desharcodear
				(*item).centerPos.y = (*item).position.y + (*item).spriteRect.h / 2;
				(*item).groundPos.x = (*item).position.x + (*item).spriteRect.w / 2;
				(*item).groundPos.y = (*item).position.y + (*item).spriteRect.h;
			}

			for (int i = 0; i < entitiesVector.size(); i++) {

				if (entitiesVector[i] == nullptr) {
					entitiesVector[i] = (Entity*)(&(*item));
					break;
				}
			}
		}
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

			infantryStats[i].visionRadius = (uint)Data.attribute("visionRange").as_int();
			infantryStats[i].attackRadius = (uint)Data.attribute("attackRange").as_int();

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

void Entity_Manager::ReleasePools()
{

	unitPool.clear();
	//hitscanPool.clear();
	//rangedPool.clear();
	//tankPool.clear();

}
