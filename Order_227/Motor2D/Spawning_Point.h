#include "Point.h"
#include "App.h"
#include "Entity_Manager.h"
#include <vector>
#include "SDL/include/SDL.h"

class Spawning_Point {

public:

	Spawning_Point(iPoint pos = iPoint(0,0), bool active = false) : position(pos), active(active) {}
	~Spawning_Point() {}

	bool active = false;
	iPoint position = { 0,0 };
	SDL_Rect SP_Rect = {position.x, position.y, 32, 32};
	Timer SpawnTime;
	bool enemiesAttacking = false;

	std::vector<int>Enemies_to_Spawn;
	
	void FillEnemies(int threat) {

		int aux_threat = threat;

		int enemyType = rand() % 7;
		if (enemyType == 1 || enemyType == 3)
			enemyType = 0;

		while (aux_threat > 0) {

			Enemies_to_Spawn.push_back(enemyType);
			aux_threat -= myApp->entities->infantryStats[enemyType].unitThreat;
		}

		SpawnTime.Start();
	}
};