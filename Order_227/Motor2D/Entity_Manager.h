#ifndef ENTITY_MANAGER_
#define ENTITY_MANAGER_

#include "Module.h"
#include "Entity.h"
#include "Unit.h"
#include "Infantry.h"
#include "Static_Object.h"
#include "Building.h"
#include "Group.h"
#include "Animation.h"

#include <vector>

#include "PugiXml\src\pugixml.hpp"


#define TIMES_PER_SEC 5
#define TROOP_TYPES 2
#define RESIZE_VALUE 100

class Entity_Manager : public Module
{

public:

	Entity_Manager();
	~Entity_Manager();

public:

	bool Awake(pugi::xml_node& config);
	bool Start();			//Load textures here
	bool PreUpdate();
	bool Update(float dt);
	bool CleanUp();

public:

	int UnitsInitialSize = 0;

	Unit* ActivateUnit(fPoint position, infantry_type infantryType, entity_faction entityFaction = entity_faction::NEUTRAL);
	bool DeActivateUnit(Unit* Unit);
	void AllocateUnitsPool(int size);
	void ReleasePools();

	void ActivateBuildings();
	void ActivateObjects();

	void DestroyEntity(Entity *Entity) {}

	bool SetupUnitStats();
	SDL_Rect SetupTreeType();

public:

	//Arrays for Units, Objects & Entities
	std::vector<Entity*> EntitiesArray;

	/*std::vector<Unit*> CommunistUnitsArray;
	std::vector<Unit*> CapitalistUnitsArray;*/
	std::vector<Unit*> UnitsPool;

	std::list<Unit*> ActiveCommunistUnits;
	std::list<Unit*> ActiveCapitalistUnits;

	//A list for buildings and objects (not need to make it a dynamic array)
	std::list<Building*> BuildingsList;
	std::list<Static_Object*> ObjectsList;

	//Last Unit activated Pointer
	Unit *lastUnitActivated = nullptr;

	//Main Base Pointer
	Building* mainBase = nullptr;	//TODO: This is here because of the lack of lists, having an "attackable buildings" list to read for capitalist units would be better
	
	//Animations Array
	Animation animationArray[TROOP_TYPES][int(unit_state::MAX_STATES)][int(unit_directions::MAX_DIRECTIONS)]; //TODO_ WTF? Troop types?

	bool entitiesDebugDraw = false;
	SDL_Texture* lifeBar_tex = nullptr; //TODO: Why is this here?

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
