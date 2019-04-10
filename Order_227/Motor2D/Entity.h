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

	OBJECT_ENT,	//TODO: It's an enum class, it will require entity_type:: to be used so the "_ENT" is unnecesary.
	UNIT_ENT,	//TODO: Needs to be more specific, I need to know if a unit is hostile or friendly
	UNKNOWN = 2
};

class Entity {

public:


	Entity(entity_type entityType, fPoint position) : type(entityType), position(position) {}

	~Entity() {}

public:

	// Called before render is available
	virtual bool Awake()				{ return true; }

	// Called before the first frame if it was activated before that
	virtual bool Start()				{ return true; }

	// Called each loop iteration
	virtual bool FixUpdate(float dt)	{ return true; }

	// Called each logic iteration
	virtual bool Update(float dt)		{ return true; }

	// Called before all Updates
	virtual bool PreUpdate()			{ return true; }

	// Called before all Updates
	virtual bool PostUpdate()			{ return true; }

	// Called before quitting
	virtual bool CleanUp()				{ return true; }

	virtual void DestroyEntity() {}

	entity_type GetType() const			{ return type; }
	std::string GetName() const			{ return name; }

public:


	virtual bool Move(float dt)			{ return true; }
	virtual bool Draw(float dt)			{ return true; }
	virtual void UpdateBlitOrder() {}
	virtual bool LoadEntityData()       { return true; }


public:

	int life = 0;
	bool active = false;
	bool mustDestroy = false;


	uint order = 0;

	entity_type type;
	std::string name;

	fPoint position;
	SDL_Texture *texture = nullptr;
	
	//TilesetFile
	pugi::xml_document	tilsetTexture;

};

#endif