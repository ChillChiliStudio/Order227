#include "Log.h"
#include "App.h"
#include "Input.h"
#include "Controls.h"
#include "Render.h"
#include "Audio.h"
#include "Entity_Manager.h"
#include "UserInterface.h"
#include "Image.h"
#include "Text.h"
#include "Horde_Manager.h"
#include "Launcher.h"
#include "GroupManager.h"
#include "Unit.h"
#include "Player.h"
#include "Window.h"
#include "Fonts.h"

#include "Brofiler/Brofiler.h"
#include "MiniMap.h"

Player::Player()
{
	name.assign("player");
}

Player::~Player()
{}

bool Player::Awake(pugi::xml_node& node)
{
	LOG("AWAKING PLAYER MODULE");
	initialMoney = playerMoney = node.child("money").attribute("value").as_int(0);

	return true;
}

bool Player::Start()
{
	//LOG("STARTING PLAYER MODULE");

	//incomeTimer.Start();
	IncomeShow = myApp->gui->CreateText({170.0f,1350.0f}, "Default Text", font_id::MOLOT, { 255,255, 255, 255 });	//TODO: In Release, string explodes sometimes, needs fix
	UpdateText();
	IncomeShow->Deactivate();
	IncomeShow->FollowCam = true;

	//Screen limit that separates world view from player UI (used on selection and issuing orders)
	mouseWorldLimit = 540;	//TODO: Unhardcode

	return true;
}

void Player::UpdateText() {

	IncomeShow->ChangeString(std::to_string(playerIncome));

}

void Player::MoveText() {
	IncomeShow->position.y -= 1.2f;
	if (incomeTimer.ReadSec() >= 1) {
		IncomeGiven = false;
		IncomeShow->position.y = 1350.0f;
		IncomeShow->Deactivate();
	}
}

bool Player::PreUpdate()
{
	return true;
}
bool Player::PostUpdate() {
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
		if (!myApp->gui->OnPause) {
			CameraInputs(dt);	// Camera Inputs
		}

		DebugInputs();		// Debug Inputs

		PlayerSelect();		// Player Area Selection Management
		CheckForOrders();

		if (myApp->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN /*&& mouseScreenPos.y < mouseWorldLimit*/) {
			ApplyOrders();
		}

		if (myApp->map->mapDebugDraw) {
			DebugMouse();	// Mouse UI Debug data update
		}

		if (myApp->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) {
			if (myApp->gui->pauseMenuPanel->active == false) {
				myApp->gui->pauseMenuPanel->Activate();
				myApp->gui->OnPause = true;
				Mix_PauseMusic();
				Mix_Pause(-1);
			}
			else {
				myApp->gui->pauseMenuPanel->Deactivate();
				myApp->gui->OnPause = false;
				Mix_ResumeMusic();
				Mix_Resume(-1);
			}
		}

		/*if (myApp->input->GetKey(SDL_SCANCODE_O) == KEY_DOWN)
			myApp->entities->ActivateUnit(fPoint((float)mousePos.x, (float)mousePos.y), infantry_type::ENGINEER, entity_faction::COMMUNIST);*/

		/*if (myApp->input->GetKey(SDL_SCANCODE_O) == KEY_DOWN) {
			myApp->SaveGame();
		}*/
	}
	/*else if (myApp->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN) {
		myApp->LoadGame();
	}*/

	if (incomeTimer.ReadSec() >= 2 && myApp->gui->Current_Screen==Screen_Type::SCREEN_INGAME&& !myApp->gui->OnPause) {

		playerMoney += playerIncome;
		myApp->gui->Moneytext->ChangeString(std::to_string(myApp->player->playerMoney));
		incomeTimer.Start();
		IncomeGiven = true;
		IncomeShow->Activate();
	}
	if (IncomeGiven) {
		MoveText();
	}

	return true;
}

bool Player::CleanUp()
{
	return true;
}

// Save and Load
bool Player::Load(pugi::xml_node& node)
{
	playerMoney = node.attribute("money").as_int();
	playerIncome = node.attribute("income").as_int();
	incomeTimer.StartFrom(node.attribute("income_timer").as_int());
	IncomeGiven = node.attribute("income_received").as_bool();

	return true;
}

bool Player::Save(pugi::xml_node& node)
{
	node.append_attribute("money") = playerMoney;
	node.append_attribute("income") = playerIncome;
	node.append_attribute("income_timer") = incomeTimer.Read();
	node.append_attribute("income_received") = IncomeGiven;

	return true;
}

void Player::UpdateMousePos()
{
	myApp->input->GetMousePosition(mouseScreenPos.x, mouseScreenPos.y);
	mousePos = myApp->render->ScreenToWorld(mouseScreenPos.x, mouseScreenPos.y);
	mouseMap = myApp->map->WorldToMap(mousePos.x, mousePos.y);

	//If the player clicks on the minimap, we change the destination
	if (mouseScreenPos.x < myApp->minimap->minimapPosition.x + myApp->minimap->minimap_width &&
		mouseScreenPos.x >  myApp->minimap->minimapPosition.x &&
		mouseScreenPos.y < myApp->minimap->minimapPosition.y + myApp->minimap->minimap_height &&
		mouseScreenPos.y > myApp->minimap->minimapPosition.y)
	{
		orderDestination.x = (mouseScreenPos.x - myApp->minimap->minimapPosition.x-myApp->minimap->minimap_width/2)/myApp->minimap->minimapScale;
		orderDestination.y = (mouseScreenPos.y - myApp->minimap->minimapPosition.y) / myApp->minimap->minimapScale;
	}
	else
		orderDestination = mousePos;
}

void Player::CameraInputs(float dt)
{
	iPoint mousePos;
	myApp->input->GetMousePosition(mousePos.x, mousePos.y);

	//Move camera upwards
	if ((myApp->input->GetKey(myApp->controls->camera.up) == KEY_REPEAT || mousePos.y >= 0 && mousePos.y < SCREEN_MOVEMENT_MARGIN) &&
		-myApp->render->camera.y > 0)
		myApp->render->camera.y += (int)ceil(CAMERA_SPEED  * dt);


	//Move camera downwards
	if ((myApp->input->GetKey(myApp->controls->camera.down) == KEY_REPEAT || mousePos.y > myApp->win->height - SCREEN_MOVEMENT_MARGIN && mousePos.y < myApp->win->height)&&
		-myApp->render->camera.y < (myApp->map->data.height*myApp->map->data.tile_height - int(myApp->win->height)))
		myApp->render->camera.y -= (int)ceil(CAMERA_SPEED * dt);


	//Move camera to the left
	if ((myApp->input->GetKey(myApp->controls->camera.left) == KEY_REPEAT || mousePos.x >= 0 && mousePos.x < SCREEN_MOVEMENT_MARGIN) &&
		-myApp->render->camera.x > -(myApp->map->data.width *myApp->map->data.tile_width)/2)
		myApp->render->camera.x += (int)ceil(CAMERA_SPEED  * dt);


	//Move camera to the right
	if ((myApp->input->GetKey(myApp->controls->camera.right) == KEY_REPEAT|| mousePos.x > myApp->win->width - SCREEN_MOVEMENT_MARGIN && mousePos.x < myApp->win->width)&&
		-myApp->render->camera.x + int(myApp->win->width) < (myApp->map->data.width *myApp->map->data.tile_width) / 2)
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

	myApp->gui->mouseDebugMark->ChangeCenter({ (float)mouseScreenPos.x, (float)(mouseScreenPos.y - 25) });
	myApp->gui->mouseDebugMark->ChangeString(mouseStr);
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
		if (myApp->input->GetKey(myApp->controls->debug.debugMap) == KEY_DOWN) {	// Toggle Map debug draw
			myApp->map->mapDebugDraw = !myApp->map->mapDebugDraw;

			if (myApp->map->mapDebugDraw) {
				LOG("Debug Map: ON");
				myApp->gui->mouseDebugMark->Activate();	//TODO: There should be a button for FPS alone
				myApp->gui->fpsText->Activate();
			}
			else {
				LOG("Debug Map: OFF");
				myApp->gui->mouseDebugMark->Deactivate();
				myApp->gui->fpsText->Deactivate();
			}
		}

		if (myApp->input->GetKey(myApp->controls->debug.debugUI) == KEY_DOWN) {	// Toggle UI debug draw
			myApp->gui->interfaceDebugDraw = !myApp->gui->interfaceDebugDraw;

			if (myApp->gui->interfaceDebugDraw) {
				LOG("Debug UI: ON");
			}
			else {
				LOG("Debug UI: OFF");
			}
		}

		if (myApp->input->GetKey(myApp->controls->debug.debugEntities) == KEY_DOWN) {		// Toggle Entities debug draw
			myApp->entities->entitiesDebugDraw = !myApp->entities->entitiesDebugDraw;

			if (myApp->entities->entitiesDebugDraw) {
				LOG("Debug Entities: ON");
			}
			else {
				LOG("Debug Entities: OFF");
			}
		}

		if (myApp->input->GetKey(myApp->controls->debug.nextRound) == KEY_DOWN) {			// Activate Next Round
			myApp->hordes->ChooseSpawningPoints();
		}

		if (myApp->input->GetKey(myApp->controls->debug.spawnBlueDog) == KEY_DOWN) {		// Spawn Enemy Dog Mouse
			DebugSpawnUnit(infantry_type::DOG, entity_faction::CAPITALIST);
		}

		if (myApp->input->GetKey(myApp->controls->debug.spawnBlueChrono) == KEY_DOWN) {		// Spawn Enemy Chrono Mouse (Old: Kill/Deactivate all enemies)
			/*myApp->hordes->ClearEnemies();*/
			DebugSpawnUnit(infantry_type::CHRONO, entity_faction::CAPITALIST);

		}
		if (myApp->input->GetKey(myApp->controls->debug.spawnBlueSniper) == KEY_DOWN) {		// Spawn Enemy Sniper Mouse
			DebugSpawnUnit(infantry_type::SNIPER, entity_faction::CAPITALIST);
		}

		if (myApp->input->GetKey(myApp->controls->debug.spawnRedChrono) == KEY_DOWN) {		// Spawn Chrono Mouse
			DebugSpawnUnit(infantry_type::CHRONO, entity_faction::COMMUNIST);
		}

		if (myApp->input->GetKey(myApp->controls->debug.spawnRedDesolator) == KEY_DOWN) {	// Spawn Desolator on Mouse
			DebugSpawnUnit(infantry_type::DESOLATOR, entity_faction::COMMUNIST);
		}

		if (myApp->input->GetKey(myApp->controls->debug.spawnRedBazooka) == KEY_DOWN) {		// Spawn Bazooka on Mouse
			DebugSpawnLauncher(infantry_type::BAZOOKA, entity_faction::COMMUNIST);
		}

		if (myApp->input->GetKey(myApp->controls->debug.spawnRedConscript) == KEY_DOWN) {	// Spawn Conscript on Mouse
			DebugSpawnUnit(infantry_type::CONSCRIPT, entity_faction::COMMUNIST);
		}
	}
}

void Player::DebugSpawnUnit(infantry_type unit, entity_faction faction)	//TODO: This should work with unit_type alone, enum ramifications like infantry or vehicles unnecesary
{
	Unit* tmp = myApp->entities->ActivateUnit(fPoint((float)mousePos.x, (float)mousePos.y), unit, faction);
	tmp->StartHold();
}

void Player::DebugSpawnLauncher(infantry_type unit, entity_faction faction)	//TODO: This should work with unit_type alone, enum ramifications like infantry or vehicles unnecesary
{
	Launcher* tmp = myApp->entities->ActivateLauncher(fPoint((float)mousePos.x, (float)mousePos.y), unit, faction);
	tmp->StartHold();
}

void Player::CheckForOrders()
{

	if (myApp->input->GetKey(myApp->controls->orders.hold) == KEY_DOWN) {	// Hold is done instantly, the others need a click
		if (myApp->groups->playerGroup.groupUnits.size() > 0) {

			ApplyAggroLevel(GetAggroLevel());
			OrderHold();
		}
	}

	/*if (myApp->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN) {
		prepOrder = unit_orders::MOVE;
	}*/
	if (myApp->input->GetKey(myApp->controls->orders.hunt) == KEY_DOWN) {
		prepOrder = unit_orders::HUNT;
	}
	if (myApp->input->GetKey(myApp->controls->orders.patrol) == KEY_DOWN) {
		prepOrder = unit_orders::PATROL;
	}
}

unit_aggro Player::GetAggroLevel()
{
	unit_aggro ret = unit_aggro::PASSIVE;	// Default Defensive

	if (myApp->input->GetKey(myApp->controls->orders.aggressive) == KEY_REPEAT) {	// Hold LEFT CONTROL for Aggresive
		ret = unit_aggro::AGGRESSIVE;
	}
	else if (myApp->input->GetKey(myApp->controls->orders.defensive) == KEY_REPEAT) {	// Hold LEFT SHIFT for Defensive
		ret = unit_aggro::DEFENSIVE;
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
	int Aux = myApp->audio->VarsXsound[(int)(*it)->infantryType][(int)type_sounds::CONFIRMATION];
	myApp->audio->PlayFx(myApp->audio->SoundTroops_Array[(int)(*it)->infantryType][(int)type_sounds::CONFIRMATION][rand() % Aux], 0, CHANNEL_CONFIRMATION);
}

void Player::OrderMove()
{
	myApp->groups->playerGroup.SpreadDestinations(orderDestination);
	myApp->groups->playerGroup.TransmitOrders(unit_orders::MOVE);

	std::list<Unit*>::iterator it = myApp->groups->playerGroup.groupUnits.begin();
	int Aux = myApp->audio->VarsXsound[(int)(*it)->infantryType][(int)type_sounds::MOVING];
	myApp->audio->PlayFx(myApp->audio->SoundTroops_Array[(int)(*it)->infantryType][(int)type_sounds::MOVING][rand() % Aux], 0, CHANNEL_MOVING);
}

void Player::OrderHunt()
{
	Unit* selectedTarget = nullptr;

	for (std::vector<Unit>::iterator item = myApp->entities->unitPool.begin(); item != myApp->entities->unitPool.end(); item = next(item)) {
		if ((*item).active == true && (*item).faction == entity_faction::CAPITALIST && (*item).IsDead() == false &&
			!(orderDestination.x < (*item).entityRect.x || orderDestination.x >(*item).entityRect.x + (*item).entityRect.w
				|| orderDestination.y < (*item).entityRect.y || orderDestination.y >(*item).entityRect.y + (*item).entityRect.h))
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
		int Aux = myApp->audio->VarsXsound[(int)(*it)->infantryType][(int)type_sounds::ATTACK];
		myApp->audio->PlayFx(myApp->audio->SoundTroops_Array[(int)(*it)->infantryType][(int)type_sounds::ATTACK][rand() % Aux], 0, CHANNEL_ATTACK);
	}
	else {
		OrderMove();
	}
}

void Player::OrderPatrol()
{
	myApp->groups->playerGroup.SpreadDestinations(orderDestination);
	myApp->groups->playerGroup.TransmitOrders(unit_orders::PATROL);

	std::list<Unit*>::iterator it = myApp->groups->playerGroup.groupUnits.begin();
	int Aux = myApp->audio->VarsXsound[(int)(*it)->infantryType][(int)type_sounds::CONFIRMATION];
	myApp->audio->PlayFx(myApp->audio->SoundTroops_Array[(int)(*it)->infantryType][(int)type_sounds::CONFIRMATION][rand() % Aux], 0, CHANNEL_CONFIRMATION);
}

void Player::PlayerSelect()
{
	if (myApp->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
		iPoint mousePosition;
		myApp->input->GetMousePosition(mousePosition.x, mousePosition.y);

		if (mouseScreenPos.y < mouseWorldLimit && !myApp->gui->OnPause) {

			StartSelect();
			selectionStarted = true;
		}
	}
	else if (selectionStarted == true && myApp->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {	// Process Selection Area	//TODO: Check if the first 2 conditions are necessary

		rectangle_width = mousePos.x - rectangle_origin.x;
		rectangle_height = mousePos.y - rectangle_origin.y;

		if (std::abs(rectangle_width) > 0 && std::abs(rectangle_height) > 0) {
			ExpandSelect();
		}
	}
	else if (selectionStarted == true && myApp->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {	// Create Group when Mouse is unpressed
		FinishSelect();
		selectionStarted = false;
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
