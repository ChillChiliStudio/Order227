#ifndef BASE_H
#define BASE_H

#include "Entity.h"

enum class building_type {

	BASE_NONE = -1,
	MAIN_BASE,
	REFINERY,
	BASE_MAX
};

class Building :public Entity
{
public:

	Building(fPoint position, building_type baseType, entity_faction faction = entity_faction::NEUTRAL);
	~Building() {}

	bool Update();
	bool CleanUp();

	bool Draw(float dt);

public:

	//ADD ANIMATION
	//Current_animation*
	building_type baseType;

};
#endif 