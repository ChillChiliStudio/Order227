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
	myApp->audio->PlayFx(myApp->audio->SoundFX_Array[(int)infantry_type::CONSCRIPT][SOV][(int)type_sounds::SPAWN][0]);
}

void CreateBazooka() {
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
	myApp->entities->ActivateUnit(fPoint(tempPoint.x, tempPoint.y), infantry_type::CONSCRIPT, entity_faction::COMMUNIST);
	myApp->audio->PlayFx(myApp->audio->SoundFX_Array[(int)infantry_type::CONSCRIPT][SOV][(int)type_sounds::SPAWN][0]);
}

void StartGame() {

	//MUSIC
	//myApp->audio->PlayMusic("audio/music/game/ingame_song3_loop.ogg",-1);

	//TODO make the game start Correctly
	//myApp->scene->Start();
	//myApp->entities->ActivateBuildings();	//TODO: Check if necessary, commented because it was asumed that wasn't
	//myApp->entities->ActivateObjects();	//TODO: Check if necessary, commented because it was asumed that wasn't
	myApp->gui->WinIcon->Deactivate();
	
	myApp->entities->ActivateBuildings();
	myApp->entities->ActivateObjects();
	myApp->hordes->hordeRoundto(0);
	myApp->player->playerMoney = 300;
	myApp->gui->hordeNumber_Label->ChangeString(std::to_string(0));
	myApp->hordes->hordeActive = true;
	myApp->hordes->roundTimerStart();
	myApp->gui->MainMenuTemp_Image->Deactivate();

	myApp->scene->SwitchMusic(Screen_Type::SCREEN_INGAME);
	myApp->scene->ActivateGameOverMusic = true;

}

void QuitGame() {

	//myApp->audio->PlayMusic();
	myApp->gui->LoseIcon->Deactivate();
	myApp->gui->WinIcon->Deactivate();
	//MUSIC
	myApp->audio->PlayMusic("audio/music/main_menu/menu_song_loop.ogg",-1);

	myApp->hordes->hordeActive = false;
	myApp->gui->pauseMenuPanel->Deactivate();
	myApp->gui->MainMenuTemp_Image->Activate();
	
	for (int i = 0; i < myApp->entities->unitPool.size(); i++) {	//TODO: This "seems" to work, check if it really does or needs extra steps
		if (myApp->entities->unitPool[i].active == true) {
			myApp->entities->DeActivateUnit(&myApp->entities->unitPool[i]);
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

Screen_Type getCurrentScreen() {

	if(myApp->gui->MainMenuTemp_Image->active == true)
		return Screen_Type::SCREEN_MAINMENU;
	else if (myApp->gui->LoseIcon->active == true)
		return Screen_Type::SCREEN_LOSE;
	else if (myApp->gui->WinIcon->active == true)
		return Screen_Type::SCREEN_WIN;
	else
		return Screen_Type::SCREEN_INGAME;


	return Screen_Type::SCREEN_NONE;
}