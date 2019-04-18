#ifndef ENTITY_MANAGER_
#define ENTITY_MANAGER_

#include "Module.h"
#include "Entity.h"
#include "Unit.h"
#include "Soldier.h"
#include "Static_Object.h"
#include "PugiXml\src\pugixml.hpp"
#include "Base.h"
#include "Group.h"


#define TIMES_PER_SEC 5

#define OBJECTS_LIST_SIZE 50
#define SOLDIERS_LIST_SIZE 100
#define BASES_LIST_SIZE 5

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

	bool CreateObject(fPoint position, object_type objectType);
	bool CreateBase(fPoint position, base_type baseType);
	bool CreateSoldier(fPoint position, soldier_type soldierType, entity_faction entityFaction = entity_faction::NEUTRAL);

	void DestroyEntity(Entity *Entity) {}

public:

	//Entity lists
	Entity*			entitiesArray[2 * SOLDIERS_LIST_SIZE + OBJECTS_LIST_SIZE + BASES_LIST_SIZE]; //List with all the entities
	
	Unit*			urssUnitsArray[SOLDIERS_LIST_SIZE];			//Player units (soldiers+vehicles)
	Unit*			eeuuUnitsArray[SOLDIERS_LIST_SIZE];			//Enemy units (soldiers+vehicles)
	
	Soldier*		urssSoldiersArray[SOLDIERS_LIST_SIZE];		//Player soldiers
	Soldier*		eeuuSoldiersArray[SOLDIERS_LIST_SIZE];		//Enemy soldiers
	
	Static_Object*	staticObjectsArray[OBJECTS_LIST_SIZE];		//Static objects
	
	Base*			basesArray[BASES_LIST_SIZE];				//Bases

private:

	//Arrays with all the textures
	SDL_Texture*	basesTextures[int(base_type::BASE_MAX)];
	SDL_Texture*	soldierTextures[int(soldier_type::SOLDIER_MAX)];
	SDL_Texture*	objectTextures[int(object_type::OBJECT_MAX)];

	//Unit stats
	unit_stats		soldierStats[int(soldier_type::SOLDIER_MAX)];

	//TO IMPLEMENT: Array with 2d arrays, or 3d arrays
	//Animation[soldiertypes][state][direction]

	//Update Timers
	float accumulated_time = 0.0f;
	float update_ms_cycle = 0.0f;
	int times_per_sec = 0;
	bool do_logic = false;

};

#endif