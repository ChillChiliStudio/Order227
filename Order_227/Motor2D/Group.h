#ifndef GROUP_H_
#define GROUP_H_

#include <list>
#include "Point.h"

#include "Map.h"	//TODO-Carles: I tried to fix it by all f*cking means, but if Map is included in the .cpp instead of the .h, the code explodes. This should not be included here, avoid includes on headers.

enum class unit_orders;
class Unit;

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
	Group(std::list<Unit*> units_list);
	~Group();

public:

	void SpreadDestinations(iPoint origDest);
	void TransmitOrders(unit_orders givenOrder);

	group_type groupType = group_type::NONE;

	/*std::queue<iPoint*> destinations_queue;*/
	std::list<Unit*> groupUnits;
	
};

#endif
