#ifndef ENTITY_MANAGER_
#define ENTITY_MANAGER_

#include "Module.h"
#include "Entity.h"
#include "Unit.h"
#include "Main_Base.h"
#include "PugiXml\src\pugixml.hpp"


#define TIMES_PER_SEC 5

class Entity_Manager : public Module
{
public:

	Entity_Manager();
	~Entity_Manager();

public:

	bool Awake();
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool CleanUp();

public:

	/*Entity *CreateEntity(entity_type entityType, fPoint position);
	Entity *CreateUnit(unit_type unitType, fPoint position, faction_enum faction);
*/
	/*void DestroyEntity(Entity *Entity);*/
	


public:

	SDL_Texture*	texture_list[int(entity_type::ENT_MAX)];
	unit_stats		units_stats[int(entity_type::ENT_MAX)];

	//std::list<Entity*> entities_list;


private:

	float accumulated_time = 0.0f;
	float update_ms_cycle = 0.0f;
	int times_per_sec = 0;
	bool do_logic = false;

};

#endif