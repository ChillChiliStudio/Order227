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
#include "Launcher.h"
#include "EntityQuadtree.h"
#include "Horde_Manager.h"

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
	launcherPoolSize = 100; //TODO DESJARCODE

	times_per_sec = TIMES_PER_SEC;
	update_ms_cycle = 1.0f / (float)times_per_sec;

	//Pool Allocation
	AllocateUnitPool();
	AllocateLauncherPool();
	AllocateHitscanPool();
	AllocateRangedPool();
	AllocateTankPool();

	return true;
}


bool Entity_Manager::Start()
{


static_assert((int)infantry_type::INFANTRY_MAX == TROOP_TYPES, "The total number of Units are different from the TROOP_TYPES define, have to be equal");

	//Load textures
	//infantryTextures[int(infantry_type::BASIC)] = myApp->tex->Load("textures/troops/allied/GI.png");
	loadTextures();
	lifeBar_tex = myApp->tex->Load("ui/Life_Icon.png");

	AllocateEntityPool();

	//Activate Buildings & Objects

	LoadEntityData();

	//Set up stats of units
	SetupUnitStats();

	//Set up the quadtree
	SDL_Rect quadtreeRect;
	quadtreeRect.x = -(myApp->map->data.width*myApp->map->data.tile_width) / 2;
	quadtreeRect.y = 0;
	quadtreeRect.w = myApp->map->data.width*myApp->map->data.tile_width;
	quadtreeRect.h = myApp->map->data.height*myApp->map->data.tile_height;

	entitiesQuadtree = new EntityQuadtree(QUADTREE_DIVISIONS, quadtreeRect);
	entitiesQuadtree->Split(QUADTREE_DIVISIONS);
	return true;
}

bool Entity_Manager::PreUpdate()
{
	BROFILER_CATEGORY("Entity_Manager Pre-Update", Profiler::Color::LightYellow);

	do_logic = false;
	return true;
}


bool Entity_Manager::Update(float dt)
{
	//Fill the quadtree  with entities
	entitiesQuadtree->FillTree();

	BROFILER_CATEGORY("Entity_Manager Update", Profiler::Color::Yellow);
	accumulated_time += dt;

	if (myApp->gui->MainMenuTemp_Image->active != true) {	//TODO: This is very hardcoded, we should have a scene workflow

		if (accumulated_time >= update_ms_cycle)
			do_logic = true;


		UpdateUnits(dt);
		UpdateBuildings(dt);
		UpdateObjects(dt);

		//Blit Ordering that actually works
		UpdateBlitOrdering();
		BlitEntities();

		accumulated_time -= update_ms_cycle;
	}

	if (entitiesDebugDraw)
		entitiesQuadtree->DrawQuadtree();

	SolveOverlapping();

	//Clear the quadtree of entities
	entitiesQuadtree->ClearTree();

	return true;
}

void Entity_Manager::UpdateUnits(float dt)
{
	BROFILER_CATEGORY("UnitPool Update", Profiler::Color::Magenta);

	int numActives = activeUnits;

	for (int i = 0; numActives > 0; ++i) {

		if (unitPool[i].active == true) {
			numActives--;

			if (i == 33) {
				int patata = 0;
			}

			unitPool[i].Update(dt);

			if (do_logic) {
				unitPool[i].FixUpdate(dt);
			}
		}
	}

	numActives = activeLaunchers;

	//LAUNCHER UNITS
	for (int i = 0; numActives > 0; ++i) {

		if (launcherPool[i].active == true) {
			numActives--;

			launcherPool[i].Update(dt);
			if (do_logic)
				launcherPool[i].FixUpdate(dt);
		}
	}
}

void Entity_Manager::UpdateBuildings(float dt)
{
	BROFILER_CATEGORY("BuildingPool Update", Profiler::Color::Purple);

	for (int i = 0; i < buildingsArray.size(); i++)
		buildingsArray[i].Update(dt);
	
	int a = 0;
}

void Entity_Manager::UpdateObjects(float dt)
{
	BROFILER_CATEGORY("ObjectPool Update", Profiler::Color::MediumPurple);

	for (int i = 0; i < objectsArray.size(); i++) 
		objectsArray[i].Update(dt);

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
	BROFILER_CATEGORY("BlitOrdering", Profiler::Color::Lavender);

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

void Entity_Manager::AllocateLauncherPool()
{
	launcherPool.resize(launcherPoolSize);
}

Unit* Entity_Manager::ActivateUnit(fPoint position, infantry_type infantryType, entity_faction entityFaction)
{
	Unit* ret = nullptr;

	for (std::vector<Unit>::iterator item = unitPool.begin(); item != unitPool.end(); item = next(item)) {

		if ((*item).active == false) {

			(*item).faction = entityFaction;
			(*item).position = position;
			(*item).infantryType = infantryType;
			(*item).texture = infantryTextures[int(infantryType)][(int)entityFaction];
			(*item).stats = infantryStats[int(infantryType)];
			(*item).Start(); //active = true goes here in this start

			for (int i = 0; i < entitiesVector.size(); i++) {

				if (entitiesVector[i] == nullptr) {

					entitiesVector[i] = (Entity*)(&(*item));
					break;
				}
			}


			ret = &(*item);
			(*item).active;
			break;
		}
	}

	if (ret == nullptr) {

		unitsPoolSize += RESIZE_VALUE;
		AllocateUnitPool();
		ActivateUnit(position, infantryType, entityFaction);
	}
	else {
		activeUnits++;
	}

	return ret;
}

Launcher* Entity_Manager::ActivateLauncher(fPoint position, infantry_type infantryType, entity_faction entityFaction)
{
	Launcher* ret = nullptr;

	for (std::vector<Launcher>::iterator item = launcherPool.begin(); item != launcherPool.end(); item = next(item)) {

		if ((*item).active == false) {

			(*item).faction = entityFaction;
			(*item).position = position;
			(*item).infantryType = infantryType;
			(*item).texture = infantryTextures[int(infantryType)][(int)entityFaction];
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

		launcherPoolSize += RESIZE_VALUE;
		AllocateLauncherPool();
		ActivateLauncher(position, infantryType, entityFaction);
	}
	else {
		activeLaunchers++;
	}

	return ret;
}

bool Entity_Manager::DeActivateUnit(Unit* _Unit) {	//TODO: Reseting values shouldn't be necessary as non-active elements are not iterated at any point, and if they become active again these values are or should be overwritten

	switch (_Unit->infantryType) {
	case infantry_type::BAZOOKA:
		activeLaunchers--;
		break;
	default:
		activeUnits--;
		break;
	}

	if (_Unit->faction == entity_faction::CAPITALIST)
		myApp->hordes->remainingEnemies--;
		

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

	return true;
}


void Entity_Manager::ActivateBuildings()
{

	for (std::vector<Building>::iterator item = buildingsArray.begin(); item != buildingsArray.end(); item = next(item)) {	//TODO: This entire thing is a workaround, needs to be ready to work with different building types

		if ((*item).buildingType != building_type::BUILDING_MAX && (*item).buildingType != building_type::BUILDING_NONE) {

			(*item).faction = entity_faction::NEUTRAL;
			(*item).health = (*item).maxHealth;
			(*item).repairable = true;

			if ((*item).buildingType == building_type::COMMAND_CENTER) {

				(*item).faction = entity_faction::COMMUNIST;
				mainBase = &(*item);
			}


			if ((*item).buildingType == building_type::COMMAND_CENTER) {
				(*item).entityRect = { (int)(*item).position.x, (int)(*item).position.y, 130, 210 };	//Todo: Use XML values for rect width, not hardcoded ones (but take these as reference)
				(*item).centerPos = { (float)((*item).position.x + (*item).entityRect.w / 2), (float)((*item).position.y + (*item).entityRect.h / 2) };
				(*item).groundPos = { (float)((*item).position.x + (*item).entityRect.w / 2), (float)((*item).position.y + (*item).entityRect.h) };
			}
			if ((*item).buildingType == building_type::EPC) {
				(*item).entityRect = { (int)(*item).position.x, (int)(*item).position.y, 320, 190 };	//Todo: Use XML values for rect width, not hardcoded ones (but take these as reference)
				(*item).centerPos = { (float)((*item).position.x + (*item).entityRect.w / 2), (float)((*item).position.y + (*item).entityRect.h / 2) };
				(*item).groundPos = { (float)((*item).position.x + (*item).entityRect.w / 2), (float)((*item).position.y + (*item).entityRect.h) };
			}
			if ((*item).buildingType == building_type::GOLDYARD) {
				(*item).entityRect = { (int)(*item).position.x, (int)(*item).position.y, 180, 140 };	//Todo: Use XML values for rect width, not hardcoded ones (but take these as reference)
				(*item).centerPos = { (float)((*item).position.x + (*item).entityRect.w / 2), (float)((*item).position.y + (*item).entityRect.h / 2) };
				(*item).groundPos = { (float)((*item).position.x + (*item).entityRect.w / 2), (float)((*item).position.y + (*item).entityRect.h) };
			}
			if ((*item).buildingType == building_type::HTPC) {
				(*item).entityRect = { (int)(*item).position.x, (int)(*item).position.y, 110, 130 };	//Todo: Use XML values for rect width, not hardcoded ones (but take these as reference)
				(*item).centerPos = { (float)((*item).position.x + (*item).entityRect.w / 2), (float)((*item).position.y + (*item).entityRect.h / 2) };
				(*item).groundPos = { (float)((*item).position.x + (*item).entityRect.w / 2), (float)((*item).position.y + (*item).entityRect.h) };
			}
			if ((*item).buildingType == building_type::PEOPLE_HEART) {
				(*item).entityRect = { (int)(*item).position.x, (int)(*item).position.y, 240, 160 };	//Todo: Use XML values for rect width, not hardcoded ones (but take these as reference)
				(*item).centerPos = { (float)((*item).position.x + (*item).entityRect.w / 2), (float)((*item).position.y + (*item).entityRect.h / 2) };
				(*item).groundPos = { (float)((*item).position.x + (*item).entityRect.w / 2), (float)((*item).position.y + (*item).entityRect.h) };
			}
			if ((*item).buildingType == building_type::RADAR) {
				(*item).entityRect = { (int)(*item).position.x, (int)(*item).position.y, 160, 100 };	//Todo: Use XML values for rect width, not hardcoded ones (but take these as reference)
				(*item).centerPos = { (float)((*item).position.x + (*item).entityRect.w / 2), (float)((*item).position.y + (*item).entityRect.h / 2) };
				(*item).groundPos = { (float)((*item).position.x + (*item).entityRect.w / 2), (float)((*item).position.y + (*item).entityRect.h) };
			}
			if ((*item).buildingType == building_type::TANK_FACTORY) {
				(*item).entityRect = { (int)(*item).position.x, (int)(*item).position.y, 280, 220 };	//Todo: Use XML values for rect width, not hardcoded ones (but take these as reference)
				(*item).centerPos = { (float)((*item).position.x + (*item).entityRect.w / 2), (float)((*item).position.y + (*item).entityRect.h / 2) };
				(*item).groundPos = { (float)((*item).position.x + (*item).entityRect.w / 2), (float)((*item).position.y + (*item).entityRect.h) };
			}

			(*item).active = true;
			(*item).selected = false;
			(*item).texture = buildingsTextures[int((*item).buildingType)];
			(*item).Start();

			for (int i = 0; i < entitiesVector.size(); i++) {

				if (entitiesVector[i] == nullptr) {
					entitiesVector[i] = (Entity*)(&(*item));
					break;
				}
			}

			activeBuildings++;
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

bool Entity_Manager::loadTextures()
{
	pugi::xml_parse_result result = unitsDocument.load_file("textures/troops/unitsDoc.xml");
	pugi::xml_parse_result result2 = BuildingsDocument.load_file("textures/buildings/BuildingsDoc.xml");

	if(result!=NULL)
	loadTroopsTextures();
	if (result2 != NULL)
	loadBuildingsTextures();

	objectTextures[int(object_type::TREE)] = myApp->tex->Load("maps/Tree_Tileset.png");

	return true;
}

bool Entity_Manager::loadTroopsTextures()
{
	for (pugi::xml_node Data = unitsDocument.child("Entity_Document").child("Troops").child("Soviet").child("Unit"); Data != NULL; Data = Data.next_sibling("Unit")) {

		switch (Data.attribute("id").as_int())
		{
		case(int(infantry_type::CONSCRIPT)):
			infantryTextures[int(infantry_type::CONSCRIPT)][0]= myApp->tex->Load(Data.attribute("TextPath").as_string());
			infantryTextures[int(infantry_type::CONSCRIPT)][1] = nullptr;
			break;

		case(int(infantry_type::BAZOOKA)):

			infantryTextures[int(infantry_type::BAZOOKA)][0] = myApp->tex->Load(Data.attribute("TextPath").as_string());
			infantryTextures[int(infantry_type::BAZOOKA)][1] = myApp->tex->Load(Data.attribute("TextPathTwo").as_string());

			break;

		case(int(infantry_type::DESOLATOR)):

			infantryTextures[int(infantry_type::DESOLATOR)][0] = myApp->tex->Load(Data.attribute("TextPath").as_string());
			infantryTextures[int(infantry_type::DESOLATOR)][1] = nullptr;
			break;

		case(int(infantry_type::CHRONO)):

			infantryTextures[int(infantry_type::CHRONO)][0] = myApp->tex->Load(Data.attribute("TextPath").as_string());
			infantryTextures[int(infantry_type::CHRONO)][1] = myApp->tex->Load(Data.attribute("TextPathTwo").as_string());


			break;

		case(int(infantry_type::SNIPER)):

			infantryTextures[int(infantry_type::SNIPER)][0] = myApp->tex->Load(Data.attribute("TextPath").as_string());
			infantryTextures[int(infantry_type::SNIPER)][1] = myApp->tex->Load(Data.attribute("TextPathTwo").as_string());
			break;

		}

	}
	for (pugi::xml_node Data = unitsDocument.child("Entity_Document").child("Troops").child("Capitalist").child("Unit"); Data; Data = Data.next_sibling("Unit")) {

		switch (Data.attribute("id").as_int())
		{
			case int(int(infantry_type::BASIC)) :

				infantryTextures[int(infantry_type::BASIC)][0] = NULL;
				infantryTextures[int(infantry_type::BASIC)][1] = myApp->tex->Load(Data.attribute("TextPath").as_string());
				break;

				case int(int(infantry_type::DOG)) :

				infantryTextures[int(infantry_type::DOG)][0] = NULL;
				infantryTextures[int(infantry_type::DOG)][1] = myApp->tex->Load(Data.attribute("TextPath").as_string());
				break;

			
		}
	}

	return true;
}

bool Entity_Manager::loadBuildingsTextures() {

	for (pugi::xml_node Data = BuildingsDocument.child("Buildings_Document").child("Building"); Data != NULL; Data = Data.next_sibling("Building")) {

		switch (Data.attribute("id").as_int())
		{
			case int(building_type::COMMAND_CENTER) :
				buildingsTextures[int(building_type::COMMAND_CENTER)] = myApp->tex->Load(Data.attribute("TextPath").as_string());

				break;

			case int(building_type::TANK_FACTORY) :
				buildingsTextures[int(building_type::TANK_FACTORY)] = myApp->tex->Load(Data.attribute("TextPath").as_string());

				break;

			case int(building_type::RADAR) :

				buildingsTextures[int(building_type::RADAR)] = myApp->tex->Load(Data.attribute("TextPath").as_string());
				break;

			case int(building_type::GOLDYARD) :

				buildingsTextures[int(building_type::GOLDYARD)] = myApp->tex->Load(Data.attribute("TextPath").as_string());
				break;

				case int(building_type::HTPC) :

					buildingsTextures[int(building_type::HTPC)] = myApp->tex->Load(Data.attribute("TextPath").as_string());

				break;


				case int(building_type::PEOPLE_HEART) :

					buildingsTextures[int(building_type::PEOPLE_HEART)] = myApp->tex->Load(Data.attribute("TextPath").as_string());
					break;

				case int(building_type::EPC) :
					buildingsTextures[int(building_type::EPC)] = myApp->tex->Load(Data.attribute("TextPath").as_string());

					break;

		}



	}


	return true;
}


bool Entity_Manager::LoadEntityData() {

	bool ret=false;
	pugi::xml_parse_result result = unitsDocument.load_file("textures/troops/unitsDoc.xml");
	pugi::xml_parse_result result2 = BuildingsDocument.load_file("textures/buildings/BuildingsDoc.xml");

	if (result != NULL && result!=NULL)
	{
		AssignAnimData("Soviet");
		AssignAnimData("Capitalist");
		LoadBuildingsData();

		ret = true;
	}

	return ret;
}


bool Entity_Manager::LoadBuildingsData() {

	bool ret=true;
	SDL_Rect temp;


	for (pugi::xml_node DataXML = BuildingsDocument.child("Buildings_Document").child("Building"); DataXML; DataXML = DataXML.next_sibling("Building")) {

		pugi::xml_parse_result TiledFile = TiledDocument.load_file(DataXML.attribute("TiledFile").as_string());

		if (TiledFile != NULL) {

			for (pugi::xml_node DataTMX = TiledDocument.child("map").child("objectgroup").child("object"); DataTMX && ret; DataTMX = DataTMX.next_sibling("object")) {

				temp.x = DataTMX.attribute("x").as_int();
				temp.y = DataTMX.attribute("y").as_int();
				temp.w = DataTMX.attribute("width").as_int();
				temp.h = DataTMX.attribute("height").as_int();

				

				std::string tempString = DataTMX.attribute("name").as_string();
				int id = DataXML.attribute("id").as_int();

				
				if (tempString == "Spawn") {

					BuildingAnimationArray[id][(int)Building_State::SPAWN].PushBack(temp);
					BuildingAnimationArray[id][(int)Building_State::SPAWN].loop = false;
					BuildingAnimationArray[id][(int)Building_State::SPAWN].speed = 10.0f;

				}
				else if (tempString == "Idle") {

					BuildingAnimationArray[id][(int)Building_State::IDLE].PushBack(temp);
					BuildingAnimationArray[id][(int)Building_State::IDLE].loop = true;
					BuildingAnimationArray[id][(int)Building_State::IDLE].speed = 6.0f;

				}
				else if (tempString == "Explosion") {

					BuildingAnimationArray[id][(int)Building_State::DESTROYED].PushBack(temp);
					BuildingAnimationArray[id][(int)Building_State::DESTROYED].loop = false;
					BuildingAnimationArray[id][(int)Building_State::DESTROYED].speed = 15.0f;

				}
			}
		}
	}

	return ret;
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

					case (int(infantry_type::BASIC)) :
						temp.x += DataXML.child("RectOffset").attribute("x").as_int();
						temp.y += DataXML.child("RectOffset").attribute("y").as_int();
						temp.w = DataXML.child("RectOffset").attribute("w").as_int();
						temp.h = DataXML.child("RectOffset").attribute("h").as_int();

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

					case (int(infantry_type::CHRONO)):

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

					animationArray[id][int(unit_state::IDLE)][degreesToArray].PushBack(temp);
					animationArray[id][int(unit_state::IDLE)][degreesToArray].loop = true;
					animationArray[id][int(unit_state::IDLE)][degreesToArray].speed = 10.0f;

				}
				else if (tempString == "Walking") {

					animationArray[id][int(unit_state::MOVING)][degreesToArray].PushBack(temp);
					animationArray[id][int(unit_state::MOVING)][degreesToArray].loop = true;
					animationArray[id][int(unit_state::MOVING)][degreesToArray].speed = 5.0f;

				}
				else if (tempString == "Shot") {

					animationArray[id][int(unit_state::ATTACKING)][degreesToArray].PushBack(temp);
					animationArray[id][int(unit_state::ATTACKING)][degreesToArray].loop = true;
					animationArray[id][int(unit_state::ATTACKING)][degreesToArray].speed = 10.0f;


				}
				else if (tempString == "DeathOne") {

					animationArray[id][int(unit_state::DEAD)][0].PushBack(temp);
					animationArray[id][int(unit_state::DEAD)][0].loop = false;
					animationArray[id][int(unit_state::DEAD)][0].speed = DataXML.child("AnimDet").attribute("DeathOneSpeed").as_float();


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

void Entity_Manager::SolveOverlapping()
{
	BROFILER_CATEGORY("Unit overlapping", Profiler::Color::Magenta);

	for (int i = 0; i< unitPool.size() ; ++i) 
	{
		if (unitPool[i].active && unitPool[i].unitState!=unit_state::DEAD) 
		{

			std::vector<Entity*> nearEntities = entitiesQuadtree->GetEntitiesNear(unitPool[i].position.x, unitPool[i].position.y);

			SDL_Rect rect1 = unitPool[i].entityRect;
			rect1.y = unitPool[i].groundPos.y - 10;
			rect1.h = 10;

			for (int b = 0; b < nearEntities.size(); ++b)
			{
				bool dynamicNeighbor = nearEntities[b]->type == entity_type::INFANTRY;

				SDL_Rect rect2 = nearEntities[b]->entityRect;
				rect2.y = nearEntities[b]->groundPos.y - 10;
				rect2.h = 10;

				if (RectsOverlap(/*unitPool[i].entityRect*/ rect1,rect2 /*nearEntities[b]->entityRect*/) && 
					unitPool[i].entityRect.x != nearEntities[b]->entityRect.x &&
					unitPool[i].entityRect.y != nearEntities[b]->entityRect.y)
				{
					
					float distances[(int)OVERLAP_DIR::MAX];
					distances[(int)OVERLAP_DIR::RIGHT] = unitPool[i].entityRect.x + unitPool[i].entityRect.w - nearEntities[b]->entityRect.x;
					distances[(int)OVERLAP_DIR::LEFT] = nearEntities[b]->entityRect.x + nearEntities[b]->entityRect.w - unitPool[i].entityRect.x;
					distances[(int)OVERLAP_DIR::UP] = unitPool[i].entityRect.y + unitPool[i].entityRect.h - nearEntities[b]->entityRect.y;
					distances[(int)OVERLAP_DIR::DOWN] = nearEntities[b]->entityRect.y + nearEntities[b]->entityRect.h - unitPool[i].entityRect.y;

					int overlap_dir = -1;

					for (uint i = 0; i < (int)OVERLAP_DIR::MAX; ++i)
					{
						if (overlap_dir == -1)
							overlap_dir = i;
						else if (distances[i] < distances[(int)overlap_dir])
							overlap_dir = i;
					}

					switch ((OVERLAP_DIR)overlap_dir)
					{
					case OVERLAP_DIR::RIGHT:
						unitPool[i].position.x -= distances[(int)OVERLAP_DIR::RIGHT] * 0.1f;
						unitPool[i].centerPos.x -= distances[(int)OVERLAP_DIR::RIGHT] * 0.1f;
						unitPool[i].groundPos.x -= distances[(int)OVERLAP_DIR::RIGHT] * 0.1f;

						if (dynamicNeighbor) {
							nearEntities[b]->position.x += distances[(int)OVERLAP_DIR::RIGHT] * 0.1f;
							nearEntities[b]->centerPos.x += distances[(int)OVERLAP_DIR::RIGHT] * 0.1f;
							nearEntities[b]->groundPos.x += distances[(int)OVERLAP_DIR::RIGHT] * 0.1f;
						}
						break;
					case OVERLAP_DIR::LEFT:
						unitPool[i].position.x += distances[(int)OVERLAP_DIR::LEFT] * 0.1f;
						unitPool[i].centerPos.x += distances[(int)OVERLAP_DIR::LEFT] * 0.1f;
						unitPool[i].groundPos.x += distances[(int)OVERLAP_DIR::LEFT] * 0.1f;

						if (dynamicNeighbor) {
							nearEntities[b]->position.x -= distances[(int)OVERLAP_DIR::LEFT] * 0.1f;
							nearEntities[b]->centerPos.x -= distances[(int)OVERLAP_DIR::LEFT] * 0.1f;
							nearEntities[b]->groundPos.x -= distances[(int)OVERLAP_DIR::LEFT] * 0.1f;
						}
						break;
					case OVERLAP_DIR::UP:
						unitPool[i].position.y -= distances[(int)OVERLAP_DIR::UP] * 0.1f;
						unitPool[i].centerPos.y -= distances[(int)OVERLAP_DIR::UP] * 0.1f;
						unitPool[i].groundPos.y-= distances[(int)OVERLAP_DIR::UP] * 0.1f;

						if (dynamicNeighbor) {
							nearEntities[b]->position.y += distances[(int)OVERLAP_DIR::UP] * 0.1f;
							nearEntities[b]->centerPos.y += distances[(int)OVERLAP_DIR::UP] * 0.1f;
							nearEntities[b]->groundPos.y += distances[(int)OVERLAP_DIR::UP] * 0.1f;
						}
						break;
					case OVERLAP_DIR::DOWN:
						unitPool[i].position.y += distances[(int)OVERLAP_DIR::DOWN] * 0.1f;
						unitPool[i].centerPos.y += distances[(int)OVERLAP_DIR::DOWN] * 0.1f;
						unitPool[i].groundPos.y += distances[(int)OVERLAP_DIR::DOWN] * 0.1f;

						if (dynamicNeighbor) {
							nearEntities[b]->position.y -= distances[(int)OVERLAP_DIR::DOWN] * 0.1f;
							nearEntities[b]->centerPos.y -= distances[(int)OVERLAP_DIR::DOWN] * 0.1f;
							nearEntities[b]->groundPos.y -= distances[(int)OVERLAP_DIR::DOWN] * 0.1f;
						}
						break;
					}

				}
			}
		}
	}

	for (int i = 0; i < launcherPool.size(); ++i)
	{
		if (launcherPool[i].active && launcherPool[i].unitState != unit_state::DEAD)
		{

			std::vector<Entity*> nearEntities = entitiesQuadtree->GetEntitiesNear(launcherPool[i].position.x, launcherPool[i].position.y);

			SDL_Rect rect1 = launcherPool[i].entityRect;
			rect1.y = launcherPool[i].groundPos.y - 10;
			rect1.h = 10;

			for (int b = 0; b < nearEntities.size(); ++b)
			{
				bool dynamicNeighbor = nearEntities[b]->type == entity_type::INFANTRY;

				SDL_Rect rect2 = nearEntities[b]->entityRect;
				rect2.y = nearEntities[b]->groundPos.y - 10;
				rect2.h = 10;

				if (RectsOverlap(/*launcherPool[i].entityRect*/ rect1, rect2 /*nearEntities[b]->entityRect*/) &&
					launcherPool[i].entityRect.x != nearEntities[b]->entityRect.x &&
					launcherPool[i].entityRect.y != nearEntities[b]->entityRect.y)
				{

					float distances[(int)OVERLAP_DIR::MAX];
					distances[(int)OVERLAP_DIR::RIGHT] = launcherPool[i].entityRect.x + launcherPool[i].entityRect.w - nearEntities[b]->entityRect.x;
					distances[(int)OVERLAP_DIR::LEFT] = nearEntities[b]->entityRect.x + nearEntities[b]->entityRect.w - launcherPool[i].entityRect.x;
					distances[(int)OVERLAP_DIR::UP] = launcherPool[i].entityRect.y + launcherPool[i].entityRect.h - nearEntities[b]->entityRect.y;
					distances[(int)OVERLAP_DIR::DOWN] = nearEntities[b]->entityRect.y + nearEntities[b]->entityRect.h - launcherPool[i].entityRect.y;

					int overlap_dir = -1;

					for (uint i = 0; i < (int)OVERLAP_DIR::MAX; ++i)
					{
						if (overlap_dir == -1)
							overlap_dir = i;
						else if (distances[i] < distances[(int)overlap_dir])
							overlap_dir = i;
					}

					switch ((OVERLAP_DIR)overlap_dir)
					{
					case OVERLAP_DIR::RIGHT:
						launcherPool[i].position.x -= distances[(int)OVERLAP_DIR::RIGHT] * 0.1f;
						launcherPool[i].centerPos.x -= distances[(int)OVERLAP_DIR::RIGHT] * 0.1f;
						launcherPool[i].groundPos.x -= distances[(int)OVERLAP_DIR::RIGHT] * 0.1f;

						if (dynamicNeighbor) {
							nearEntities[b]->position.x += distances[(int)OVERLAP_DIR::RIGHT] * 0.1f;
							nearEntities[b]->centerPos.x += distances[(int)OVERLAP_DIR::RIGHT] * 0.1f;
							nearEntities[b]->groundPos.x += distances[(int)OVERLAP_DIR::RIGHT] * 0.1f;
						}
						break;
					case OVERLAP_DIR::LEFT:
						launcherPool[i].position.x += distances[(int)OVERLAP_DIR::LEFT] * 0.1f;
						launcherPool[i].centerPos.x += distances[(int)OVERLAP_DIR::LEFT] * 0.1f;
						launcherPool[i].groundPos.x += distances[(int)OVERLAP_DIR::LEFT] * 0.1f;

						if (dynamicNeighbor) {
							nearEntities[b]->position.x -= distances[(int)OVERLAP_DIR::LEFT] * 0.1f;
							nearEntities[b]->centerPos.x -= distances[(int)OVERLAP_DIR::LEFT] * 0.1f;
							nearEntities[b]->groundPos.x -= distances[(int)OVERLAP_DIR::LEFT] * 0.1f;
						}
						break;
					case OVERLAP_DIR::UP:
						launcherPool[i].position.y -= distances[(int)OVERLAP_DIR::UP] * 0.1f;
						launcherPool[i].centerPos.y -= distances[(int)OVERLAP_DIR::UP] * 0.1f;
						launcherPool[i].groundPos.y -= distances[(int)OVERLAP_DIR::UP] * 0.1f;

						if (dynamicNeighbor) {
							nearEntities[b]->position.y += distances[(int)OVERLAP_DIR::UP] * 0.1f;
							nearEntities[b]->centerPos.y += distances[(int)OVERLAP_DIR::UP] * 0.1f;
							nearEntities[b]->groundPos.y += distances[(int)OVERLAP_DIR::UP] * 0.1f;
						}
						break;
					case OVERLAP_DIR::DOWN:
						launcherPool[i].position.y += distances[(int)OVERLAP_DIR::DOWN] * 0.1f;
						launcherPool[i].centerPos.y += distances[(int)OVERLAP_DIR::DOWN] * 0.1f;
						launcherPool[i].groundPos.y += distances[(int)OVERLAP_DIR::DOWN] * 0.1f;

						if (dynamicNeighbor) {
							nearEntities[b]->position.y -= distances[(int)OVERLAP_DIR::DOWN] * 0.1f;
							nearEntities[b]->centerPos.y -= distances[(int)OVERLAP_DIR::DOWN] * 0.1f;
							nearEntities[b]->groundPos.y -= distances[(int)OVERLAP_DIR::DOWN] * 0.1f;
						}
						break;
					}

				}
			}
		}
	}

}

bool Entity_Manager::RectsOverlap(SDL_Rect &rect1, SDL_Rect &rect2) const
{
	return !(	rect1.x > rect2.x + rect2.w ||
				rect1.x + rect1.w < rect2.x ||
				rect1.y > rect2.y + rect2.h ||
				rect1.y + rect1.h < rect2.y
			);
}