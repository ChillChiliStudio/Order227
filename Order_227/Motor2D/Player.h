#ifndef PLAYER_H
#define PLAYER_H

#include "Module.h"
#include "Point.h"

//#include <vector>
//#include <ctime>
//#include "Point.h"
//#include "SDL/include/SDL.h

#define CAMERA_SPEED 500
#define SCREEN_MOVEMENT_MARGIN 200

enum class infantry_type;
enum class entity_faction;
enum class unit_orders;

class Text;

class Player : public Module {

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

	void CheckForOrders();
	void ApplyOrders();
	void OrderHold();
	void OrderMove();
	void OrderHunt();
	void OrderPatrol();

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

	Text* mouseDebugMark = nullptr;
	iPoint origin;
	iPoint rectangle_origin;
	
	unit_orders prepOrder = (unit_orders)-1;

	//Group playerGroup = nullptr;	//TODO: On group manager, should probably be here

	int playerMoney = 400;
	bool startCreationUnit = false;

};

#endif	//PLAYER_H
