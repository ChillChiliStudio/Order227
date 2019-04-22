#ifndef PLAYER_H_
#define PLAYER_H_

#include "Module.h"
#include "Point.h"

//#include <vector>
//#include <ctime>
//#include "Point.h"
//#include "SDL/include/SDL.h


enum class infantry_type;
enum class entity_faction;
enum class unit_orders;

class Text;

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

	void UpdateMousePos();
	void CameraInputs(float dt);
	void DebugMouse();
	void DebugInputs();
	void DebugSpawnUnit(infantry_type type, entity_faction faction);

	void DebugOrders();
	void DebugOrderHold();
	void DebugOrderMove();
	void DebugOrderAttack();
	void DebugOrderMoveAndAttack();
	void DebugOrderPatrol();

	void PlayerSelect();
	void StartSelect();
	void ExpandSelect();
	void FinishSelect();
	
public:

	/*int rectangle_width;
	int rectangle_height;
	iPoint rectangle_origin;

	iPoint mouseScreenPos;
	iPoint mousePos;*/

	int rectangle_width;
	int rectangle_height;

	iPoint mousePos;
	iPoint mouseScreenPos;
	iPoint mouseMap;

	Text* mouseDebugMark;

	iPoint origin;

	iPoint rectangle_origin;

	//Group playerGroup = nullptr;	//TODO: On group manager, should probably be here
	
	int playerMoney = 300;
	bool startCreationUnit = false;

};

#endif 
