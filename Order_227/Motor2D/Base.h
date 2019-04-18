#ifndef BASE_H
#define BASE_H

#include "Entity.h"

enum class base_type {
	BASE_NONE = -1,
	MAIN_BASE,
	REFINERY,
	BASE_MAX
};

class Base :public Entity
{
public:

	Base(fPoint position, base_type baseType, entity_faction faction = entity_faction::NEUTRAL);
	~Base() {}

	bool Update();
	bool CleanUp();

	bool Draw(float dt);

public:

	//ADD ANIMATION
	//Current_animation*
	base_type baseType;

};
#endif 