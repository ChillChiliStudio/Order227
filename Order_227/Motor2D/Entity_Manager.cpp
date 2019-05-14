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


static_assert((int)infantry_type::INFANTRY_MAX == TROOP_TYPES, "The total number of Uniots are different from the TROOP_TYPES define, have to be equal");

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
	
	_Unit->currTarget = nullptr;
	_Unit->aggroTriggered = false;

	_Unit->unitPath.clear();
	_Unit->currNode = _Unit->unitPath.end();

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

				(*item).faction = entity_faction::COMMUNIST;
				mainBase = &(*item);
				(*item).spriteRect = { 605, 1882, 212, 148 }; //TODO: Desharcodear
				(*item).entityRect.w = (*item).spriteRect.w;
				(*item).entityRect.h = (*item).spriteRect.h;

				(*item).centerPos.x = (*item).position.x + (*item).spriteRect.w / 2;	//TODO: Desharcodear
				(*item).centerPos.y = (*item).position.y + (*item).spriteRect.h / 2;
				(*item).groundPos.x = (*item).position.x + (*item).spriteRect.w / 2;
				(*item).groundPos.y = (*item).position.y + (*item).spriteRect.h;
			}
			else if ((*item).buildingType == building_type::EPC) {


				(*item).faction = entity_faction::COMMUNIST;
				(*item).spriteRect = { 0, 804, 155, 134 }; //TODO: Desharcodear
				(*item).entityRect.w = (*item).spriteRect.w;
				(*item).entityRect.h = (*item).spriteRect.h;

				(*item).centerPos.x = (*item).position.x + (*item).spriteRect.w / 2;	//TODO: Desharcodear
				(*item).centerPos.y = (*item).position.y + (*item).spriteRect.h / 2;
				(*item).groundPos.x = (*item).position.x + (*item).spriteRect.w / 2;
				(*item).groundPos.y = (*item).position.y + (*item).spriteRect.h;
			}
			else
				(*item).faction == entity_faction::NEUTRAL;	//TODO: if not Main_Base then enemy building???

			(*item).active = true;
			(*item).selected = false;
			(*item).texture = buildingsTextures[int((*item).buildingType)];

			for (int i = 0; i < entitiesVector.size(); i++) {

				if (entitiesVector[i] == nullptr) {
					entitiesVector[i] = (Entity*)(&(*item));
					break;
				}
			}

			(*item).Start();
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


	pugi::xml_parse_result result = unitsDocument.load_file("textures/troops/unitsDoc.xml");


	for (pugi::xml_node Data = unitsDocument.child("Entity_Document").child("Troops").child("Soviet").child("Unit"); Data != NULL; Data = Data.next_sibling("Unit")) {

		switch (Data.attribute("id").as_int())
		{
		case(int(infantry_type::CONSCRIPT)):
			infantryTextures[int(infantry_type::CONSCRIPT)] = myApp->tex->Load(Data.attribute("TextPath").as_string());

			break;

		case(int(infantry_type::BAZOOKA)):

			infantryTextures[int(infantry_type::BAZOOKA)] = myApp->tex->Load(Data.attribute("TextPath").as_string());

			break;

		case(int(infantry_type::DESOLATOR)):

			infantryTextures[int(infantry_type::DESOLATOR)] = myApp->tex->Load(Data.attribute("TextPath").as_string());
			break;

		case(int(infantry_type::MACHINE_GUN)):

			infantryTextures[int(infantry_type::MACHINE_GUN)] = myApp->tex->Load(Data.attribute("TextPath").as_string());


			break;

		case(int(infantry_type::SNIPER)):

			infantryTextures[int(infantry_type::SNIPER)] = myApp->tex->Load(Data.attribute("TextPath").as_string());
			break;

		}

	}
	for (pugi::xml_node Data = unitsDocument.child("Entity_Document").child("Troops").child("Capitalist").child("Unit"); Data; Data = Data.next_sibling("Unit")) {
		
		switch (Data.attribute("id").as_int())
		{
			case int(int(infantry_type::BASIC)) :

				infantryTextures[int(infantry_type::BASIC)] = myApp->tex->Load(Data.attribute("TextPath").as_string());

				break;

				case int(int(infantry_type::DOG)) :

					infantryTextures[int(infantry_type::DOG)] = myApp->tex->Load(Data.attribute("TextPath").as_string());

					break;

		}
	}

	buildingsTextures[int(building_type::MAIN_BASE)] = myApp->tex->Load("textures/buildings/mainbase.png");
	buildingsTextures[int(building_type::EPC)] = myApp->tex->Load("textures/buildings/Facilities/Enhance_facility/Barracks_anim.png");
	objectTextures[int(object_type::TREE)] = myApp->tex->Load("maps/Tree_Tileset.png");

	return true;

}

bool Entity_Manager::LoadEntityData() {

	bool ret;
	pugi::xml_parse_result result = unitsDocument.load_file("textures/troops/unitsDoc.xml");


	if (result != NULL)
	{
		AssignAnimData("Soviet");
		AssignAnimData("Capitalist");


	}

	return true;
}

bool Entity_Manager::AssignAnimData(std::string faction) {

	bool ret = true;
	SDL_Rect temp;
	int posArr;

	for (pugi::xml_node DataXML = unitsDocument.child("Entity_Document").child("Troops").child(faction.c_str()).child("Unit"); DataXML; DataXML = DataXML.next_sibling("Unit")) {


		pugi::xml_parse_result TiledFile = TiledDocument.load_file(DataXML.attribute("TiledFile").as_string());
		
		if (faction == "Soviet")
			posArr = 0;
		else
			posArr = 1;


		if (TiledFile != NULL) {

			for (pugi::xml_node DataTMX = TiledDocument.child("map").child("objectgroup").child("object"); DataTMX && ret; DataTMX = DataTMX.next_sibling("object")) {

				temp.x = DataTMX.attribute("x").as_int();
				temp.y = DataTMX.attribute("y").as_int();
				temp.w = DataTMX.attribute("width").as_int();
				temp.h = DataTMX.attribute("height").as_int();


				std::string tempString = DataTMX.attribute("name").as_string();
				int degreesToArray = DataTMX.attribute("type").as_int() / 45;    //DEGREES    HAVE IN ACCOUNT THAT THE TILES ARE DEFINED CONTERCLOCKWISE

				int id = DataXML.attribute("id").as_int();

				switch (id)
				{

					case int(infantry_type::BASIC) :

					   
						break;


					case (int(infantry_type::CONSCRIPT)):

						temp.x += DataXML.child("RectOffset").attribute("x").as_int();
						temp.y += DataXML.child("RectOffset").attribute("y").as_int();
						temp.w = DataXML.child("RectOffset").attribute("w").as_int();
						temp.h = DataXML.child("RectOffset").attribute("h").as_int();

						break;

					case (int(infantry_type::BAZOOKA)):

						//Introduce offset
						temp.x += DataXML.child("RectOffset").attribute("x").as_int();
						temp.y += DataXML.child("RectOffset").attribute("y").as_int();
						temp.w = DataXML.child("RectOffset").attribute("w").as_int();
						temp.h = DataXML.child("RectOffset").attribute("h").as_int();

						break;

					case (int(infantry_type::MACHINE_GUN)):

						//Introduce offset
						temp.x += DataXML.child("RectOffset").attribute("x").as_int();
						temp.y += DataXML.child("RectOffset").attribute("y").as_int();
						temp.w = DataXML.child("RectOffset").attribute("w").as_int();
						temp.h = DataXML.child("RectOffset").attribute("h").as_int();
						break;

					case (int(infantry_type::SNIPER)):

						//Introduce offset
						temp.x += DataXML.child("RectOffset").attribute("x").as_int();
						temp.y += DataXML.child("RectOffset").attribute("y").as_int();
						temp.w = DataXML.child("RectOffset").attribute("w").as_int();
						temp.h = DataXML.child("RectOffset").attribute("h").as_int();
						break;


					case(int(infantry_type::DESOLATOR)):

						//Introduce offset
						temp.x += DataXML.child("RectOffset").attribute("x").as_int();
						temp.y += DataXML.child("RectOffset").attribute("y").as_int();
						temp.w = DataXML.child("RectOffset").attribute("w").as_int();
						temp.h = DataXML.child("RectOffset").attribute("h").as_int();
						break;

					case(int(infantry_type::DOG)):

						//Introduce offset
						temp.x += DataXML.child("RectOffset").attribute("x").as_int();
						temp.y += DataXML.child("RectOffset").attribute("y").as_int();
						temp.w = DataXML.child("RectOffset").attribute("w").as_int();
						temp.h = DataXML.child("RectOffset").attribute("h").as_int();
						break;

				}

				if (tempString == "Pointing") {

					animationArray[id][int(unit_state::IDLE)][degreesToArray][posArr].PushBack(temp);
					animationArray[id][int(unit_state::IDLE)][degreesToArray][posArr].loop = true;
					animationArray[id][int(unit_state::IDLE)][degreesToArray][posArr].speed = 10.0f;

				}
				else if (tempString == "Walking") {

					animationArray[id][int(unit_state::MOVING)][degreesToArray][posArr].PushBack(temp);
					animationArray[id][int(unit_state::MOVING)][degreesToArray][posArr].loop = true;
					animationArray[id][int(unit_state::MOVING)][degreesToArray][posArr].speed = 5.0f;

				}
				else if (tempString == "Shot") {

					animationArray[id][int(unit_state::ATTACKING)][degreesToArray][posArr].PushBack(temp);
					animationArray[id][int(unit_state::ATTACKING)][degreesToArray][posArr].loop = true;
					animationArray[id][int(unit_state::ATTACKING)][degreesToArray][posArr].speed = 10.0f;


				}
				else if (tempString == "DeathOne") {

					animationArray[id][int(unit_state::DEAD)][0][posArr].PushBack(temp);
					animationArray[id][int(unit_state::DEAD)][0][posArr].loop = false;
					animationArray[id][int(unit_state::DEAD)][0][posArr].speed = DataXML.child("AnimDet").attribute("DeathOneSpeed").as_float();


				}

			}

		}
		else
			ret = false;

	}

	return ret;

}

bool Entity_Manager::SetupUnitStats() {

	bool ret = false;

	pugi::xml_parse_result result = unitsDocument.load_file("textures/troops/unitsDoc.xml");

	if (result != NULL)
	{
		AssignStats("Soviet");
		AssignStats("Capitalist");
		
		ret = true;
	}
	return ret;
}

bool Entity_Manager::AssignStats(std::string faction) {

	bool ret = false;
	for (pugi::xml_node Data = unitsDocument.child("Entity_Document").child("Troops").child(faction.c_str()).child("Unit"); Data; Data = Data.next_sibling("Unit"))
	{

		int id = Data.attribute("id").as_int();

		infantryStats[id].linSpeed = Data.child("stats").attribute("speed").as_float();
		infantryStats[id].cadency = Data.child("stats").attribute("cadency").as_float();
		infantryStats[id].damage = (uint)Data.child("stats").attribute("damage").as_int();
		infantryStats[id].health = Data.child("stats").attribute("health").as_float();

		infantryStats[id].visionRadius = (uint)Data.child("stats").attribute("visionRange").as_int();
		infantryStats[id].attackRadius = (uint)Data.child("stats").attribute("attackRange").as_int();

		infantryStats[id].cost = Data.child("stats").attribute("cost").as_int();
		infantryStats[id].productionTime = Data.child("stats").attribute("prodTime").as_int();
		infantryStats[id].unitThreat = Data.child("stats").attribute("threat").as_int();



		ret = true;
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
