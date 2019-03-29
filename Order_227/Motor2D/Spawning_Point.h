#include "Point.h"
#include "Timer.h"
#include "App.h"
#include "EntityManager.h"
#include <vector>
#include "SDL/include/SDL.h"

class Spawning_Point {

public:

	Spawning_Point(iPoint pos = iPoint(0,0), bool active = false) : position(pos), active(active) {}
	~Spawning_Point() {}

	bool active = false;
	iPoint position;
	SDL_Rect SP_Rect = {position.x, position.y, 32, 32};

	void FillEnemies(int threat) {

		int aux_threat = threat;
		while (aux_threat > 0) {

			myApp->entities->CreateUnit(unit_type::INFANTRY_DIVISION, fPoint(position.x, position.y), faction_enum::FACTION_CAPITALIST);
			aux_threat--;
		}
	}
};