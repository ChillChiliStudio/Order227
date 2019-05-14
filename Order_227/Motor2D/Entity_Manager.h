#ifndef ENTITY_MANAGER_
#define ENTITY_MANAGER_

#include <vector>
#include "Module.h"
#include "Entity.h"
#include "Unit.h"
#include "Static_Object.h"
#include "Building.h"
#include "Group.h"
#include "Animation.h"

#define TIMES_PER_SEC 5
#define TROOP_TYPES 7
#define RESIZE_VALUE 50



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

	//Pools	//TODO: With .reserve() we can reserve memory for a vector so if a resize is needed in runtime the memory is already allocated, making the process faster
	void AllocateEntityPool();
	void AllocateUnitPool();
	void AllocateHitscanPool();
	void AllocateRangedPool();
	void AllocateTankPool();
	void ReleasePools();

	void UpdateBlitOrdering();
	void BlitEntities();

	Unit* ActivateUnit(fPoint position, infantry_type infantryType, entity_faction entityFaction = entity_faction::NEUTRAL);
	bool DeActivateUnit(Unit* Unit);

	void ActivateBuildings();
	void ActivateObjects();

	bool SetupUnitStats();
	bool AssignStats(std::string faction);
	bool AssignAnimData(std::string faction);
	SDL_Rect SetupTreeType();

public:

	//Pool sizes
	int unitsPoolSize = 0;

	//Pools
	std::vector<Unit>			unitPool;
	//std::vector<Hitscan>		hitscanPool;
	//std::vector<Ranged>		rangedPool;
	//std::vector<Tank>			tankPool;

	std::vector<Static_Object>	objectsArray;
	std::vector<Building>		buildingsArray;
	std::vector<Entity*>		entitiesVector;

	//Last Unit activated Pointer
	Unit *lastUnitActivated = nullptr;

	//Main Base Pointer
	Building* mainBase = nullptr;	//TODO: This is here because of the lack of lists, having an "attackable buildings" list to read for capitalist units would be better
	
	//Animations Array
	Animation animationArray[TROOP_TYPES][int(unit_state::MAX_STATES)][int(unit_directions::MAX_DIRECTIONS)][2]; //TODO_ WTF? Troop types?
	Animation BuildingAnimationArray[int(building_type::BUILDING_MAX)][2];

	bool entitiesDebugDraw = false;
	SDL_Texture* lifeBar_tex = nullptr; //TODO: Why is this here?4

	//Unit stats
	unit_stats		infantryStats[int(infantry_type::INFANTRY_MAX)];

	bool heavyUnitsUnlocked = true;
private:

	bool LoadEntityData();
	bool LoadBuildingsData();
	bool loadTextures();
	bool loadTroopsTextures();
	bool loadBuildingsTextures();


	//TilesetFile
	pugi::xml_document	tilsetTexture;

	//Arrays with all the textures
	SDL_Texture*	buildingsTextures[int(building_type::BUILDING_MAX)];
	SDL_Texture*	infantryTextures[int(infantry_type::INFANTRY_MAX)];
	SDL_Texture*	objectTextures[int(object_type::OBJECT_MAX)];

	
	pugi::xml_document unitsDocument;
	pugi::xml_document BuildingsDocument;
	pugi::xml_document TiledDocument;
	//TO IMPLEMENT: Array with 2d arrays, or 3d arrays
	//Animation[soldiertypes][state][direction]

	//Update Timers
	float accumulated_time = 0.0f;
	float update_ms_cycle = 0.0f;
	int times_per_sec = 0;
	bool do_logic = false;

};

#endif
