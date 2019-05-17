#ifndef  HORDE_MANAGER_H
#define	 HORED_MANAGER_H

#include "Module.h"
#include "Point.h"
#include "vector"
#include "Timer.h"

#define TIME_BETWEEN_ROUNDS 4*1000
#define TIME_TO_CHECK_HORDES 2

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

	void ClearEnemies();
	void ChooseSpawningPoints();

	void restartRounds() {
		roundNumber = 0;
		roundThreat = 0;
	}

	int getRoundTimer() {

		int i = roundTimer.ReadSec();
		switch (i)
		{
		case 0:
			i = 3;
			break;
		case 1:
			i = 2;
			break;
		case 2:
			i = 1;
			break;
		case 3:
			i = 0;
			break;
		default:
			break;
		}
		return i ;
	}

private:

	void CleanHordes();
	bool HordesDead();

	void roundTimerStart() { roundTimer.Start(); }

public:

	std::vector<Spawning_Point*> SpawningPoints_Array;
	std::vector<Group*> hordes;

	int roundNumber = 0;
	bool hordeActive = false;

	Timer roundTimer;

private:

	//Spawning & SP
	int roundThreat = 0;
	int threatIncremental = 0;


	Timer CleanHordesTimer;

};

#endif // ! HORDE_MANAGER_H

