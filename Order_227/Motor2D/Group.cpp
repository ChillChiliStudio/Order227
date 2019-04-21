#include <vector>
#include <queue>
#include "App.h"
#include "Log.h"
//#include "Map.h"	//Check Group.h
#include "Pathfinding.h"
#include "Unit.h"
#include "Group.h"

// Constructors
Group::Group()
{}

Group::Group(std::list<Unit*> units_list)
{
	groupUnits = units_list;
};

// Destructor
Group::~Group()
{
	groupUnits.clear();
};

void Group::SpreadDestinations(iPoint origDest)
{
	origDest = myApp->map->WorldToMap(origDest);

	if (!myApp->pathfinding->IsWalkable(origDest))
		return;

	std::vector<iPoint*> visited;
	std::queue<iPoint*> frontier;

	visited.push_back(&origDest);
	frontier.push(&origDest);

	std::list<Unit*>::iterator currentUnit = groupUnits.begin();

	(*currentUnit++)->destination = myApp->map->MapToWorld(origDest);
	LOG("%d %d", origDest.x, origDest.y);

	while (currentUnit != groupUnits.end())
	{
		iPoint* tile = frontier.front();
		frontier.pop();

		iPoint neighbors[4];
		neighbors[0].create(tile->x + 1, tile->y + 0);
		neighbors[1].create(tile->x + 0, tile->y + 1);
		neighbors[2].create(tile->x - 1, tile->y + 0);
		neighbors[3].create(tile->x + 0, tile->y - 1);

		for (int i = 0; i < 4; ++i)
		{
			if (myApp->pathfinding->IsWalkable(neighbors[i]))
			{
				for (std::vector<iPoint*>::iterator ii = visited.begin(); ii != visited.end(); ++ii)
				{
					if ((*ii)->x == neighbors[i].x && (*ii)->y == neighbors[i].y)
						break;
				}
				frontier.push(&neighbors[i]);
				visited.push_back(&neighbors[i]);

				(*currentUnit)->destination = myApp->map->MapToWorld(neighbors[i]);
				currentUnit++;
				LOG("%d %d", neighbors[i].x, neighbors[i].y);
				if (currentUnit == groupUnits.end())
					break;
			}
		}
	}
}

void Group::TransmitOrders(unit_orders givenOrder)
{
	switch (givenOrder) {
	case unit_orders::MOVE:
		for (std::list<Unit*>::iterator it = groupUnits.begin(); it != groupUnits.end(); it = next(it)) {
			if ((*it)->IsDead() == false) {
				(*it)->StartMove((*it)->destination);
			}
		}
		break;
	case unit_orders::MOVE_AND_ATTACK:
		for (std::list<Unit*>::iterator it = groupUnits.begin(); it != groupUnits.end(); it = next(it)) {
			if ((*it)->IsDead() == false) {
				(*it)->StartMoveAndAttack((*it)->destination);
			}
		}
		break;
	case unit_orders::PATROL:
		for (std::list<Unit*>::iterator it = groupUnits.begin(); it != groupUnits.end(); it = next(it)) {
			if ((*it)->IsDead() == false) {
				(*it)->StartPatrol((*it)->destination);
			}
		}
		break;
	}
}

void Group::AddUnit(Unit* unit)
{
	groupUnits.push_back(unit);
}