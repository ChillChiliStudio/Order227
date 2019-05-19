#ifndef  HORDE_MANAGER_H
#define	 HORED_MANAGER_H

#include "Module.h"
#include "Point.h"
#include "vector"
#include "Timer.h"

#define TIME_BETWEEN_ROUNDS 7*1000
#define TIME_TO_CHECK_HORDES 2

class Group;
class Spawning_Point;

class Horde_Manager : public Module
{
public:

	Horde_Manager();

	bool Start();
	bool Awake(pugi::xml_node&config);
	bool Update(float dt);
	bool CleanUp();

public:

	void ClearEnemies();
	void ChooseSpawningPoints();
	bool HordesDead();

	void restartRounds() {
		roundNumber = 0;
		roundThreat = 0;
	}

	int getRoundTimer() {
		int ret = 0;

		ret = roundTimer.ReadSec() - 7;

		ret *=(-1);

		if (ret < 0) {
			ret = 0;
		}

		return ret;
	}

private:

	void CleanHordes();
	void roundTimerStart() { roundTimer.Start(); }

public:

	std::vector<Spawning_Point*> SpawningPoints_Array;
	std::vector<Group*> hordes;

	int roundNumber = 0;
	bool hordeActive = false;
	int maxHordes = 0;

	Timer roundTimer;

private:

	//Spawning & SP
	int roundThreat = 0;
	int threatIncremental = 0;


	Timer CleanHordesTimer;

};

#endif // ! HORDE_MANAGER_H
