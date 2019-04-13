#ifndef GROUPMANAGER_H_
#define GROUPMANAGER_H_


#include "Module.h"
#include "Entity.h"
#include "Unit.h"
#include "Group.h"
#include "Main_Base.h"
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


	void SelectUnit(SDL_Rect rect);
	void SelectUnit(iPoint position);
	void CreateGroup();
	void CreateGroupForPlayer();
	void EmptyPlayerGroup();
	void AddUnitsPlayerGroup();
	void MoveUnits(std::list<Unit*> list, iPoint origin, iPoint destination);
	void MoveUnitsGroup();

public:

	Group playerGroup;
};










#endif