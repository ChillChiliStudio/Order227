#ifndef  HORDE_MANAGER_H
#define	 HORED_MANAGER_H

#include "Module.h"
#include "Point.h"

class Group;

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

private:

	std::list<Group*> hordes;

};

#endif // ! HORDE_MANAGER_H

