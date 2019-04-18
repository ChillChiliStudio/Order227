#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Fonts.h"
#include "Render.h"
#include "Window.h"
#include "Map.h"
#include "PathFinding.h"
#include "Entity_Manager.h"
#include "UserInterface.h"
#include "ButtonActions.h"
#include "ParamBox.h"
#include "GroupManager.h"
#include "Player.h"

Player::Player() {
	
}
Player::~Player() {

}

bool Player::Awake() {

	LOG("AWAKING PLAYER MODULE");

	return true;
}

bool Player::Start() {

	LOG("STARTING PLAYER MODULE");

	return true;
}


bool Player::PreUpdate() {

	return true;
}


bool Player::CleanUp() {
	return true;
}

bool Player::Update(float dt) {

	//MOVE CAMERA
	if (myApp->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		myApp->render->camera.y += 500 * dt;

	if (myApp->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		myApp->render->camera.y -= 500 * dt;

	if (myApp->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		myApp->render->camera.x += 500 * dt;

	if (myApp->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		myApp->render->camera.x -= 500 * dt;

	//Mouse selection code bellow

	myApp->input->GetMousePosition(mousePos.x, mousePos.y);
	mouseScreenPos = myApp->render->ScreenToWorld(mousePos.x, mousePos.y);

	//ENTITIES TEST
	if (myApp->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		iPoint position;
		myApp->input->GetMousePosition(position.x, position.y);
		//position = myApp->render->ScreenToWorld(position.x, position.y);
		fPoint position2;
		position2.x = position.x;
		position2.y = position.y;
 		myApp->entities->CreateSoldier(position2, soldier_type::BAZOOKA, entity_faction::URSS);
	}

	//group movement bellow
	SDL_Rect goToRect = { 10,10,20,10 };
	if (myApp->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_REPEAT) {
		//goToRect.x = mouseScreenPos.x;
		//goToRect.y = mouseScreenPos.y;
		//myApp->render->DrawQuad(goToRect, 0, 255, 100, 255, true);
		//myApp->input->GetMousePosition(mouseClickPos.x, mouseClickPos.y);
		////FER WORLD TO MAP  DE MOUSECLICKPOS
		//myApp->groups->CreateDestination(mouseClickPos);
		//myApp->groups->CreateGroupPaths(myApp->groups->playerGroup.groupUnits_list, mouseScreenPos);
	}

	entitiesSelection();

	return true;
}



//void Player::CreateUnitOnPos(iPoint mouseScreenPos_)
//{
//	//RECODE
//	//fPoint position;
//	//position.x = (float)mouseScreenPos_.x - 30;
//	//position.y = (float)mouseScreenPos_.y - 35;
//	//myApp->entities->CreateUnit(unit_type::INFANTRY, position, faction_enum::COMMUNIST);
//}

void Player::entitiesSelection()
{
	SDL_Rect SRect;
	rectangle_width = mouseScreenPos.x - rectangle_origin.x;
	rectangle_height = mouseScreenPos.y - rectangle_origin.y;

	if (myApp->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
		rectangle_origin = mouseScreenPos;

	else if (std::abs(rectangle_width) >= 5 && std::abs(rectangle_height) >= 5 && myApp->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
		
		// --- Rectangle size ---
		int width = mouseScreenPos.x - rectangle_origin.x;
		int height = mouseScreenPos.y - rectangle_origin.y;

		// --- Draw Rectangle ---
		SRect = { rectangle_origin.x, rectangle_origin.y, width, height };
		myApp->render->DrawQuad(SRect, 0, 200, 100, 255, false);

		// --- Once we get to the negative side of SRect numbers must be adjusted ---
		if (width < 0) {
			SRect.x = mouseScreenPos.x;
			SRect.w *= -1;
		}
		if (height < 0) {
			SRect.y = mouseScreenPos.y;
			SRect.h *= -1;
		}
		//units selection
		myApp->groups->SelectUnit(SRect);
	}

	// 1 click selection
	if (myApp->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
		myApp->groups->SelectUnit(mouseScreenPos);
	}

	//groups management
	if (myApp->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
		myApp->groups->CreateGroupForPlayer();
	}
}

