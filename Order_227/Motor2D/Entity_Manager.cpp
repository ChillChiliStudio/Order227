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

	//Allocate Memory for Units and place them also in entities array
	for (int i = 0; i < HALF_UNITS_INITIAL_SIZE; ++i) {

		Infantry *CommunistInf = new Infantry({ 0,0 }, infantry_type::INFANTRY_NONE, entity_faction::COMMUNIST);
		CommunistInf->active = false;

		Infantry *CapitalistInf = new Infantry({ 0,0 }, infantry_type::INFANTRY_NONE, entity_faction::CAPITALIST);
		CapitalistInf->active = false;

		CommunistUnitsArray.push_back(CommunistInf);
		CapitalistUnitsArray.push_back(CapitalistInf);

		EntitiesArray.push_back(CommunistInf);
		EntitiesArray.push_back(CapitalistInf);
	}

	//Initializing to nullptr non-initialized arrays
	ObjectsArray.assign(OBJECTS_INITIAL_SIZE, nullptr);
	buildingsArray.assign(BUILDINGS_SIZE, nullptr);

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


	//Add Buildings as entities
	for (int i = 0; i < BUILDINGS_SIZE; ++i) {

		if (buildingsArray[i] == nullptr) {

			Building* newBuilding = new Building({ 0, 0 }, building_type::BUILDING_NONE, entity_faction::NEUTRAL);
			newBuilding->active = false;
			buildingsArray[i] = newBuilding;

		}
		else {
			myApp->entities->ActivateBuilding(buildingsArray[i]->position, buildingsArray[i]->buildingType, buildingsArray[i]->faction);
			mainBase = buildingsArray[i];
		}

		EntitiesArray.push_back(buildingsArray[i]);
	}

	//Allocate Memory for Objects
	for (int i = 0; i < OBJECTS_INITIAL_SIZE; ++i) {

		if (ObjectsArray[i] == nullptr) {

			Static_Object *newObject = new Static_Object({ 0,0 }, object_type::OBJECT_NONE, entity_faction::NEUTRAL);
			newObject->active = false;

			ObjectsArray.push_back(newObject);
		}
		else
			myApp->entities->ActivateObject(ObjectsArray[i]->position, ObjectsArray[i]->objectType);

		EntitiesArray.push_back(ObjectsArray[i]);
	}

	//Tell Units who their enemies are
	for (int i = 0; i < HALF_UNITS_INITIAL_SIZE; ++i) {

		CommunistUnitsArray[i]->hostileUnits = CapitalistUnitsArray;
		CapitalistUnitsArray[i]->hostileUnits = CommunistUnitsArray;
		CapitalistUnitsArray[i]->hostileBuildings = buildingsArray;
	}


	LoadEntityData();

	//Set up stats of units
	SetupUnitStats();

	return true;
}


bool Entity_Manager::CleanUp() {

	LOG("Clean Up Entity_Manager");

	//Clean soldiers
	for (int i = 0; i < HALF_UNITS_INITIAL_SIZE; ++i)
	{

		CommunistUnitsArray[i]->CleanUp();
		CapitalistUnitsArray[i]->CleanUp();

		RELEASE(CommunistUnitsArray[i]);
		RELEASE(CapitalistUnitsArray[i]);
	}

	CommunistUnitsArray.clear();
	CapitalistUnitsArray.clear();

	//Clean objects
	for (int i = 0; i < OBJECTS_INITIAL_SIZE; ++i)
	{
		ObjectsArray[i]->CleanUp();
		RELEASE(ObjectsArray[i]);
	}

	ObjectsArray.clear();

	//Clean Buildings
	for (int i = 0; i < BUILDINGS_SIZE; ++i)
	{
		buildingsArray[i]->CleanUp();
		RELEASE(buildingsArray[i]);
	}

	buildingsArray.clear();

	//Finally, Clean Entities
	for (int i = 0; i < ENTITIES_INITIAL_SIZE; ++i)
		if (EntitiesArray[i] != nullptr)
			RELEASE(EntitiesArray[i]);

	EntitiesArray.clear();

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

		for (int i = 0; i < HALF_UNITS_INITIAL_SIZE; ++i) {

			if (CapitalistUnitsArray[i]->active == true)
				CapitalistUnitsArray[i]->Update(dt);

			if (CommunistUnitsArray[i]->active == true)
				CommunistUnitsArray[i]->Update(dt);

			if (do_logic)
			{
				CapitalistUnitsArray[i]->FixUpdate(dt);
				CommunistUnitsArray[i]->FixUpdate(dt);
			}
		}

		for (int i = 0; i < BUILDINGS_SIZE; ++i)
			if (buildingsArray[i]->active == true)
				buildingsArray[i]->Update(dt);

	for (int i = 0; i < OBJECTS_INITIAL_SIZE; ++i)
		//ObjectsArray[i]->Update(dt);

	for (int i = 0; i < OBJECTS_INITIAL_SIZE; ++i)
		//ObjectsArray[i]->Update(dt);


		accumulated_time -= update_ms_cycle;

		myApp->render->OrderBlit(myApp->render->OrderToRender);
	}
	return true;
}


Unit* Entity_Manager::ActivateInfantry(fPoint position, infantry_type infantryType, entity_faction entityFaction)
{
	Unit* unit = nullptr;
	//Player troops
	if (entityFaction == entity_faction::COMMUNIST)
	{
		for (int i = 0; i < HALF_UNITS_INITIAL_SIZE; ++i)
		{
			if (CommunistUnitsArray[i]->active == false)
			{

				CommunistUnitsArray[i]->position = position;
				CommunistUnitsArray[i]->texture = infantryTextures[int(infantryType)];
				CommunistUnitsArray[i]->stats = infantryStats[int(infantryType)];
				CommunistUnitsArray[i]->active = true;
				CommunistUnitsArray[i]->selected = false;
				CommunistUnitsArray[i]->infantryType = infantryType;
				CommunistUnitsArray[i]->UnitRect.w = 45;
				CommunistUnitsArray[i]->UnitRect.h = 55;
				CommunistUnitsArray[i]->Start();
				myApp->gui->CreateLifeBar(fPoint(position.x, position.y), CommunistUnitsArray[i], lifeBar_tex);

				//CommunistInfantryArray[i]->infantryType = infantryType;
				//To implement:: Update animations
				unit = CommunistUnitsArray[i];

				return unit;
			}
		}
	}

	//Enemy troops
	else if (entityFaction == entity_faction::CAPITALIST)
	{
		for (int i = 0; i < HALF_UNITS_INITIAL_SIZE; ++i)
		{
			if (CapitalistUnitsArray[i]->active == false)
			{

				CapitalistUnitsArray[i]->position = position;
				CapitalistUnitsArray[i]->texture = infantryTextures[int(infantryType)];
				CapitalistUnitsArray[i]->stats = infantryStats[int(infantryType)];
				CapitalistUnitsArray[i]->active = true;
				CapitalistUnitsArray[i]->selected = false;
				CapitalistUnitsArray[i]->infantryType = infantryType;
				CapitalistUnitsArray[i]->UnitRect.w = 45;
				CapitalistUnitsArray[i]->UnitRect.h = 55;
				CapitalistUnitsArray[i]->Start();
				//CapitalistInfantryArray[i]->infantryType = infantryType;
				//To implement:: Update animations
				myApp->gui->CreateLifeBar(fPoint(position.x, position.y), CapitalistUnitsArray[i], lifeBar_tex);

				unit = CapitalistUnitsArray[i];

				return unit;
			}
		}
	}

	return unit;
}


bool Entity_Manager::ActivateBuilding(fPoint position, building_type buildingType, entity_faction entityFaction)
{
	for (int i = 0; i < BUILDINGS_SIZE; ++i)
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
	for (int i = 0; i < OBJECTS_INITIAL_SIZE; ++i)
	{
		if (ObjectsArray[i]->active == false)
		{
			ObjectsArray[i]->position = position;
			ObjectsArray[i]->objectType = objectType;
			ObjectsArray[i]->active = true;
			ObjectsArray[i]->texture = objectTextures[int(objectType)];
			ObjectsArray[i]->selected = false;

			if (objectType == object_type::TREE)
				ObjectsArray[i]->UnitRect = SetupTreeType();

			return true;

		}
	}
	return false;
}




bool Entity_Manager::DeActivateInfantry(Unit* infantry)
{

	infantry->stats = infantryStats[int(infantry_type::INFANTRY_NONE)];
	infantry->infantryType = infantry_type::INFANTRY_NONE;
	infantry->position = fPoint(0.0f, 0.0f);
	infantry->texture = nullptr;
	infantry->active = false;
	infantry->selected = false;
	//

	//if (infantry->faction == entity_faction::COMMUNIST) {

	//	for (int i = 0; i < INFANTRY_ARRAY_SIZE; ++i) {

	//		if (CommunistUnitsArray[i] == infantry) {

	//			CommunistUnitsArray[i]->stats = infantryStats[int(infantry_type::INFANTRY_NONE)];
	//			//CommunistInfantryArray[i]->infantryType = infantry_type::INFANTRY_NONE;
	//			CommunistUnitsArray[i]->infantryType = infantry_type::INFANTRY_NONE;
	//			CommunistUnitsArray[i]->position = fPoint(0.0f, 0.0f);
	//			CommunistUnitsArray[i]->texture = nullptr;
	//			CommunistUnitsArray[i]->active = false;
	//			CommunistUnitsArray[i]->selected = false;
	//			//To implement:: Update animations

	//			return true;
	//		}
	//	}
	//}

	//////Enemy troops
	//else if (infantry->faction == entity_faction::CAPITALIST) {

	//	for (int i = 0; i < INFANTRY_ARRAY_SIZE; ++i) {

	//		if (CapitalistUnitsArray[i] == infantry) {

	//			CapitalistUnitsArray[i]->stats = infantryStats[int(infantry_type::INFANTRY_NONE)];
	//			//CommunistInfantryArray[i]->infantryType = infantry_type::INFANTRY_NONE;
	//			CapitalistUnitsArray[i]->infantryType = infantry_type::INFANTRY_NONE;
	//			CapitalistUnitsArray[i]->position = fPoint(0.0f, 0.0f);
	//			CapitalistUnitsArray[i]->texture = nullptr;
	//			CapitalistUnitsArray[i]->active = false;
	//			CapitalistUnitsArray[i]->selected = false;
	//			//To implement:: Update animations

	//			return true;
	//		}
	//	}
	//}

	return false;
}

bool Entity_Manager::DeActivateBuilding(Building* building)
{

	for (int i = 0; i < BUILDINGS_SIZE; ++i) {

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

	for (int i = 0; i < OBJECTS_INITIAL_SIZE; ++i) {

		if (ObjectsArray[i] == object) {

			ObjectsArray[i]->position = fPoint(0.0f, 0.0f);
			ObjectsArray[i]->faction = entity_faction::NEUTRAL;
			ObjectsArray[i]->objectType = object_type::OBJECT_NONE;
			ObjectsArray[i]->texture = nullptr;

			return true;
		}
	}

	return false;
}

bool Entity_Manager::ResetAll() {

	for (int i = 0; i < HALF_UNITS_INITIAL_SIZE; ++i) {

		CommunistUnitsArray[i]->stats = infantryStats[int(infantry_type::INFANTRY_NONE)];
		//CommunistInfantryArray[i]->infantryType = infantry_type::INFANTRY_NONE;
		CommunistUnitsArray[i]->infantryType = infantry_type::INFANTRY_NONE;
		CommunistUnitsArray[i]->position = fPoint(0.0f, 0.0f);
		CommunistUnitsArray[i]->texture = nullptr;
		CommunistUnitsArray[i]->active = false;
		CommunistUnitsArray[i]->selected = false;
		//To implement:: Update animations
	}

	for (int i = 0; i < HALF_UNITS_INITIAL_SIZE; ++i) {

		CapitalistUnitsArray[i]->stats = infantryStats[int(infantry_type::INFANTRY_NONE)];
		//CommunistInfantryArray[i]->infantryType = infantry_type::INFANTRY_NONE;
		CapitalistUnitsArray[i]->infantryType = infantry_type::INFANTRY_NONE;
		CapitalistUnitsArray[i]->position = fPoint(0.0f, 0.0f);
		CapitalistUnitsArray[i]->texture = nullptr;
		CapitalistUnitsArray[i]->active = false;
		CapitalistUnitsArray[i]->selected = false;
		//To implement:: Update animations
	}
	/*mainBase->Start();*/
	return true;
}

bool Entity_Manager::loadTextures() {

	//TODO This need to be charged by a XML
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
