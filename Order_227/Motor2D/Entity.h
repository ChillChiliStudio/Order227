#ifndef ENTITY_H
#define ENTITY_H

#include "Point.h"
#include "PugiXml\src\pugixml.hpp"
#include "App.h"

#include <string>

class EntityManager;
class ImageRender;
struct SDL_Texture;
struct SDL_Rect;

enum class entity_type {

	ENT_NONE=-1,
	ENT_OBJECT,		
	ENT_BASE,		
	ENT_SOLDIER,	
	ENT_MAX
};

enum class entity_faction {

	FACTION_NONE = -1,
	FACTION_URSS,
	FACTION_US,
	FACTION_NEUTRAL,
	FACTION_MAX

};

class Entity {
	friend Entity_Manager;
public:


	Entity(fPoint position,entity_type entityType,entity_faction faction = entity_faction::FACTION_NEUTRAL) : type(entityType), position(position), faction(faction) {}

	~Entity() {}

public:

	//// Called before render is available
	//virtual bool Awake()				{ return true; }

	// Called before the first frame if it was activated before that
	virtual bool Start()				{ return true; }

	// Called each loop iteration
	virtual bool FixUpdate(float dt)	{ return true; }

	// Called each logic iteration
	virtual bool Update(float dt)		{ return true; }

	//// Called before all Updates
	//virtual bool PreUpdate()			{ return true; }

	//// Called after all Updates
	//virtual bool PostUpdate()			{ return true; }

	// Called before quitting
	virtual bool CleanUp()				{ return true; }


public:
	
	virtual bool Draw(float dt)			{ return true; }
	virtual void UpdateBlitOrder() {}

	virtual bool DebugDraw()			{ return true; }

public:

	//True if the entity is "alive", false if it's not
	bool active = false;

	//Entity data
	fPoint			position;
	SDL_Texture*	texture=nullptr;
	entity_faction	faction;
	entity_type		type;

	//IMLPEMENT ANIMATION
	//SDL_Rect		blitRect;

	uint order = 0;

};

#endif