#ifndef ENTITY_H
#define ENTITY_H

#include "Point.h"
#include "Vector.h"
#include "PugiXml\src\pugixml.hpp"
#include "App.h"
#include "SDL/include/SDL_rect.h"
#include <string>

class EntityManager;
class ImageRender;
struct SDL_Texture;
struct SDL_Rect;

enum class entity_type {

	ENT_NONE = -1,
	OBJECT,
	BUILDING,
	INFANTRY,
	ENT_MAX
};

enum class entity_faction {

	FACTION_NONE = -1,
	COMMUNIST,
	CAPITALIST,
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

	Entity() {};
	Entity(fPoint position, entity_type entityType, entity_faction faction = entity_faction::NEUTRAL)
		: type(entityType), position(position), entityRect({ (int)position.x, (int)position.y, 0, 0 }), faction(faction)
	{}

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
	virtual bool LoadEntityData()       { return true; }

	virtual bool DebugDraw() { return true; }
	
	//TODO-Carles: Tried to make a new class "between" entity and Unit/Building and the bugs that popped out from that mess made me change my mind, fuck that so hard, so I put shit here
	virtual float Hurt(float damage) = 0;
	virtual bool IsDead() = 0;
	virtual bool IsVisible() = 0;

public:

	//Marks if entity exists in world
	bool active = false;

	//Marks if entity is selected by the player and already stored in a list	//TODO: A tree can be selected by the player? A bullet? This should be in Dynamic Class, not here
	bool selected = false;
	bool stored = false;

	//Entity data
	fPoint			position;	//World Position (should be worldPos)
	//fPoint		centerPos;	//Rect center world position

	entity_faction	faction;
	entity_type		type;

	SDL_Rect		entityRect;			// Entity SDL_Rect for world pos
	SDL_Rect		spriteRect;			// Entity SDL_Rect for texture area cutting
	SDL_Texture*	texture = nullptr;	// Entity Texture

	int order = 0;

};

#endif
