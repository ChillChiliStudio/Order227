#ifndef  HORDE_MANAGER_H
#define	 HORED_MANAGER_H

#include "Module.h"
#include "Point.h"
#include "vector"
#include "Timer.h"

#define TIME_BETWEEN_ROUNDS 5*1000

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

	std::vector<Spawning_Point*> SpawningPoints_Array;
	std::vector<Group*> hordes;

private:

	//Spawning & SP
	int roundThreat = 0;
	int threatIncremental = 0;
	int roundNumber = 0;

	Timer roundTimer;

};

#endif // ! HORDE_MANAGER_H

