#ifndef GROUPMANAGER_H_
#define GROUPMANAGER_H_


#include "Module.h"
#include "Entity.h"
#include "Unit.h"
#include "Group.h"
#include "Main_Base.h"
#include "PugiXml\src\pugixml.hpp"


enum class movement_State
{
	MovementState_NoState,
	MovementState_Wait,
	MovementState_FollowPath,
	MovementState_NextStep,
	MovementState_DestinationReached
};

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
	
	void CreateGroupForPlayer();
	void EmptyPlayerGroup();
	void AddUnitsPlayerGroup();
	void CreateDestination(iPoint mouseScreenPos);
	void CreateGroupPaths(std::list<Unit*> list,iPoint destination);
	void MoveUnitsGroup();

	//codi aitor
	void Move( float dt);
	void SetUnitGoalTile(Unit* entity);
	bool FindFreeAdjacents(iPoint * base_tile);

	bool stop_iteration = false;
public:

	Group playerGroup;


};










#endif