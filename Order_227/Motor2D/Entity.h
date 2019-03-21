#ifndef ENTITY_H
#define ENTITY_H

#include "Point.h"
#include "PugiXml\src\pugixml.hpp"
#include "App.h"

#include <string>

class EntityManager;
struct SDL_Texture;
struct SDL_Rect;

enum class entity_type {

	OBJECT_ENT,
	UNIT_ENT,
	UNKNOWN = 2
};

class Entity {

public:

	Entity(entity_type entityType, iPoint position)  {}
	~Entity() {}

public:

	// Called before render is available
	virtual bool Awake() {}

	// Called before the first frame if it was activated before that
	virtual bool Start() {}

	// Called each loop iteration
	virtual bool FixUpdate(float dt) {}

	// Called each logic iteration
	virtual bool Update(float dt) {}

	// Called before all Updates
	virtual bool PreUpdate() {}

	// Called before all Updates
	virtual bool PostUpdate() {}

	// Called before quitting
	virtual bool CleanUp() {}

	virtual bool DestroyEntity() {}

	entity_type GetType() const { return type; }
	std::string GetName() const { return name; }

public:

	virtual void Move(float dt) {}
	virtual void Draw(float dt) {}

public:

	int life = 0;
	bool active = false;
	bool mustDestroy = false;
	entity_type type;
	std::string name;

	fPoint position;
	SDL_Texture *texture = nullptr;

};

#endif