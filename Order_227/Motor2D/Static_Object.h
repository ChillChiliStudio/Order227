#ifndef STATIC_OBJECT_H
#define STATIC_OBJECT_H

#include "Entity.h"

enum class object_type {

	OBJECT_NONE = -1,
	TREE,
	ROCK,
	OBJECT_MAX
};

enum class Trees {


	NONE = -1,
	DEFAULT,
	FOREST_ONE,
	FOREST_TWO,
	PINE,
	DESERT


};

class Static_Object : public Entity
{
public:

	Static_Object();
	Static_Object(fPoint pos, object_type objectType = object_type::OBJECT_NONE, entity_faction faction = entity_faction::NEUTRAL);
	~Static_Object() {}

	bool Update(float dt);
	bool CleanUp();
	bool Draw();
	void UpdateBlitOrder() override;

	object_type objectType;
};

#endif 