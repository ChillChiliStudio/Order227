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
#include "UserInterface.h"
#include "Image.h"

Entity_Manager::Entity_Manager()
{
	name.assign("entities");
	AllocateUnitsPool();
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
	lifeBar_tex = myApp->tex->Load("ui/Life_Icon.png");

	//Activate Buildings & Objects
	ActivateBuildings();
	ActivateObjects();

	//Tell Units who their enemies are
	for (int i = 0; i < CommunistUnitsArray.size(); ++i)
		CommunistUnitsArray[i]->hostileUnits = CapitalistUnitsArray;
		
	for (int i = 0; i < CapitalistUnitsArray.size(); ++i) {

		CapitalistUnitsArray[i]->hostileUnits = CommunistUnitsArray;
		CapitalistUnitsArray[i]->hostileBuildings = BuildingsList;
	}


	LoadEntityData();

	//Set up stats of units
	SetupUnitStats();

	return true;
}


bool Entity_Manager::CleanUp() {

	LOG("Clean Up Entity_Manager");

	//Clean Units
	if (CommunistUnitsArray.size() > 0) {

		for (int i = 0; i < CommunistUnitsArray.size(); i++)
			RELEASE(CommunistUnitsArray[i]);

		CommunistUnitsArray.clear();
	}

	if (CapitalistUnitsArray.size() > 0) {

		for (int i = 0; i < CapitalistUnitsArray.size(); i++)
			RELEASE(CapitalistUnitsArray[i]);

		CapitalistUnitsArray.clear();
	}

	//Clean objects
	if (ObjectsList.size() > 0) {

		std::list<Static_Object*>::iterator item = ObjectsList.begin();
		for (; (*item); item = next(item))
		{
			(*item)->CleanUp();
			RELEASE(*item);
		}

		ObjectsList.clear();
	}

	//Clean Buildings
	if (BuildingsList.size() > 0) {

		std::list<Building*>::iterator item2 = BuildingsList.begin();
		for (; (*item2); item2 = next(item2))
		{
			(*item2)->CleanUp();
			RELEASE(*item2);
		}

		BuildingsList.clear();
	}

	//Finally, Clean Entities
	if (EntitiesArray.size() > 0) {

		for (int i = 0; i < EntitiesArray.size(); ++i)
			if (EntitiesArray[i] != nullptr)
				RELEASE(EntitiesArray[i]);

		EntitiesArray.clear();
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


	if (myApp->gui->MainMenuTemp_Image->active != true) {

		if (accumulated_time >= update_ms_cycle)
			do_logic = true;

		for (int i = 0; i < CommunistUnitsArray.size(); ++i) {

			if (CommunistUnitsArray[i]->active == true)
				CommunistUnitsArray[i]->Update(dt);

			if (do_logic)
				CommunistUnitsArray[i]->FixUpdate(dt);
		}

		for (int i = 0; i < CapitalistUnitsArray.size(); ++i) {

			if (CapitalistUnitsArray[i]->active == true)
				CapitalistUnitsArray[i]->Update(dt);

			if (do_logic)
				CapitalistUnitsArray[i]->FixUpdate(dt);
		}

		std::list<Building*>::iterator item = BuildingsList.begin();
		for (; (*item); item = next(item))
			if ((*item)->active == true)
				(*item)->Update(dt);

		std::list<Static_Object*>::iterator item2 = ObjectsList.begin();
		for (; (*item2); item2 = next(item2))
			if ((*item2)->active == true)
				(*item2)->Update(dt);


		accumulated_time -= update_ms_cycle;
		myApp->render->OrderBlit(myApp->render->OrderToRender);
	}

	return true;
}


bool Entity_Manager::ActivateUnit(fPoint position, infantry_type infantryType, entity_faction entityFaction)
{

	bool ret = false;

	//Player troops
	if (entityFaction == entity_faction::COMMUNIST)
	{
		for (int i = 0; i < CommunistUnitsArray.size(); ++i)
		{
			if (CommunistUnitsArray[i]->active == false)
			{

				CommunistUnitsArray[i]->position = position;
				CommunistUnitsArray[i]->infantryType = infantryType;
				CommunistUnitsArray[i]->texture = infantryTextures[int(infantryType)];
				CommunistUnitsArray[i]->stats = infantryStats[int(infantryType)];
				

				CommunistUnitsArray[i]->UnitRect.w = 45;
				CommunistUnitsArray[i]->UnitRect.h = 55;
				CommunistUnitsArray[i]->unitState = unit_state::IDLE;
				CommunistUnitsArray[i]->unitOrders = unit_orders::HOLD;
				CommunistUnitsArray[i]->unitDirection = unit_directions::SOUTH_EAST;


				CommunistUnitsArray[i]->active = true;
				CommunistUnitsArray[i]->selected = false;

				CommunistUnitsArray[i]->currentAnimation = &myApp->entities->animationArray[int(infantryType)][int(CommunistUnitsArray[i]->unitState)][int(CommunistUnitsArray[i]->unitDirection)];
				myApp->gui->CreateLifeBar(fPoint(position.x, position.y), CommunistUnitsArray[i], lifeBar_tex);

				ret = true;
				break;
			}
		}

		if (ret == false) {

			CommunistUnitsArray.resize(RESIZE_VALUE);
			ActivateUnit(position, infantryType, entityFaction);
		}

	}

	//Enemy troops
	else if (entityFaction == entity_faction::CAPITALIST)
	{
		for (int i = 0; i < CapitalistUnitsArray.size(); ++i)
		{
			if (CapitalistUnitsArray[i]->active == false)
			{

				CapitalistUnitsArray[i]->position = position;
				CapitalistUnitsArray[i]->infantryType = infantryType;
				CapitalistUnitsArray[i]->texture = infantryTextures[int(infantryType)];
				CapitalistUnitsArray[i]->stats = infantryStats[int(infantryType)];


				CapitalistUnitsArray[i]->UnitRect.w = 45;
				CapitalistUnitsArray[i]->UnitRect.h = 55;
				CapitalistUnitsArray[i]->unitState = unit_state::IDLE;
				CapitalistUnitsArray[i]->unitOrders = unit_orders::HOLD;
				CommunistUnitsArray[i]->unitDirection = unit_directions::SOUTH_EAST;


				CapitalistUnitsArray[i]->active = true;
				CapitalistUnitsArray[i]->selected = false;

				CapitalistUnitsArray[i]->currentAnimation = &myApp->entities->animationArray[int(infantryType)][int(CapitalistUnitsArray[i]->unitState)][int(CapitalistUnitsArray[i]->unitDirection)];
				myApp->gui->CreateLifeBar(fPoint(position.x, position.y), CapitalistUnitsArray[i], lifeBar_tex);

				ret = true;
				break;
			}
		}

		if (ret == false) {

			CapitalistUnitsArray.resize(RESIZE_VALUE);
			ActivateUnit(position, infantryType, entityFaction);
		}
	}

	return ret;
}


bool Entity_Manager::DeActivateUnit(Unit* Unit) {

	Unit->stats = infantryStats[int(infantry_type::INFANTRY_NONE)];
	Unit->infantryType = infantry_type::INFANTRY_NONE;
	Unit->position = fPoint(0.0f, 0.0f);
	Unit->texture = nullptr;
	Unit->active = false;
	Unit->selected = false;
	Unit->currentAnimation = &myApp->entities->animationArray[int(infantry_type::INFANTRY_NONE)][int(unit_state::NONE)][int(unit_directions::NONE)];

	return true;
}


void Entity_Manager::ActivateBuildings()
{

	std::list<Building*>::iterator item = BuildingsList.begin();
	for (; (*item); item = next(item)) {

		if ((*item)->buildingType == building_type::BUILDING_MAX || (*item)->buildingType == building_type::BUILDING_NONE) {

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

		if ((*item)->objectType != object_type::OBJECT_NONE || (*item)->objectType != object_type::OBJECT_MAX) {

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


void Entity_Manager::AllocateUnitsPool() {


	ReleaseUnitsPool();

	//Allocate Memory for Units and place them also in entities array
	for (int i = 0; i < myApp->UnitsInitialSize; ++i) {

		Infantry *CommunistInf = new Infantry({ 0,0 }, infantry_type::INFANTRY_NONE, entity_faction::COMMUNIST);
		CommunistInf->active = false;

		Infantry *CapitalistInf = new Infantry({ 0,0 }, infantry_type::INFANTRY_NONE, entity_faction::CAPITALIST);
		CapitalistInf->active = false;

		CommunistUnitsArray.push_back(CommunistInf);
		CapitalistUnitsArray.push_back(CapitalistInf);

		EntitiesArray.push_back(CommunistInf);
		EntitiesArray.push_back(CapitalistInf);
	}
}


void Entity_Manager::ReleaseUnitsPool() {

	if (CommunistUnitsArray.size() > 0) {

		for (int i = 0; i < CommunistUnitsArray.size(); i++)
			RELEASE(CommunistUnitsArray[i]);

		CommunistUnitsArray.clear();
	}

	if (CapitalistUnitsArray.size() > 0) {

		for (int i = 0; i < CapitalistUnitsArray.size(); i++)
			RELEASE(CapitalistUnitsArray[i]);

		CapitalistUnitsArray.clear();
	}

	if (EntitiesArray.size() > 0) {

		std::vector<Entity*>::iterator item = EntitiesArray.begin();
		for (; (*item); item = next(item))
			if ((*item)->type == entity_type::INFANTRY)
				EntitiesArray.erase(item);

	}
}