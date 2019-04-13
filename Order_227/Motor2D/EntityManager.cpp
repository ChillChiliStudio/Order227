//@lucho1: Be extremely careful with shitty STL, to iterate a list,
//if you start it with list.begin() and then check if *item is nullptr,
//it will not "detect" that *item is nullptr and will enter wherever wants to enter
//with an empty list.
//Just check, before creating the iterator, that list.size() > 0

//Also, note that I think there is some memory leak in this module, but
//I couldn't track them, I don't exactly know why the fuck there is a memLeak
//if I load NOTHING 

#include "EntityManager.h"
#include "Log.h"
#include "Entity.h"
#include "Scene.h"
#include "Render.h"
#include "App.h"
#include <assert.h>
#include "Input.h"
#include "Pathfinding.h"

Group::Group(){}


EntityManager::EntityManager()
{
	name.assign("entities");
}


EntityManager::~EntityManager()
{
}


bool EntityManager::Awake() {

	LOG("AWAKING ENTITY MANAGER");
	times_per_sec = TIMES_PER_SEC;
	update_ms_cycle = 1.0f / (float)times_per_sec;

	return true;
}


bool EntityManager::Start() {

	if (entities_list.size() > 0) {

		std::list<Entity*>::iterator item = entities_list.begin();
		for (; item != entities_list.end(); item = next(item))
			(*item)->Start();

	}
	return true;
}


bool EntityManager::PreUpdate() {

	do_logic = false;
	return true;
}


bool EntityManager::Update(float dt) {

	accumulated_time += dt;

	if (accumulated_time >= update_ms_cycle)
		do_logic = true;

	if (do_logic == true) {

		if (entities_list.size() > 0) {

			std::list<Entity*>::iterator item = entities_list.begin();
			for (; item != entities_list.end(); item = next(item))
				(*item)->FixUpdate(dt);
		}
	}

	accumulated_time -= update_ms_cycle;

	if (entities_list.size() > 0) {

		std::list<Entity*>::iterator item = entities_list.begin();
		for (; item != entities_list.end(); item = next(item))
			(*item)->Update(dt);

	}

	//if (entities_list.size() > 0)
	myApp->render->OrderBlit(myApp->render->OrderToRender);


	return true;
}


bool EntityManager::CleanUp() {

	LOG("Clean Up Entity Manager");

	if (entities_list.size() > 0) {

		std::list<Entity*>::iterator item = entities_list.begin();
		while (item != entities_list.end()) {

			RELEASE(*item);
			item = next(item);
		}

		entities_list.clear();
	}

	return true;
}


Entity *EntityManager::CreateEntity(entity_type entityType, fPoint position) {

	static_assert(entity_type::UNKNOWN == entity_type(2), "UPDATE ENTITY TYPES");
	//assert(entityType == entity_type::UNIT_ENT, "UNITS ARE NOT CREATED WITH CreateEntity()! TRY CreateUnit() INSTEAD!");

	Entity* Entity = nullptr;

	switch (entityType) {


	case entity_type::OBJECT:
		Entity = new Main_Base(position);
		break;
	case entity_type::UNIT:
		break;
	case entity_type::UNKNOWN:
		break;

	default:
		break;
	}

	entities_list.push_back(Entity);
	return Entity;
}


Entity *EntityManager::CreateUnit(unit_type unitType, fPoint position, faction_enum faction) {

	Unit* Unit_Ent = nullptr;

	switch (unitType) {

	case unit_type::INFANTRY:
		Unit_Ent = new Unit(unitType, position, faction);
		break;
	case unit_type::UNKNOWN:
		break;
	default:
		break;
	}

	entities_list.push_back(Unit_Ent); //TODO : Revisar esto cuando joan añada las pools

	if (Unit_Ent->unitFaction == faction_enum::COMMUNIST) {
		comunist_list.push_back(Unit_Ent);
	}
	if (Unit_Ent->unitFaction == faction_enum::CAPITALIST) {
		capitalist_list.push_back(Unit_Ent);
	}
	return Unit_Ent;
}


void EntityManager::DestroyEntity(Entity *object) {

	std::list<Entity*>::iterator item = entities_list.begin();
	while (item != entities_list.end()) {

		if ((*item) == object) {

			(*item)->DestroyEntity();
			entities_list.erase(item);
			break;
		}

		item = next(item);
	}
}

