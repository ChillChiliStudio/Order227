#ifndef STATIC_OBJECT_H
#define STATIC_OBJECT_H

#include "Entity.h"

enum class object_type {

	OBJECT_NONE = -1,
	TREE,
	ROCK,
	OBJECT_MAX
};

class Static_Object : public Entity
{
public:

	Static_Object(fPoint position, object_type objectType, entity_faction faction = entity_faction::NEUTRAL);
	~Static_Object() {}

	bool Update();
	bool CleanUp();

	bool Draw(float dt);

	object_type objectType;
};

#endif 
