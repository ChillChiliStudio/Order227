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
#include "Horde_Manager.h"
#include "GroupManager.h"
#include "Unit.h"
#include "Player.h"
#include "Window.h"
#include "Brofiler/Brofiler.h"
#include "MiniMap.h"

bool Player::Awake(pugi::xml_node& node)
{
	LOG("AWAKING PLAYER MODULE");

	return true;
}

bool Player::Start()
{
	//LOG("STARTING PLAYER MODULE");

	//incomeTimer.Start();
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
	BROFILER_CATEGORY("Player.cpp Update()-Blue", Profiler::Color::Blue);
	//if (unitCreationCD.ReadSec() >= 10) {
	//	startCreationUnit = false;
	//}
	if (myApp->gui->MainMenuTemp_Image->active == false) {

		UpdateMousePos();	// Mouse Position Update
		CameraInputs(dt);	// Camera Inputs
		DebugInputs();		// Debug Inputs

		PlayerSelect();		// Player Area Selection Management
		CheckForOrders();

		if (myApp->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN) {
			ApplyOrders();
		}

		if (myApp->map->mapDebugDraw) {
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

	if (incomeTimer.ReadSec() >= 2) {

		playerMoney += playerIncome;
		myApp->gui->Moneytext->ChangeString(std::to_string(myApp->player->playerMoney));
		incomeTimer.Start();
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
	iPoint mousePos;
	myApp->input->GetMousePosition(mousePos.x, mousePos.y);

	//Move camera upwards
	if (myApp->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		myApp->render->camera.y += (int)ceil(CAMERA_SPEED  * dt);

	else if (mousePos.y >= 0 && mousePos.y < SCREEN_MOVEMENT_MARGIN)
		myApp->render->camera.y += (int)ceil(CAMERA_SPEED  * dt);

	//Move camera downwards
	if (myApp->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		myApp->render->camera.y -= (int)ceil(CAMERA_SPEED * dt);

	else if (mousePos.y > myApp->win->height - SCREEN_MOVEMENT_MARGIN && mousePos.y < myApp->win->height)
		myApp->render->camera.y -= (int)ceil(CAMERA_SPEED  * dt);

	//Move camera to the left
	if (myApp->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		myApp->render->camera.x += (int)ceil(CAMERA_SPEED  * dt);

	else if (mousePos.x >= 0 && mousePos.x < SCREEN_MOVEMENT_MARGIN)
		myApp->render->camera.x += (int)ceil(CAMERA_SPEED  * dt);

	//Move camera to the right
	if (myApp->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		myApp->render->camera.x -= (int)ceil(CAMERA_SPEED  * dt);

	else if (mousePos.x > myApp->win->width- SCREEN_MOVEMENT_MARGIN && mousePos.x < myApp->win->width)
		myApp->render->camera.x -= (int)ceil(CAMERA_SPEED  * dt);

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
	/*if (myApp->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN) {
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
	}*/

	if (myApp->debugMode) {
		if (myApp->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN) {	// Toggle Map debug draw
			myApp->map->mapDebugDraw = !myApp->map->mapDebugDraw;

			if (myApp->map->mapDebugDraw) {
				LOG("Debug Map: ON");
				mouseDebugMark->Activate();
			}
			else {
				LOG("Debug Map: OFF");
				mouseDebugMark->Deactivate();
			}
		}

		if (myApp->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {	// Toggle UI debug draw
			myApp->gui->interfaceDebugDraw = !myApp->gui->interfaceDebugDraw;

			if (myApp->gui->interfaceDebugDraw) {
				LOG("Debug UI: ON");
			}
			else {
				LOG("Debug UI: OFF");
			}
		}

		if (myApp->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) {	// Toggle Entities debug draw
			myApp->entities->entitiesDebugDraw = !myApp->entities->entitiesDebugDraw;

			if (myApp->entities->entitiesDebugDraw) {
				LOG("Debug Entities: ON");
			}
			else {
				LOG("Debug Entities: OFF");
			}
		}

		if (myApp->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN) {	// Insta-Win
			
		}

		if (myApp->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN) {	// Insta-Lose

		}

		if (myApp->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) {

		}

		if (myApp->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) {
			
		}

		if (myApp->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN) {	// Instantly start next round
			myApp->hordes->ChooseSpawningPoints();
		}

		if (myApp->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) {	// Destroy all enemies
			myApp->hordes->ClearEnemies();
		}

		if (myApp->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) {	// Spawn Capitalist Unit on Mouse
			DebugSpawnUnit(infantry_type::BASIC, entity_faction::CAPITALIST);
		}

		if (myApp->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {	// Spawn Communist Unit on Mouse
			DebugSpawnUnit(infantry_type::CONSCRIPT, entity_faction::COMMUNIST);
		}
	}
}

void Player::DebugSpawnUnit(infantry_type unit, entity_faction faction)	//TODO: This should work with unit_type alone, enum ramifications like infantry or vehicles unnecesary
{
	Unit* tmp = myApp->entities->ActivateUnit(fPoint((float)mousePos.x, (float)mousePos.y), unit, faction);
	tmp->StartHold();
}

void Player::CheckForOrders()
{
	unit_aggro aggro;

	if (myApp->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {	// Hold is done instantly, the others need a click
		if (myApp->groups->playerGroup.groupUnits.size() > 0) {
			ApplyAggroLevel(GetAggroLevel());
			OrderHold();
		}
	}
	if (myApp->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN) {
		prepOrder = unit_orders::MOVE;
	}
	if (myApp->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN) {
		prepOrder = unit_orders::HUNT;
	}
	if (myApp->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN) {
		prepOrder = unit_orders::PATROL;
	}
}

unit_aggro Player::GetAggroLevel()
{
	unit_aggro ret = unit_aggro::DEFENSIVE;	// Default Defensive

	if (myApp->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT) {	// Hold LEFT CONTROL for Aggresive
		ret = unit_aggro::AGGRESSIVE;
	}
	else if (myApp->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) {	// Hold LEFT SHIFT for Passive
		ret = unit_aggro::PASSIVE;
	}
	
	return ret;
}

void Player::ApplyAggroLevel(unit_aggro aggro)
{
	for (std::list<Unit*>::iterator it = myApp->groups->playerGroup.groupUnits.begin(); it != myApp->groups->playerGroup.groupUnits.end(); it = next(it))
	{
		if ((*it)->IsDead() == false) {
			(*it)->unitAggro = aggro;
		}
	}
}

void Player::ApplyOrders()
{
	BROFILER_CATEGORY("Player Apply Unit Orders", Profiler::Color::Cyan);

	if (myApp->groups->playerGroup.groupUnits.size() > 0) {
		switch (prepOrder) {
		case unit_orders::MOVE:
			ApplyAggroLevel(GetAggroLevel());
			OrderMove();
			break;
		case unit_orders::HUNT:
			ApplyAggroLevel(GetAggroLevel());
			OrderHunt();
			break;
		case unit_orders::PATROL:
			ApplyAggroLevel(GetAggroLevel());
			OrderPatrol();
			break;
		default:
			ApplyAggroLevel(GetAggroLevel());
			OrderHunt();
		}
	}

	prepOrder = unit_orders::NONE;
}

void Player::OrderHold()
{
	myApp->groups->playerGroup.TransmitOrders(unit_orders::HOLD);

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

void Player::OrderHunt()
{
	Unit* selectedTarget = nullptr;

	for (std::vector<Unit>::iterator item = myApp->entities->unitPool.begin(); item != myApp->entities->unitPool.end(); item = next(item)) {
		if ((*item).active == true && (*item).faction == entity_faction::CAPITALIST && (*item).IsDead() == false &&
			!(mousePos.x < (*item).entityRect.x || mousePos.x >(*item).entityRect.x + (*item).entityRect.w
				|| mousePos.y < (*item).entityRect.y || mousePos.y >(*item).entityRect.y + (*item).entityRect.h))
		{
			selectedTarget = &(*item);
			break;
		}
	}

	if (selectedTarget != nullptr) {
		for (std::list<Unit*>::iterator it = myApp->groups->playerGroup.groupUnits.begin(); it != myApp->groups->playerGroup.groupUnits.end(); it = next(it))
		{
			if ((*it)->IsDead() == false) {
				(*it)->StartHunt(selectedTarget);
			}
		}

		std::list<Unit*>::iterator it = myApp->groups->playerGroup.groupUnits.begin();
		myApp->audio->PlayFx(myApp->audio->SoundFX_Array[(int)(*it)->infantryType][(int)(*it)->faction][(int)type_sounds::ATTACK][0]);
	}
	else {
		OrderMove();
	}
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
		iPoint mousePosition;
		myApp->input->GetMousePosition(mousePosition.x, mousePosition.y);

		if (mousePosition.x > myApp->minimap->minimapPosition.x + myApp->minimap->minimap_width &&
			mousePosition.x < myApp->minimap->minimapPosition.x &&
			mousePosition.y > myApp->minimap->minimapPosition.y + myApp->minimap->minimap_height &&
			mousePosition.y < myApp->minimap->minimapPosition.y)
			return;

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
	myApp->render->DrawQuad(SRect, 0, 255, 100, 255, false);
	myApp->render->DrawQuad(SRect, 0, 200, 100, 40, true);

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
