#ifndef ENTITY_MANAGER_
#define ENTITY_MANAGER_

#include "Module.h"
#include "Entity.h"
#include "Unit.h"
#include "Group.h"
#include "Main_Base.h"
#include "PugiXml\src\pugixml.hpp"


#define TIMES_PER_SEC 5

class EntityManager : public Module		//TODO: Change class name so it follows guideline standards
{
public:

	EntityManager();
	~EntityManager();

public:

	bool Awake();
	bool Start() override;
	bool PreUpdate() override;
	bool Update(float dt) override;
	bool CleanUp() override;

public:

	Entity *CreateEntity(entity_type entityType, fPoint position);
	Entity *CreateUnit(unit_type unitType, fPoint position, faction_enum faction);

	void DestroyEntity(Entity *Entity);
	
	void SelectUnit(SDL_Rect rect);
	void SelectUnit(iPoint position);
	void CreateGroup();
	void CreateGroupForPlayer();
	void EmptyPlayerGroup();
	void AddUnitsPlayerGroup();
	void MoveUnits(std::list<Unit*> list,iPoint origin, iPoint destination);

public:

	std::list<Entity*> entities_list;
	std::list<Unit*> comunist_list;
	std::list<Unit*> capitalist_list;
	
	Group playerGroup;

	float accumulated_time = 0.0f;
	float update_ms_cycle = 0.0f;
	int times_per_sec = 0;
	bool do_logic = false;
	
};

#endif