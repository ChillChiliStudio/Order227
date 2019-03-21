#include "Point.h"
#include "Timer.h"
#include <vector>
#include "SDL/include/SDL.h"

class Spawning_Point {

public:

	Spawning_Point(iPoint pos = iPoint(0,0), int th = 0) : position(pos), threat(th) {}
	~Spawning_Point() {}

	int threat;
	bool active = false;
	iPoint position;
	SDL_Rect SP_Rect = {position.x, position.y, 32, 32};

	std::vector<SDL_Rect> enemies;

	void FillEnemies() {

		int aux_threat = threat;
		while (aux_threat > 0) {

			SDL_Rect r = { position.x, position.y, 20, 20 };
			enemies.push_back(r);
			aux_threat--;
		}
	}
};