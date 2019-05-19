#ifndef PLAYER_H
#define PLAYER_H

#include "Module.h"
#include "Timer.h"
#include "Point.h"

//#include <vector>
//#include <ctime>
//#include "Point.h"
//#include "SDL/include/SDL.h

#define CAMERA_SPEED 700
#define SCREEN_MOVEMENT_MARGIN 20

enum class infantry_type;
enum class entity_faction;
enum class unit_orders;
enum class unit_aggro;

class Text;

class Player : public Module {

public:

	bool Awake(pugi::xml_node&) override;
	bool Start() override;
	bool PreUpdate() override;
	bool Update(float dt) override;
	bool PostUpdate() override;
	bool CleanUp() override;

	void UpdateMousePos();
	void CameraInputs(float dt);
	void DebugMouse();
	void DebugInputs();
	void DebugSpawnUnit(infantry_type type, entity_faction faction);

	void CheckForOrders();
	unit_aggro GetAggroLevel();
	void ApplyAggroLevel(unit_aggro aggro);
	void ApplyOrders();
	void OrderHold();
	void OrderMove();
	void OrderHunt();
	void OrderPatrol();

	void PlayerSelect();
	void StartSelect();
	void ExpandSelect();
	void FinishSelect();
	void UpdateText();
	void MoveText();

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

	Text* IncomeShow = nullptr;
	Text* mouseDebugMark = nullptr;
	iPoint origin;
	iPoint rectangle_origin;
	
	unit_orders prepOrder = (unit_orders)-1;

	//Group playerGroup = nullptr;	//TODO: On group manager, should probably be here

	int playerIncome = 0;
	int playerMoney = 400;
	Timer incomeTimer;
	
	bool IncomeGiven = false;
	bool startCreationUnit = false;

};

#endif	//PLAYER_H
