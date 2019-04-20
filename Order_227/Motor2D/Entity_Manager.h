#ifndef ENTITY_MANAGER_
#define ENTITY_MANAGER_

#include "Module.h"
#include "Entity.h"
#include "Unit.h"
#include "Infantry.h"
#include "Static_Object.h"
#include "PugiXml\src\pugixml.hpp"
#include "Building.h"
#include "Group.h"
#include "Animation.h"


#define TIMES_PER_SEC 5

#define OBJECTS_ARRAY_SIZE 50
#define INFANTRY_ARRAY_SIZE 100
#define BUILDINGS_ARRAY_SIZE 5
#define UNITS_ARRAY_SIZE (2 * INFANTRY_ARRAY_SIZE + OBJECTS_ARRAY_SIZE + BUILDINGS_ARRAY_SIZE)

class Entity_Manager : public Module
{

public:

	Entity_Manager();
	~Entity_Manager();

public:

	bool Awake();
	bool Start();			//Load textures here
	bool PreUpdate();
	bool Update(float dt);
	bool CleanUp();

public:

	bool ActivateObject(fPoint position, object_type objectType);
	bool ActivateBuilding(fPoint position, building_type buildingType, entity_faction entityFaction = entity_faction::NEUTRAL);
	bool ActivateInfantry(fPoint position, infantry_type infantryType, entity_faction entityFaction = entity_faction::NEUTRAL);

	bool DeActivateObject(Static_Object* Object);
	bool DeActivateBuilding(Building* Building);
	bool DeActivateInfantry(Infantry* Infantry);

	void DestroyEntity(Entity *Entity) {}

	bool SetupUnitStats();

public:

	//Entity lists
	Entity*			entitiesArray[UNITS_ARRAY_SIZE]; //List with all the entities

	Unit*			CommunistUnitsArray[INFANTRY_ARRAY_SIZE];			//Player units (soldiers+vehicles)
	Unit*			CapitalistUnitsArray[INFANTRY_ARRAY_SIZE];			//Enemy units (soldiers+vehicles)

	Infantry*		CommunistInfantryArray[INFANTRY_ARRAY_SIZE];		//Player soldiers
	Infantry*		CapitalistInfantryArray[INFANTRY_ARRAY_SIZE];		//Enemy soldiers

	Static_Object*	staticObjectsArray[OBJECTS_ARRAY_SIZE];		//Static objects

	Building*		buildingsArray[BUILDINGS_ARRAY_SIZE];				//Bases

																//Animations Array
	Animation		animationArray[1][int(unit_state::MAX_STATES)][int(unit_directions::MAX_DIRECTIONS)];

	bool entitiesDebugDraw = false;

private:

	bool LoadEntityData();
	bool loadTextures();

	//TilesetFile
	pugi::xml_document	tilsetTexture;

	//Arrays with all the textures
	SDL_Texture*	buildingsTextures[int(building_type::BUILDING_MAX)];
	SDL_Texture*	infantryTextures[int(infantry_type::INFANTRY_MAX)];
	SDL_Texture*	objectTextures[int(object_type::OBJECT_MAX)];



	//Unit stats
	unit_stats		infantryStats[int(infantry_type::INFANTRY_MAX)];
	pugi::xml_document unitsDocument;
	//TO IMPLEMENT: Array with 2d arrays, or 3d arrays
	//Animation[soldiertypes][state][direction]

	//Update Timers
	float accumulated_time = 0.0f;
	float update_ms_cycle = 0.0f;
	int times_per_sec = 0;
	bool do_logic = false;

};

#endif
