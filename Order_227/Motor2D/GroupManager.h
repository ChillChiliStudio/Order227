#ifndef GROUPMANAGER_H_
#define GROUPMANAGER_H_


#include "Module.h"
#include "Entity.h"
#include "Unit.h"
#include "Group.h"
#include "PugiXml\src\pugixml.hpp"

class GroupManager : public Module {

public:

	GroupManager() ;
	~GroupManager() ;

public:

	bool Awake();
	bool Start() override;
	bool PreUpdate() override;
	bool Update(float dt) override;
	bool CleanUp() override;

	//Functionalities for the player's groups
	void EmptyPlayerGroup();
	void CreateGroupForPlayer();
	void AddUnitsPlayerGroup();
	void SelectUnit(SDL_Rect rect);
	void SelectUnit(iPoint position);


	//Functionalities for all the groups
	void CreateGroup();
	void CreateDestination(iPoint mouseScreenPos, Group group);
	void CreateGroupPaths(std::list<Unit*> list,iPoint destination);
	void MoveUnitsGroup();

public:

	Group playerGroup;

};


#endif