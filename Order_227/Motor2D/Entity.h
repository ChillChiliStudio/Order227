#ifndef ENTITY_H
#define ENTITY_H

#include "Point.h"
#include "Vector.h"
#include "PugiXml\src\pugixml.hpp"
#include "App.h"

#include <string>

class EntityManager;
class ImageRender;
struct SDL_Texture;
struct SDL_Rect;

enum class entity_type {

	ENT_NONE = -1,
	OBJECT,
	BASE,
	SOLDIER,
	ENT_MAX
};

enum class entity_faction {

	FACTION_NONE = -1,
	URSS,
	US,
	NEUTRAL,
	FACTION_MAX

};

struct EntityData {

	std::string Action;
	iPoint TilePos;
	iPoint TileSize;
	int Degrees;
	int AnimFrames;

};

class Entity {

public:

	Entity(fPoint position, entity_type entityType, entity_faction faction = entity_faction::NEUTRAL) : type(entityType), position(position), faction(faction) {}
	~Entity() {}

public:

	// Called before the first frame if it was activated before that
	virtual bool Start() { return true; }

	// Called each loop iteration
	virtual bool FixUpdate(float dt) { return true; }

	// Called each logic iteration
	virtual bool Update(float dt) { return true; }

	// Called before quitting
	virtual bool CleanUp() { return true; }


public:

	virtual bool Draw(float dt) { return true; }
	virtual void UpdateBlitOrder() {}
	virtual bool LoadEntityData()       { return true; }


	virtual bool DebugDraw() { return true; }

public:

	//True if the entity is "alive", false if it's not
	bool active = false;
	bool selected = false;

	//Entity data
	fPoint			position;
	SDL_Texture*	texture = nullptr;
	entity_faction	faction;
	entity_type		type;

	//IMLPEMENT ANIMATION
	//SDL_Rect		blitRect;

	uint order = 0;

};

#endif
