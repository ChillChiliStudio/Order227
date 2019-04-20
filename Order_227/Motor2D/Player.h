#ifndef PLAYER_H_
#define PLAYER_H_

#include "Module.h"
#include <vector>
#include <time.h>
#include "Point.h"
#include "SDL/include/SDL.h"

class Group;	//TODO: Remove, this is used because playerGroup is in Group_Manager
enum class infantry_type;
enum class entity_faction;
enum class unit_orders;

class Player : public Module {
public:
	Player();
	~Player();

public:

	bool Awake();
	bool Start() override;
	bool PreUpdate() override;
	bool Update(float dt) override;
	bool CleanUp() override;

	void CameraInputs(float dt);
	void DebugInputs();
	void DebugSpawnUnit(infantry_type type, entity_faction faction);

	void OrderHold();
	void OrderMove();
	void OrderAttack();
	void OrderMoveAndAttack();
	void OrderPatrol();

	void entitiesSelection();
	
public:

	int rectangle_width;
	int rectangle_height;

	iPoint mousePos;
	iPoint mouseScreenPos;

	iPoint mouseClickPos;

	iPoint origin;

	iPoint rectangle_origin;

	Group* playerGroup = nullptr;

	int playerMoney = 300;

};

#endif 
