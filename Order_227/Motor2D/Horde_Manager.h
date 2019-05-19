#ifndef  HORDE_MANAGER_H
#define	 HORED_MANAGER_H

#include "Module.h"
#include "Point.h"
#include "vector"
#include "Timer.h"

#define TIME_BETWEEN_ROUNDS 4*1000

class Group;
class Spawning_Point;

class Horde_Manager : public Module
{
public:

	Horde_Manager();

	bool Start();
	bool Update(float dt);
	bool CleanUp();

public:

	void ChooseSpawningPoints();
	void ClearEnemies();
	bool HordesDead();
	void roundTimerStart() {
		roundTimer.Start();
	}

	void hordeRoundto(int round) {
		roundThreat = round;
		roundNumber = round;
	}

	int getRoundTimer() {
		int ret = 0;

		ret = roundTimer.ReadSec() - 4;

		ret *=(-1);

		return ret;
	}

	std::vector<Spawning_Point*> SpawningPoints_Array;
	std::vector<Group*> hordes;

	bool hordeActive = false;
	int roundNumber = 0;

private:

	//Spawning & SP
	int currentHorde;
	int roundThreat = 0;
	int threatIncremental = 0;


	Timer roundTimer;

};

#endif // ! HORDE_MANAGER_H

