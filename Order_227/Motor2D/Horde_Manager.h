#ifndef  HORDE_MANAGER_H
#define	 HORED_MANAGER_H

#include "Module.h"
#include "Point.h"
#include "vector"

class Group;
class Spawning_Point;

struct Horde {
	Group* units;
	iPoint objective;
};

class Horde_Manager : public Module
{
public:

	Horde_Manager();

	bool Start();
	bool Update(float dt);
	bool CleanUp();

public:

	

	void ChooseSpawningPoints();

	std::vector<Spawning_Point*> SpawningPoints_Array;
	std::vector<Group*> hordes;

private:

	//Spawning & SP
	int roundThreat = 0;
	int threatIncremental = 0;
	int roundNumber = 0;


};

#endif // ! HORDE_MANAGER_H

