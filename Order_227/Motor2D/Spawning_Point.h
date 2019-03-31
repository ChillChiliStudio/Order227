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
	Timer SpawnTime;

	std::vector<int>Enemies_to_Spawn;

	void FillEnemies(int threat) {

		int aux_threat = threat;
		while (aux_threat > 0) {

			Enemies_to_Spawn.push_back(1);
			aux_threat--;
		}

		SpawnTime.Start();
	}
};