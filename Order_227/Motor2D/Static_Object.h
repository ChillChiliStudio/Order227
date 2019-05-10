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

	//NOTE from Carles: After several hours of suffering in an attempt to create a new class from entity that had this stuff, I gave up and this is the result
	virtual float Hurt(float damage) { return 0.0f; };
	virtual bool IsDead() { return false; };
	virtual bool IsVisible() { return true; };

	object_type objectType;
};

#endif 