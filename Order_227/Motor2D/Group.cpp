#include <vector>
#include <queue>
#include "App.h"
#include "Log.h"
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
	iPoint mainDestination = myApp->map->WorldToMap(origDest);
	
	if (!myApp->pathfinding->IsWalkable(mainDestination))
	{
		FindClosestWalkable(mainDestination);
	}

	std::vector<iPoint> visited;
	std::queue<iPoint> frontier;

	visited.push_back(mainDestination);
	frontier.push(mainDestination);

	std::list<Unit*>::iterator currentUnit = groupUnits.begin();

	(*currentUnit++)->destination = myApp->map->MapToWorld(mainDestination);

	while (currentUnit != groupUnits.end())
	{
		iPoint tile = frontier.front();
		frontier.pop();

		iPoint neighbors[4];
		neighbors[0].create(tile.x + 1, tile.y + 0);
		neighbors[1].create(tile.x + 0, tile.y + 1);
		neighbors[2].create(tile.x - 1, tile.y + 0);
		neighbors[3].create(tile.x + 0, tile.y - 1);

		for (int i = 0; i < 4; ++i)
		{
			bool tileVisited = false;
			
			for (int b = 0; b < visited.size(); ++b)
			{
				if (visited[b] == neighbors[i]) {
					tileVisited = true;
					break;
				}
					
			}

			if (myApp->pathfinding->IsWalkable(neighbors[i])&&!tileVisited)
			{
				frontier.push(neighbors[i]);
				visited.push_back(neighbors[i]);

				(*currentUnit)->destination = myApp->map->MapToWorld(neighbors[i]);
				currentUnit++;
				if (currentUnit == groupUnits.end())
					break;
			}
		}
	}
}

void Group::TransmitOrders(unit_orders givenOrder)
{
	switch (givenOrder) {
	case unit_orders::HOLD:
		for (std::list<Unit*>::iterator it = groupUnits.begin(); it != groupUnits.end(); it = next(it)) {
			if ((*it)->IsDead() == false) {
				(*it)->StartHold();
			}
		}
		break;
	case unit_orders::MOVE:
		for (std::list<Unit*>::iterator it = groupUnits.begin(); it != groupUnits.end(); it = next(it)) {
			if ((*it)->IsDead() == false) {
				(*it)->StartMove((*it)->destination);
			}
		}
		break;
	//case unit_orders::HUNT:
	//	for (std::list<Unit*>::iterator it = groupUnits.begin(); it != groupUnits.end(); it = next(it)) {
	//		if ((*it)->IsDead() == false) {
	//			(*it)->StartHunt((*it)->destination);
	//		}
	//	}
	//	break;
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

void Group::FindClosestWalkable(iPoint& destination)
{
	std::vector<iPoint> visited;
	std::queue<iPoint> frontier;

	visited.push_back(destination);
	frontier.push(destination);

	while (!myApp->pathfinding->IsWalkable(destination))
	{
		iPoint tile = frontier.front();
		frontier.pop();

		iPoint neighbors[4];
		neighbors[0].create(tile.x + 1, tile.y + 0);
		neighbors[1].create(tile.x + 0, tile.y + 1);
		neighbors[2].create(tile.x - 1, tile.y + 0);
		neighbors[3].create(tile.x + 0, tile.y - 1);

		for (int i = 0; i < 4; ++i)
		{
			bool tileVisited = false;
			
			if (myApp->pathfinding->IsWalkable(neighbors[i]))
			{
				destination = neighbors[i];
				break;
			}

			for (int b =0;b<visited.size();++b)
			{
				if (visited[b] == neighbors[i])
					tileVisited = true;
			}

			if (!tileVisited)
			{
				frontier.push(neighbors[i]);
				visited.push_back(neighbors[i]);
			}
		}
	}
}