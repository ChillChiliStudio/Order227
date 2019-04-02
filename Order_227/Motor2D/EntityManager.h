#ifndef ENTITY_MANAGER_
#define ENTITY_MANAGER_

#include "Module.h"
#include "Entity.h"
#include "Unit.h"
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
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool CleanUp();

public:

	Entity *CreateEntity(entity_type entityType, fPoint position);
	Entity *CreateUnit(unit_type unitType, fPoint position, faction_enum faction);

	void DestroyEntity(Entity *Entity);

public:


	std::list<Entity*> entities_list;

	float accumulated_time = 0.0f;
	float update_ms_cycle = 0.0f;
	int times_per_sec = 0;
	bool do_logic = false;

};

#endif