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

	PlayerSelect();

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
			myApp->entities->entitiesDebugDraw = false;
			LOG("Debug Mode: OFF");
		}
		else {
			LOG("Debug Mode: ON");
		}
	}

	if (myApp->debugMode) {
		if (myApp->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {	// Toggle Map debug draw
			myApp->map->mapDebugDraw = !myApp->map->mapDebugDraw;

			if (myApp->map->mapDebugDraw) {
				LOG("Debug Map: ON");
			}
			else {
				LOG("Debug Map: OFF");
			}
		}

		if (myApp->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) {	// Toggle UI debug draw
			myApp->gui->interfaceDebugDraw = !myApp->gui->interfaceDebugDraw;

			if (myApp->gui->interfaceDebugDraw) {
				LOG("Debug UI: ON");
			}
			else {
				LOG("Debug UI: OFF");
			}
		}

		if (myApp->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN) {	// Toggle Entities debug draw
			myApp->entities->entitiesDebugDraw = !myApp->entities->entitiesDebugDraw;

			if (myApp->entities->entitiesDebugDraw) {
				LOG("Debug Entities: ON");
			}
			else {
				LOG("Debug Entities: OFF");
			}
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

		DebugOrders();
	}
}

void Player::DebugSpawnUnit(infantry_type unit, entity_faction faction)	//TODO: This should work with unit_type alone, enum ramifications like infantry or vehicles unnecesary
{
	iPoint mousePos;
	myApp->input->GetMousePosition(mousePos.x, mousePos.y);
	mousePos = myApp->render->ScreenToWorld(mousePos.x, mousePos.y);
	myApp->entities->ActivateInfantry(fPoint((float)mousePos.x, (float)mousePos.y), unit, faction);
}

void Player::DebugOrders()
{
	if (myApp->groups->playerGroup.groupUnits.size() > 0) {
		if (myApp->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN) {	// Mantain a NumKey (1-5) for Order type
			DebugOrderHold();
		}
		else if (myApp->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN) { // Press RightMouseButton to mark destination
			if (myApp->input->GetKey(SDL_SCANCODE_2) == KEY_REPEAT) {
				DebugOrderMove();
			}
			else if (myApp->input->GetKey(SDL_SCANCODE_3) == KEY_REPEAT) {
				DebugOrderAttack();
			}
			else if (myApp->input->GetKey(SDL_SCANCODE_4) == KEY_REPEAT) {
				DebugOrderMoveAndAttack();
			}
			else if (myApp->input->GetKey(SDL_SCANCODE_5) == KEY_REPEAT) {
				DebugOrderPatrol();
			}
		}
	}
}

void Player::DebugOrderHold()
{
	for (std::list<Unit*>::iterator it = myApp->groups->playerGroup.groupUnits.begin(); it != myApp->groups->playerGroup.groupUnits.end(); it = next(it))
	{
		if ((*it)->IsDead() == false)
		{
			(*it)->StartHold();
		}
	}
}

void Player::DebugOrderMove()
{
	myApp->input->GetMousePosition(mousePos.x, mousePos.y);
	myApp->render->ScreenToWorld(&mousePos.x, &mousePos.y);
	
	myApp->groups->playerGroup.SpreadDestinations(mousePos);
	myApp->groups->playerGroup.TransmitOrders(unit_orders::MOVE);
}

void Player::DebugOrderAttack()
{
	iPoint mousePos;
	myApp->input->GetMousePosition(mousePos.x, mousePos.y);
	mousePos = myApp->render->ScreenToWorld(mousePos.x, mousePos.y);

	for (std::list<Unit*>::iterator it = myApp->groups->playerGroup.groupUnits.begin(); it != myApp->groups->playerGroup.groupUnits.end(); it = next(it))
	{
		if ((*it)->IsDead() == false)
		{
			//(*it)->StartAttack(mousePos);	//TODO: Make it work
		}
	}
}

void Player::DebugOrderMoveAndAttack()
{
	myApp->input->GetMousePosition(mousePos.x, mousePos.y);
	myApp->render->ScreenToWorld(&mousePos.x, &mousePos.y);

	myApp->groups->playerGroup.SpreadDestinations(mousePos);
	myApp->groups->playerGroup.TransmitOrders(unit_orders::MOVE_AND_ATTACK);
}

void Player::DebugOrderPatrol()
{
	myApp->input->GetMousePosition(mousePos.x, mousePos.y);
	myApp->render->ScreenToWorld(&mousePos.x, &mousePos.y);

	myApp->groups->playerGroup.SpreadDestinations(mousePos);
	myApp->groups->playerGroup.TransmitOrders(unit_orders::PATROL);
}

void Player::PlayerSelect()
{
	myApp->input->GetMousePosition(mousePos.x, mousePos.y);
	mouseScreenPos = myApp->render->ScreenToWorld(mousePos.x, mousePos.y);

	rectangle_width = mouseScreenPos.x - rectangle_origin.x;
	rectangle_height = mouseScreenPos.y - rectangle_origin.y;

	if (myApp->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
		StartSelect();
	}
	else if (std::abs(rectangle_width) > 5 && std::abs(rectangle_height) > 5 && myApp->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {	// Process Selection Area	//TODO: Check if the first 2 conditions are necessary
		ExpandSelect();
	}
	else if (myApp->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {	// Create Group when Mouse is unpressed
		FinishSelect();
	}
}

void Player::StartSelect()
{
	rectangle_origin = mouseScreenPos;	// Set selection area Start Point
	myApp->groups->SelectUnit(mouseScreenPos);	// Select any units on mouse click point
}

void Player::ExpandSelect()
{
	// --- Rectangle size ---
	int width = mouseScreenPos.x - rectangle_origin.x;
	int height = mouseScreenPos.y - rectangle_origin.y;

	// --- Draw Rectangle ---
	SDL_Rect SRect = { rectangle_origin.x, rectangle_origin.y, width, height };
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

	//Unit Selection
	myApp->groups->SelectUnit(SRect);
}

void Player::FinishSelect()
{
	myApp->groups->CreateGroupForPlayer();
}