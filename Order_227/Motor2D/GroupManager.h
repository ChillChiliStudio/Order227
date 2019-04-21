#ifndef GROUP_MANAGER_H
#define GROUP_MANAGER_H

#include "Module.h"
#include "Group.h"

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
	//void CreateGroup();
	//void MoveUnitsGroup();

	//void CreateGroupPaths(std::list<Unit*> list,iPoint destination);	//TODO: Not needed, probably

public:

	Group playerGroup;
	//std::list<Group*> groupList;
};


#endif	// GROUP_MANAGER_H