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

Player::Player()
{}

Player::~Player()
{}

bool Player::Awake()
{
	LOG("AWAKING PLAYER MODULE");

	return true;
}

bool Player::Start()
{
	LOG("STARTING PLAYER MODULE");

	playerGroup = &myApp->groups->playerGroup;	// TODO: This should be in player, not in GroupManager, this is a workaround

	return true;
}

bool Player::PreUpdate()
{
	return true;
}

bool Player::Update(float dt)
{
	CameraInputs(dt);
	DebugInputs();

	////Map drawing debug mode
	//if (myApp->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
	//	myApp->map->debugMode = !myApp->map->debugMode;

	//Mouse selection code bellow
	myApp->input->GetMousePosition(mousePos.x, mousePos.y);
	mouseScreenPos = myApp->render->ScreenToWorld(mousePos.x, mousePos.y);

	//group movement bellow
	SDL_Rect goToRect = { 10,10,20,10 };
	if (myApp->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN) {
		goToRect.x = mouseScreenPos.x;
		goToRect.y = mouseScreenPos.y;
		
		myApp->input->GetMousePosition(mouseClickPos.x, mouseClickPos.y);
		//FER WORLD TO MAP  DE MOUSECLICKPOS
		myApp->groups->CreateDestination(myApp->map->WorldToMap(mouseClickPos), myApp->groups->playerGroup);
		myApp->groups->CreateGroupPaths(myApp->groups->playerGroup.groupUnits_list, mouseScreenPos);
	}

	entitiesSelection();

	return true;
}

bool Player::CleanUp()
{
	return true;
}

void Player::CameraInputs(float dt)
{
	//Move Camera
	if (myApp->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		myApp->render->camera.y += ceil(500 * dt);

	if (myApp->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		myApp->render->camera.y -= ceil(500 * dt);

	if (myApp->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		myApp->render->camera.x += ceil(500 * dt);

	if (myApp->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		myApp->render->camera.x -= ceil(500 * dt);
}

void Player::DebugInputs()
{
	//Toggle DebugMode
	if (myApp->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN) {
		myApp->debugMode = !myApp->debugMode;

		if (myApp->debugMode == false) {
			myApp->map->mapDebugDraw = false;
			myApp->gui->interfaceDebugDraw = false;
			//myApp->entities->entitiesDebugDraw = false;
			LOG("Debug Mode: OFF");
		}
		else {
			LOG("Debug Mode: ON");
		}
	}

	if (myApp->debugMode) {
		if (myApp->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {	// Toggle Map debug draw
			myApp->map->mapDebugDraw = !myApp->map->mapDebugDraw;
		}

		if (myApp->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) {	// Toggle UI debug draw
			myApp->gui->interfaceDebugDraw = !myApp->gui->interfaceDebugDraw;
		}

		if (myApp->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN) {	// Toggle Entities debug draw
			//myApp->entities->entitiesDebugDraw = !myApp->entities->entitiesDebugDraw;
		}

		if (myApp->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN) {	// Insta-Win


		}

		if (myApp->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) {	// Insta-Lose

		}

		if (myApp->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) {	// Instantaneous Next Round + Kill all active enemies

		}

		if (myApp->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN) {	// Spawn Capitalist Unit on Mouse
			DebugSpawnUnit(infantry_type::BASIC, entity_faction::CAPITALIST);
		}

		if (myApp->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) {	// Spawn Communist Unit on Mouse
			DebugSpawnUnit(infantry_type::BASIC, entity_faction::COMMUNIST);
		}

		if (myApp->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN) {
			if (myApp->input->GetKey(SDL_SCANCODE_1) == KEY_REPEAT) {
				OrderHold();
			}
			else if (myApp->input->GetKey(SDL_SCANCODE_2) == KEY_REPEAT) {
				OrderMove();
			}
			else if (myApp->input->GetKey(SDL_SCANCODE_3) == KEY_REPEAT) {
				OrderAttack();
			}
			else if (myApp->input->GetKey(SDL_SCANCODE_4) == KEY_REPEAT) {
				OrderMoveAndAttack();
			}
			else if (myApp->input->GetKey(SDL_SCANCODE_5) == KEY_REPEAT) {
				OrderPatrol();
			}
		}
	}
}

void Player::DebugSpawnUnit(infantry_type unit, entity_faction faction)	//TODO: This should work with unit_type alone, enum ramifications like infantry or vehicles unnecesary
{
	iPoint mousePos;
	myApp->input->GetMousePosition(mousePos.x, mousePos.y);
	mousePos = myApp->render->ScreenToWorld(mousePos.x, mousePos.y);
	myApp->entities->ActivateInfantry(fPoint((float)mousePos.x, (float)mousePos.y), unit, faction);
}

void Player::OrderHold()
{
	iPoint mousePos;
	myApp->input->GetMousePosition(mousePos.x, mousePos.y);
	mousePos = myApp->render->ScreenToWorld(mousePos.x, mousePos.y);

	for (std::list<Unit*>::iterator it = playerGroup->groupUnits_list.begin(); it != playerGroup->groupUnits_list.end(); it = next(it))
	{
		if ((*it)->IsDead() == false)
		{
			(*it)->StartHold();
		}
	}
}

void Player::OrderMove()
{
	iPoint mousePos;
	myApp->input->GetMousePosition(mousePos.x, mousePos.y);
	mousePos = myApp->render->ScreenToWorld(mousePos.x, mousePos.y);

	for (std::list<Unit*>::iterator it = playerGroup->groupUnits_list.begin(); it != playerGroup->groupUnits_list.end(); it = next(it))
	{
		if ((*it)->IsDead() == false)
		{
			(*it)->StartMove(mousePos);
		}
	}
}

void Player::OrderAttack()
{
	iPoint mousePos;
	myApp->input->GetMousePosition(mousePos.x, mousePos.y);
	mousePos = myApp->render->ScreenToWorld(mousePos.x, mousePos.y);

	for (std::list<Unit*>::iterator it = playerGroup->groupUnits_list.begin(); it != playerGroup->groupUnits_list.end(); it = next(it))
	{
		if ((*it)->IsDead() == false)
		{
			//(*it)->StartAttack(mousePos);	//TODO: Make it work
		}
	}
}

void Player::OrderMoveAndAttack()
{
	iPoint mousePos;
	myApp->input->GetMousePosition(mousePos.x, mousePos.y);
	mousePos = myApp->render->ScreenToWorld(mousePos.x, mousePos.y);

	for (std::list<Unit*>::iterator it = playerGroup->groupUnits_list.begin(); it != playerGroup->groupUnits_list.end(); it = next(it))
	{
		if ((*it)->IsDead() == false)
		{
			(*it)->StartMoveAndAttack(mousePos);
		}
	}
}

void Player::OrderPatrol()
{
	iPoint mousePos;
	myApp->input->GetMousePosition(mousePos.x, mousePos.y);
	mousePos = myApp->render->ScreenToWorld(mousePos.x, mousePos.y);

	for (std::list<Unit*>::iterator it = playerGroup->groupUnits_list.begin(); it != playerGroup->groupUnits_list.end(); it = next(it))
	{
		if ((*it)->IsDead() == false)
		{
			(*it)->StartPatrol(mousePos);
		}
	}
}

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