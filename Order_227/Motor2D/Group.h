#ifndef GROUP_H_
#define GROUP_H_

#include "Entity.h"
#include "SDL/include/SDL.h"
#include "Unit.h"

enum class group_type {

	NONE =-1,
	GROUP_STATIC,
	GROUP_DYNAMIC
};


//Groups are maded of units, other type of entities will need their own group or to modificate thsi group class
//Group constructor needs a list of units
class Group {

public:

	Group();
	Group(std::list<Unit*> units_list)  {
		groupUnits_list = units_list;
	};
	~Group() {
		groupUnits_list.clear();
	};

public:

	group_type groupType=group_type::NONE;
	int size=0;

public:

	/*std::queue<iPoint*> destinations_queue;*/
	std::list<Unit*> groupUnits_list;
	
};

#endif
