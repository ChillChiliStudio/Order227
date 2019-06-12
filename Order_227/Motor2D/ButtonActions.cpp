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
#include "Video.h"
#include "Audio.h"
#include "UnitButton.h"
#include "VoidBox.h"
#include "KeyConfig.h"
#include "TutorialBox.h"
#include "Window.h"
#include "Controls.h"
#include "Log.h"
#include "Slider.h"

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
void CreateEngineer() {
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
	myApp->entities->ActivateUnit(fPoint(tempPoint.x, tempPoint.y), infantry_type::ENGINEER, entity_faction::COMMUNIST);
	myApp->audio->PlayFx(myApp->audio->SoundTroops_Array[(int)infantry_type::ENGINEER][(int)type_sounds::SPAWN][0]);

}

void HotkeyButtonPrepare(int* code)
{
	myApp->controls->PrepareInputChange(code);
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

	myApp->gui->DeactivateScreen(myApp->gui->Main_Menu_Elements);
	myApp->gui->ActivateScreen(myApp->gui->InGame_Elements);


	myApp->hordes->restartRounds();
	myApp->player->playerMoney = myApp->player->initialMoney; //TODO Deharcode
	myApp->gui->hordeNumber_Label->ChangeString(std::to_string(0));
	myApp->hordes->hordeActive = true;
	myApp->hordes->roundTimer.Start();
	myApp->gui->MainMenuTemp_Image->Deactivate();
	
	myApp->gui->Current_Screen = Screen_Type::SCREEN_INGAME;
	myApp->scene->SwitchMusic(Screen_Type::SCREEN_INGAME);
	Mix_Resume(-1);
	myApp->scene->ActivateGameOverMusic = true;
	myApp->video->StopVideo();
	//myApp->video->PlayVideo("Video/iterator_hordes.ogv", SDL_Rect({ 0,(int)(myApp->win->height/2.8f),1280,212 }), false);
	myApp->gui->OnPause = false;

	myApp->render->camera.x = 400;
	myApp->render->camera.y = -1000;

	myApp->gui->WinIcon->Deactivate();
	myApp->gui->LoseIcon->Deactivate();

	//for (int i = 0; i < myApp->entities->buildingsArray.size(); i++) {

	//	if (myApp->entities->buildingsArray[i].buildingType == building_type::COMMAND_CENTER) {

	//		myApp->player->playerIncome = myApp->entities->buildingsArray[i].income;
	//		break;
	//	}
	//}

	myApp->playingGame = true;
}

void RequestMenuLoad()
{
	myApp->LoadGame();

	if (myApp->scene->firstGame) {
		myApp->entities->ActivateObjects();
		myApp->scene->firstGame = false;
	}

	myApp->playingGame = true;
}

void RequestLoad()
{
	QuitGame();
	myApp->LoadGame();

	myApp->playingGame = true;
}

void RequestSave()
{
	myApp->SaveGame();

	if (!myApp->saveFileExists) {

		myApp->gui->ContinueGame_Button->Enable();
		myApp->gui->LoadGame_Button->Enable();
		myApp->saveFileExists = true;
	}
}

void QuitGame() {

	//myApp->audio->PlayMusic();
	for (int i = 0; i < myApp->entities->buildingsArray.size(); i++) {

		if(myApp->entities->buildingsArray[i].buildingType != building_type::COMMAND_CENTER)
			myApp->entities->buildingsArray[i].Hurt(myApp->entities->buildingsArray[i].health);

		myApp->player->playerIncome = 0;
		
	}

	myApp->gui->LoseIcon->Deactivate();
	myApp->gui->WinIcon->Deactivate();
	myApp->entities->heavyUnitsUnlocked = false;

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

	myApp->gui->ConscriptCreator->ResetButton();
	myApp->gui->FlakCreator->ResetButton();
	myApp->gui->SniperCreator->ResetButton();
	myApp->gui->ChronoCreator->ResetButton();
	myApp->gui->DesolatorCreator->ResetButton();
	myApp->gui->EngineerCreator->ResetButton();
	myApp->gui->DeactivateScreen(myApp->gui->InGame_Elements);
	myApp->gui->ActivateScreen(myApp->gui->Main_Menu_Elements);
	myApp->scene->SwitchMusic(Screen_Type::SCREEN_MAINMENU);
	myApp->video->PlayVideo("Video/Main_Menu_Background.ogv", { 0,0,1280,720 },true);
	//myApp->entities->ReleasePools();	//TODO: Check if necessary, commented because it was asumed that wasn't
	//myApp->entities->ResetAll();
	//myApp->scene->CleanUp();

	myApp->entities->unitBuff = false;
	myApp->entities->buildingsBuff = false;
	myApp->entities->heavyUnitsUnlocked = false;
	myApp->entities->incomeBuff1 = false;
	myApp->entities->incomeBuff2 = false;
	myApp->entities->incomeBuff45 = false;

	myApp->gui->OnPause = false;
	myApp->playingGame = false;
}

void CloseGame()
{
	myApp->mustShutDown = true;
}

void OptionsOpen() {
	myApp->gui->DeactivateScreen(myApp->gui->Main_Menu_Elements);
	myApp->gui->pauseMenuPanel->Deactivate();
	myApp->gui->OptionsPanel->Activate();
	myApp->gui->SetHotkeys_Button->Activate();
}
void QuitOptions() {
	if (myApp->gui->OnPause == false && myApp->gui->Hotkey_Conscript->active == false) {
		myApp->gui->OptionsPanel->Deactivate();
		myApp->gui->ActivateScreen(myApp->gui->Main_Menu_Elements);
	}
	else if (myApp->gui->Hotkey_Conscript->active == false) {
		myApp->gui->OptionsPanel->Deactivate();
		myApp->gui->pauseMenuPanel->Activate();
	}
	else {
		myApp->gui->Hotkey_Conscript->Deactivate();
		myApp->gui->Hotkey_Flak->Deactivate();
		myApp->gui->Hotkey_Desolator->Deactivate();
		myApp->gui->Hotkey_Chrono->Deactivate();
		myApp->gui->Hotkey_Sniper->Deactivate();
		myApp->gui->Hotkey_Engineer->Deactivate();

		myApp->gui->Hotkey_Hold->Deactivate();
		myApp->gui->Hotkey_Hunt->Deactivate();
		myApp->gui->Hotkey_Patrol->Deactivate();

		myApp->gui->Hotkey_Defensive->Deactivate();
		myApp->gui->Hotkey_Aggressive->Deactivate();

		myApp->gui->OptionsPanel->Activate();
		myApp->gui->SetHotkeys_Button->Activate();
		myApp->gui->Hotkey_Up->Deactivate();
		myApp->gui->Hotkey_Down->Deactivate();
		myApp->gui->Hotkey_Left->Deactivate();
		myApp->gui->Hotkey_Right->Deactivate();

		if (myApp->controls->awaitingInput) {
			myApp->audio->PlayFx(myApp->audio->SoundUI_Player[(int)UI_playerSound::UI_ERROR], 0, CHANNEL_UI);
			myApp->controls->awaitingInput = false;

			myApp->controls->buttonUsed->OnIdle();
			myApp->controls->buttonUsed = nullptr;
		}
	}

}

void Hotkeys_Options() {

	myApp->gui->VolumeSFX_Slide->active = false;
	myApp->gui->VolumeMusic_Slide->Deactivate();
	myApp->gui->SetHotkeys_Button->Deactivate();
	myApp->gui->Hotkey_Conscript->Activate();
	myApp->gui->Hotkey_Flak->Activate();
	myApp->gui->Hotkey_Desolator->Activate();
	myApp->gui->Hotkey_Chrono->Activate();
	myApp->gui->Hotkey_Sniper->Activate();
	myApp->gui->Hotkey_Engineer->Activate();

	myApp->gui->Hotkey_Hold->Activate();
	myApp->gui->Hotkey_Hunt->Activate();
	myApp->gui->Hotkey_Patrol->Activate();

	myApp->gui->Hotkey_Defensive->Activate();
	myApp->gui->Hotkey_Aggressive->Activate();

	myApp->gui->Hotkey_Up->Activate();
	myApp->gui->Hotkey_Down->Activate();
	myApp->gui->Hotkey_Left->Activate();
	myApp->gui->Hotkey_Right->Activate();

	myApp->gui->musicSlider->Deactivate();
	myApp->gui->sfxSlider->Deactivate();
}
void QuitTutorial() {
	myApp->gui->Tutorial->currentPage = 0;
	myApp->gui->Tutorial->Rect.x = 0;
	myApp->gui->Tutorial->Deactivate();
	myApp->gui->Tutorial_Arrow_Foreward->Deactivate();
	myApp->gui->Tutorial_Arrow_Back->Deactivate();
	myApp->gui->ReturnfromTutorial_Button->Deactivate();
	myApp->gui->ReturnfromTutorial_Label->Deactivate();
	myApp->gui->ActivateScreen(myApp->gui->Main_Menu_Elements);
}

void TutorialOpen() {

	myApp->gui->DeactivateScreen(myApp->gui->Main_Menu_Elements);
	myApp->gui->Tutorial->Activate();
	myApp->gui->Tutorial->currentPage = 0;
	myApp->gui->Tutorial_Arrow_Foreward->Activate();
	myApp->gui->Tutorial_Arrow_Back->Activate();
	myApp->gui->ReturnfromTutorial_Button->Activate();
	myApp->gui->ReturnfromTutorial_Label->Activate();
}
void NextPage_Tutorial() {
	if(myApp->gui->Tutorial->movingforeward==false&& myApp->gui->Tutorial->movingBackward==false
		&&myApp->gui->Tutorial->currentPage < 7)
		myApp->gui->Tutorial->Front = true;
}
void BackPage_Tutorial() {
	if (myApp->gui->Tutorial->movingforeward == false && myApp->gui->Tutorial->movingBackward == false
		&&myApp->gui->Tutorial->currentPage>0)
		myApp->gui->Tutorial->Back = true;
}
void OpenCredits() {
	myApp->gui->DeactivateScreen(myApp->gui->Main_Menu_Elements);
	myApp->gui->Lucho_Button->Activate();
	myApp->gui->Jaume_Button->Activate();
	myApp->gui->Roger_Button->Activate();
	myApp->gui->Adri_Button->Activate();
	myApp->gui->Joan_Button->Activate();
	myApp->gui->Carles_Button->Activate();
	myApp->gui->Rafel_Button->Activate();
	myApp->gui->ReturnfromCredits_Button->Activate();
}
void CloseCredits() {
	myApp->gui->ActivateScreen(myApp->gui->Main_Menu_Elements);
	myApp->gui->Lucho_Button->Deactivate();
	myApp->gui->Jaume_Button->Deactivate();
	myApp->gui->Roger_Button->Deactivate();
	myApp->gui->Adri_Button->Deactivate();
	myApp->gui->Joan_Button->Deactivate();
	myApp->gui->Carles_Button->Deactivate();
	myApp->gui->Rafel_Button->Deactivate();
	myApp->gui->ReturnfromCredits_Button->Deactivate();

}
void LuchoPage() {
	ShellExecuteA(NULL, "open", "https://rafelbrau03.wixsite.com/website-1/about", NULL, NULL, SW_SHOWNORMAL);
}
void RogerPage() {
	ShellExecuteA(NULL, "open", "https://rafelbrau03.wixsite.com/website-1/about-2", NULL, NULL, SW_SHOWNORMAL);
}
void AdriPage() {
	ShellExecuteA(NULL, "open", "https://rafelbrau03.wixsite.com/website-1/about-3", NULL, NULL, SW_SHOWNORMAL);
}
void JaumePage() {
	ShellExecuteA(NULL, "open", "https://rafelbrau03.wixsite.com/website-1/about-4", NULL, NULL, SW_SHOWNORMAL);
}
void JoanPage() {
	ShellExecuteA(NULL, "open", "https://rafelbrau03.wixsite.com/website-1/about-6", NULL, NULL, SW_SHOWNORMAL);
}
void CarlesPage() {
	ShellExecuteA(NULL, "open", "https://rafelbrau03.wixsite.com/website-1/about-5", NULL, NULL, SW_SHOWNORMAL);
}
void RafelPage() {
	ShellExecuteA(NULL, "open", "https://rafelbrau03.wixsite.com/website-1/about-1", NULL, NULL, SW_SHOWNORMAL);
}