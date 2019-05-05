#include "Log.h"
#include "App.h"
#include "Input.h"
#include "Fonts.h"
#include "Render.h"
#include "Audio.h"
#include "Map.h"
#include "Entity_Manager.h"
#include "UserInterface.h"
#include "Text.h"
#include "ParamBox.h"
#include "GroupManager.h"
#include "Unit.h"
#include "Player.h"

bool Player::Awake()
{
	LOG("AWAKING PLAYER MODULE");

	return true;
}

bool Player::Start()
{
	LOG("STARTING PLAYER MODULE");

	mouseDebugMark = myApp->gui->CreateText({ 0.0f, 0.0f }, "Default Text", font_id::DEFAULT, { 0, 0, 255, 255 });	//TODO: In Release, string explodes sometimes, needs fix
	mouseDebugMark->Deactivate();


	return true;
}

bool Player::PreUpdate()
{
	return true;
}

bool Player::Update(float dt)
{
	//if (unitCreationCD.ReadSec() >= 10) {
	//	startCreationUnit = false;
	//}
	if (myApp->gui->MainMenuTemp_Image->active != true) {
		UpdateMousePos();	// Mouse Position Update
		CameraInputs(dt);	// Camera Inputs
		DebugInputs();		// Debug Inputs

		PlayerSelect();		// Player Area Selection Management

		CheckForOrders();
		if (myApp->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN) {
			ApplyOrders();
		}

		if (myApp->gui->interfaceDebugDraw) {
			DebugMouse();	// Mouse UI Debug data update
		}

		if (myApp->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) {
			if (myApp->gui->pauseMenuPanel->active == false) {
				myApp->gui->pauseMenuPanel->Activate();
			}
			else
				myApp->gui->pauseMenuPanel->Deactivate();
		}
	}

	return true;
}

bool Player::CleanUp()
{
	myApp->gui->DestroyElement((UI_Element*)mouseDebugMark);
	mouseDebugMark = nullptr;

	return true;
}

void Player::UpdateMousePos()
{
	myApp->input->GetMousePosition(mouseScreenPos.x, mouseScreenPos.y);
	mousePos = myApp->render->ScreenToWorld(mouseScreenPos.x, mouseScreenPos.y);
	mouseMap = myApp->map->WorldToMap(mousePos.x, mousePos.y);
}

void Player::CameraInputs(float dt)
{
	//Move Camera
	if (myApp->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		myApp->render->camera.y += (int)ceil(500 * dt);

	if (myApp->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		myApp->render->camera.y -= (int)ceil(500 * dt);

	if (myApp->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		myApp->render->camera.x += (int)ceil(500 * dt);

	if (myApp->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		myApp->render->camera.x -= (int)ceil(500 * dt);
}

void Player::DebugMouse()
{
	std::string mouseStr = std::to_string(mouseScreenPos.x);
	mouseStr += " x ";
	mouseStr += std::to_string(mouseScreenPos.y);
	mouseStr += " / ";

	mouseStr += std::to_string(mousePos.x);
	mouseStr += " x ";
	mouseStr += std::to_string(mousePos.y);
	mouseStr += " / ";

	mouseStr += std::to_string(mouseMap.x);
	mouseStr += " x ";
	mouseStr += std::to_string(mouseMap.y);

	mouseDebugMark->ChangeCenter({ (float)mouseScreenPos.x, (float)(mouseScreenPos.y - 25) });
	mouseDebugMark->ChangeString(mouseStr);
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
				mouseDebugMark->Activate();
			}
			else {
				LOG("Debug UI: OFF");
				mouseDebugMark->Deactivate();
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
			DebugSpawnUnit(infantry_type::CONSCRIPT, entity_faction::COMMUNIST);
		}
	}
}

void Player::DebugSpawnUnit(infantry_type unit, entity_faction faction)	//TODO: This should work with unit_type alone, enum ramifications like infantry or vehicles unnecesary
{
	myApp->entities->ActivateUnit(fPoint((float)mousePos.x, (float)mousePos.y), unit, faction);
}

void Player::CheckForOrders()
{
	if (myApp->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN) {	// Hold is done instantly, the others need a click
		if (myApp->groups->playerGroup.groupUnits.size() > 0) {
			OrderHold();
		}
	}
	if (myApp->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN) {
		prepOrder = unit_orders::MOVE;
	}
	if (myApp->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN) {
		prepOrder = unit_orders::ATTACK;
	}
	if (myApp->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN) {
		prepOrder = unit_orders::MOVE_AND_ATTACK;
	}
	if (myApp->input->GetKey(SDL_SCANCODE_5) == KEY_DOWN) {
		prepOrder = unit_orders::PATROL;
	}
}

void Player::ApplyOrders()
{
	if (myApp->groups->playerGroup.groupUnits.size() > 0) {
		switch (prepOrder) {
		case unit_orders::MOVE:
			OrderMove();
			break;
		case unit_orders::ATTACK:
			OrderAttack();
			break;
		case unit_orders::MOVE_AND_ATTACK:
			OrderMoveAndAttack();
			break;
		case unit_orders::PATROL:
			OrderPatrol();
			break;
		default:
			OrderMoveAndAttack();
		}
	}

	prepOrder = unit_orders::NONE;
}

void Player::OrderHold()
{
	for (std::list<Unit*>::iterator it = myApp->groups->playerGroup.groupUnits.begin(); it != myApp->groups->playerGroup.groupUnits.end(); it = next(it))
	{
		if ((*it)->IsDead() == false)
		{
			(*it)->StartHold();
		}
	}

	std::list<Unit*>::iterator it = myApp->groups->playerGroup.groupUnits.begin();
	myApp->audio->PlayFx(myApp->audio->SoundFX_Array[(int)(*it)->infantryType][(int)(*it)->faction][(int)type_sounds::COMFIRMATION][rand() % 2]);
}

void Player::OrderMove()
{
	myApp->groups->playerGroup.SpreadDestinations(mousePos);
	myApp->groups->playerGroup.TransmitOrders(unit_orders::MOVE);

	std::list<Unit*>::iterator it = myApp->groups->playerGroup.groupUnits.begin();
	myApp->audio->PlayFx(myApp->audio->SoundFX_Array[(int)(*it)->infantryType][(int)(*it)->faction][(int)type_sounds::MOVING][0]);
}

void Player::OrderAttack()
{
	Unit* attackTarget = nullptr;

	std::list<Unit*>::iterator CommunistItem = myApp->entities->ActiveCommunistUnits.begin();
	std::list<Unit*>::iterator item = myApp->entities->ActiveCapitalistUnits.begin();
	for (; (*item); item = next(item)) {

		if ((*item)->active == true && (*item)->IsDead() == false && !(mousePos.x < (*item)->UnitRect.x
			|| mousePos.x >(*item)->UnitRect.x + (*CommunistItem)->UnitRect.w
			|| mousePos.y <  (*item)->UnitRect.y
			|| mousePos.y >(*item)->UnitRect.y + (*CommunistItem)->UnitRect.h))
		{
			attackTarget = *item;
		}

		CommunistItem = next(CommunistItem);
	}

	if (attackTarget == nullptr) {
		OrderMoveAndAttack();
	}
	else {
		for (std::list<Unit*>::iterator it = myApp->groups->playerGroup.groupUnits.begin(); it != myApp->groups->playerGroup.groupUnits.end(); it = next(it))
		{
			if ((*it)->IsDead() == false)
			{
				(*it)->StartAttack(attackTarget);
			}
		}

		std::list<Unit*>::iterator it = myApp->groups->playerGroup.groupUnits.begin();
		myApp->audio->PlayFx(myApp->audio->SoundFX_Array[(int)(*it)->infantryType][(int)(*it)->faction][(int)type_sounds::ATTACK][0]);
	}
}

void Player::OrderMoveAndAttack()
{
	myApp->groups->playerGroup.SpreadDestinations(mousePos);
	myApp->groups->playerGroup.TransmitOrders(unit_orders::MOVE_AND_ATTACK);

	std::list<Unit*>::iterator it = myApp->groups->playerGroup.groupUnits.begin();
	myApp->audio->PlayFx(myApp->audio->SoundFX_Array[(int)(*it)->infantryType][(int)(*it)->faction][(int)type_sounds::MOVING][rand() % 2]);
}

void Player::OrderPatrol()
{
	myApp->groups->playerGroup.SpreadDestinations(mousePos);
	myApp->groups->playerGroup.TransmitOrders(unit_orders::PATROL);

	std::list<Unit*>::iterator it = myApp->groups->playerGroup.groupUnits.begin();
	myApp->audio->PlayFx(myApp->audio->SoundFX_Array[(int)(*it)->infantryType][(int)(*it)->faction][(int)type_sounds::COMFIRMATION][rand() % 2]);
}

void Player::PlayerSelect()
{
	if (myApp->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
		StartSelect();
	}
	else if (myApp->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {	// Process Selection Area	//TODO: Check if the first 2 conditions are necessary

		rectangle_width = mousePos.x - rectangle_origin.x;
		rectangle_height = mousePos.y - rectangle_origin.y;

		if (std::abs(rectangle_width) > 0 && std::abs(rectangle_height) > 0) {
			ExpandSelect();
		}
	}
	else if (myApp->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {	// Create Group when Mouse is unpressed
		FinishSelect();
	}
}

void Player::StartSelect()
{
	rectangle_origin = mousePos;	// Set selection area Start Point
	myApp->groups->SelectUnit(mousePos);
}

void Player::ExpandSelect()
{
	// --- Rectangle size ---
	int width = mousePos.x - rectangle_origin.x;
	int height = mousePos.y - rectangle_origin.y;

	// --- Draw Rectangle ---
	SDL_Rect SRect = { rectangle_origin.x, rectangle_origin.y, width, height };
	myApp->render->DrawQuad(SRect, 0, 200, 100, 255, false);

	// --- Once we get to the negative side of SRect numbers must be adjusted ---
	if (width < 0) {
		SRect.x = mousePos.x;
		SRect.w *= -1;
	}
	if (height < 0) {
		SRect.y = mousePos.y;
		SRect.h *= -1;
	}

	//Unit Selection
	myApp->groups->SelectUnit(SRect);
}

void Player::FinishSelect()
{
	myApp->groups->CreateGroupForPlayer();
}
