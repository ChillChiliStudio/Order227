#include "ButtonActions.h"
#include "App.h"
#include "Entity_Manager.h"
#include "time.h"
#include "Map.h"
#include "UserInterface.h"
#include "Image.h"
#include "Scene.h"
#include "Player.h"
#include "Text.h"
#include "Horde_Manager.h"
#include "GroupManager.h"
#include "Audio.h"

void CreateConscript() {
	//srand(time(NULL));

	//TODO NEED TO DESHARCODE
	fPoint randomPos = {0,0};
	int temp = rand() % 2;

	switch (temp)
	{
	case 0:
		randomPos.x = 48;
		randomPos.y = rand() % 5 + 40;
		break;
	case 1:
		randomPos.x = rand() % 5 + 43;
		randomPos.y = 45;
		break;
	}

	iPoint tempPoint =myApp->map->MapToWorld(iPoint(randomPos.x, randomPos.y));
	fPoint test = myApp->entities->mainBase->position;
	myApp->entities->ActivateUnit(fPoint(tempPoint.x, tempPoint.y), infantry_type::CONSCRIPT, entity_faction::COMMUNIST);
}

void CreateFlak() {

	//srand(time(NULL));
	//TODO NEED TO DESHARCODE
	fPoint randomPos = { 0,0 };
	int temp = rand() % 2;

	switch (temp)
	{
	case 0:
		randomPos.x = 48;
		randomPos.y = rand() % 5 + 40;
		break;
	case 1:
		randomPos.x = rand() % 5 + 43;
		randomPos.y = 45;
		break;
	}

	iPoint tempPoint = myApp->map->MapToWorld(iPoint(randomPos.x, randomPos.y));
	fPoint test = myApp->entities->mainBase->position;
	myApp->entities->ActivateLauncher(fPoint(tempPoint.x, tempPoint.y), infantry_type::BAZOOKA, entity_faction::COMMUNIST);
	myApp->audio->PlayFx(myApp->audio->SoundTroops_Array[(int)infantry_type::BAZOOKA][(int)type_sounds::SPAWN][0]);
}
void CreateChrono() {
	//srand(time(NULL));
	//TODO NEED TO DESHARCODE
	fPoint randomPos = { 0,0 };
	int temp = rand() % 2;

	switch (temp)
	{
	case 0:
		randomPos.x = 48;
		randomPos.y = rand() % 5 + 40;
		break;
	case 1:
		randomPos.x = rand() % 5 + 43;
		randomPos.y = 45;
		break;
	}

	iPoint tempPoint = myApp->map->MapToWorld(iPoint(randomPos.x, randomPos.y));
	fPoint test = myApp->entities->mainBase->position;

	myApp->entities->ActivateUnit(fPoint(tempPoint.x, tempPoint.y), infantry_type::CHRONO, entity_faction::COMMUNIST);
	myApp->audio->PlayFx(myApp->audio->SoundTroops_Array[(int)infantry_type::CHRONO][(int)type_sounds::SPAWN][0]);

}

void CreateDesolator() {
	//srand(time(NULL));
	//TODO NEED TO DESHARCODE
	fPoint randomPos = { 0,0 };
	int temp = rand() % 2;

	switch (temp)
	{
	case 0:
		randomPos.x = 48;
		randomPos.y = rand() % 5 + 40;
		break;
	case 1:
		randomPos.x = rand() % 5 + 43;
		randomPos.y = 45;
		break;
	}

	iPoint tempPoint = myApp->map->MapToWorld(iPoint(randomPos.x, randomPos.y));
	fPoint test = myApp->entities->mainBase->position;
	myApp->entities->ActivateUnit(fPoint(tempPoint.x, tempPoint.y), infantry_type::DESOLATOR, entity_faction::COMMUNIST);
	myApp->audio->PlayFx(myApp->audio->SoundTroops_Array[(int)infantry_type::DESOLATOR][(int)type_sounds::SPAWN][0]);
}

void CreateSniper() {
	//srand(time(NULL));
	//TODO NEED TO DESHARCODE
	fPoint randomPos = { 0,0 };
	int temp = rand() % 2;

	switch (temp)
	{
	case 0:
		randomPos.x = 48;
		randomPos.y = rand() % 5 + 40;
		break;
	case 1:
		randomPos.x = rand() % 5 + 43;
		randomPos.y = 45;
		break;
	}

	iPoint tempPoint = myApp->map->MapToWorld(iPoint(randomPos.x, randomPos.y));
	fPoint test = myApp->entities->mainBase->position;
	myApp->entities->ActivateUnit(fPoint(tempPoint.x, tempPoint.y), infantry_type::SNIPER, entity_faction::COMMUNIST);
	myApp->audio->PlayFx(myApp->audio->SoundTroops_Array[(int)infantry_type::SNIPER][(int)type_sounds::SPAWN][0]);

}



void StartGame() {

	//MUSIC
	//myApp->audio->PlayMusic("audio/music/game/ingame_song3_loop.ogg",-1);

	////TODO make the game start Correctly

	myApp->entities->ActivateBuildings();
	if (myApp->scene->firstGame) {
		myApp->entities->ActivateObjects();
		myApp->scene->firstGame = false;
	}

	myApp->hordes->restartRounds();
	myApp->player->playerMoney = myApp->player->initialMoney; //TODO Deharcode
	myApp->gui->hordeNumber_Label->ChangeString(std::to_string(0));
	myApp->hordes->hordeActive = true;
	myApp->hordes->roundTimer.Start();
	myApp->gui->MainMenuTemp_Image->Deactivate();
	myApp->gui->Current_Screen = Screen_Type::SCREEN_INGAME;
	myApp->scene->SwitchMusic(Screen_Type::SCREEN_INGAME);
	myApp->scene->ActivateGameOverMusic = true;
	myApp->gui->OnPause = false;
	myApp->gui->WinIcon->Deactivate();
	myApp->gui->LoseIcon->Deactivate();
}

void QuitGame() {

	//myApp->audio->PlayMusic();
	myApp->gui->LoseIcon->Deactivate();
	myApp->gui->WinIcon->Deactivate();

	//MUSIC
	//myApp->audio->PlayMusic("audio/music/main_menu/menu_song_loop.ogg",-1);

	myApp->hordes->hordeActive = false;
	myApp->gui->pauseMenuPanel->Deactivate();
	myApp->gui->MainMenuTemp_Image->Activate();

	for (int i = 0; i < myApp->entities->unitPool.size(); i++) {	//TODO: This "seems" to work, check if it really does or needs extra steps
		if (myApp->entities->unitPool[i].active == true) {
			myApp->entities->DeActivateUnit(&myApp->entities->unitPool[i]);
		}
	}
	myApp->gui->Current_Screen = Screen_Type::SCREEN_MAINMENU;

	for (int i = 0; i < myApp->entities->launcherPool.size(); i++) {	
		if (myApp->entities->launcherPool[i].active == true) {
			myApp->entities->DeActivateUnit(&myApp->entities->launcherPool[i]);
		}
	}

	myApp->scene->SwitchMusic(Screen_Type::SCREEN_MAINMENU);
	//myApp->entities->ReleasePools();	//TODO: Check if necessary, commented because it was asumed that wasn't
	//myApp->entities->ResetAll();
	//myApp->scene->CleanUp();
}

void CloseGame()
{
	myApp->mustShutDown = true;
}
