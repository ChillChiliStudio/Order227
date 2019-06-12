#include "Brofiler/Brofiler.h"

#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "Render.h"
#include "Textures.h"
#include "Fonts.h"
#include "Input.h"
#include "Scene.h"
#include "Window.h"
#include "Animation.h"
#include "Player.h"
#include "Horde_Manager.h"
#include "GroupManager.h"
#include "Entity_Manager.h"
#include "MiniMap_UI_Element.h"
#include "Controls.h"

#include "UserInterface.h"
#include "UIElement.h"
#include "Image.h"
#include "Text.h"
#include "Button.h"
#include "ActionBox.h"
#include "ParamBox.h"
#include "VoidBox.h"
#include "CheckBox.h"
#include "Spawn_Box.h"
#include "UnitButton.h"
#include "LifeBarBox.h"
#include "ButtonActions.h"
#include "Mouse.h"
#include "Unit_Panel.h"
#include "Buff_Box.h"
#include "TutorialBox.h"
#include "Video.h"

User_Interface::User_Interface() : Module()
{
	name.append("user_interface");
}

// Destructor
User_Interface::~User_Interface()
{}

// Called before render is available
bool User_Interface::Awake(pugi::xml_node& config)
{
	LOG("Loading UI atlas");
	bool ret = true;

	atlasFileName = config.child("atlas").attribute("file").as_string("");

	return ret;
}

Animation User_Interface::loadAnim() {

	Animation ret;
	int x = 0, y = 0;

	for (int i = 0; i < 57; ++i) {
		if (x < 60 * 17) {
			ret.PushBack(SDL_Rect({ x,y,60,48 }));
			x += 60;
		}
		else {
			y += 48;
			x = 0;
		}
	}
	return ret;
}

// Called before the first frame
bool User_Interface::Start()
{
	Current_Screen = Screen_Type::SCREEN_MAINMENU;
	bool ret = true;

	SDL_ShowCursor(SDL_DISABLE);

	loadAnim();

	atlas = myApp->tex->Load(atlasFileName.c_str());
	mainBar = myApp->tex->Load("ui/Principal_Down_Bar.png");

	pauseMenuPanel_Tex = myApp->tex->Load("ui/Pause_Panel.png");;
	selectorinGame_Tex = myApp->tex->Load("ui/inGame_selector_Units.png");
	unitsSelection_Tex = myApp->tex->Load("ui/Troops_Icons.png");
	Timer_Texture = myApp->tex->Load("ui/Timer_Icon.png");
	Main_Menu_Temp_Tex = myApp->tex->Load("ui/Main_Menu.png");
	StartGame_text = myApp->tex->Load("ui/Buttons_And_Slides.png");
	PauseButton_text = myApp->tex->Load("ui/Pause_Buton_Icon.png");
	unitStats_text = myApp->tex->Load("ui/Unit_Stats.png");
	endingImages_Tex = myApp->tex->Load("ui/Ending_Game_Mesage_Icon.png");
	mouse_tex = myApp->tex->Load("ui/Mouse_Actions.png");
	Unit_Panels_tex = myApp->tex->Load("ui/Unit_Costs_Panel.png");
	InGame_Label_tex= myApp->tex->Load("ui/In_Game_Labels.png");
	Buff_tex = myApp->tex->Load("ui/Buff_Texture.png");
	Options_tex = myApp->tex->Load("ui/Options_Text.png");
	Volume_Slides = myApp->tex->Load("ui/VolumeBars_Tex.png");
	Tutorial_Tex = myApp->tex->Load("ui/Tutorial_SpriteSheet.png");

	//Debug Elements
	fpsText = CreateText({ 10, 10 }, "0", font_id::DEFAULT, { 255, 255, 0, 255 });
	fpsText->Deactivate();
	mouseDebugMark = myApp->gui->CreateText({ 0.0f, 0.0f }, "Default Text", font_id::DEFAULT, { 0, 0, 255, 255 });	//TODO: In Release, string explodes sometimes, needs fix
	mouseDebugMark->Deactivate();

	SDL_Rect LoseRect = { 437,112,437,112 };
	SDL_Rect TempButtonRect[4];
	TempButtonRect[0] = { 314,0,290,84 };
	TempButtonRect[1] = { 626,0,290,84 };
	TempButtonRect[2] = { 938,0,290,84 };
	TempButtonRect[3] = {1250,0,290,84 };

	SDL_Rect mini_TempButtonRect[4];
	mini_TempButtonRect[0] = { 0,85,97,82 };
	mini_TempButtonRect[1] = { 0,85,97,82 };
	mini_TempButtonRect[2] = { 97,85,97,82 };
	mini_TempButtonRect[3] = { 194,85,97,82 };

	SDL_Rect Pause_Button[4];
	Pause_Button[0] = { 0,85,168,42 };
	Pause_Button[1] = { 0,0,168,42 };
	Pause_Button[2] = { 0,42,168,42 };
	Pause_Button[3] = { 0,42,168,42 };

	Conscript_Selection_Rect[0] = { 120,0,60,48 };
	Conscript_Selection_Rect[1] = { 120,0,60,48 };
	Conscript_Selection_Rect[2] = { 120,0,60,48 };
	Conscript_Selection_Rect[3] = { 120,0,60,48 };


	Flak_Selection_Rect[0] = { 0,0,60,48 };
	Flak_Selection_Rect[1] = { 0,0,60,48 };
	Flak_Selection_Rect[2] = { 0,0,60,48 };
	Flak_Selection_Rect[3] = { 0,144,60,48 };


	Chrono_Selection_Rect[0] = { 60,0,60,48 };
	Chrono_Selection_Rect[1] = { 60,0,60,48 };
	Chrono_Selection_Rect[2] = { 60,0,60,48 };
	Chrono_Selection_Rect[3] = { 60,144,60,48 };

	Sniper_Selection_Rect[0] = { 60,96,60,48 };
	Sniper_Selection_Rect[1] = { 60,96,60,48 };
	Sniper_Selection_Rect[2] = { 60,96,60,48 };
	Sniper_Selection_Rect[3] = { 60,240,60,48 };

	Desolator_Selection_Rect[0] = { 180,0,60,48 };
	Desolator_Selection_Rect[1] = { 180,0,60,48 };
	Desolator_Selection_Rect[2] = { 180,0,60,48 };
	Desolator_Selection_Rect[3] = { 180,144,60,48 };

	selectorInfantry_Rect[0] = { 131,38,44,31 };
	selectorInfantry_Rect[1] = { 131,38,44,31 };
	selectorInfantry_Rect[2] = { 0,38,44,31 };
	selectorInfantry_Rect[3] = { 44,38,44,31 };

	selectorDefenses_Rect[0] = { 131,69,44,31 };
	selectorDefenses_Rect[1] = { 131,69,44,31 };
	selectorDefenses_Rect[2] = { 0,69,44,31 };
	selectorDefenses_Rect[3] = { 44,69,44,31 };

	selectorTank_Rect[0] = { 131,101,44,31 };
	selectorTank_Rect[1] = { 131,101,44,31 };
	selectorTank_Rect[2] = { 0,101,44,31 };
	selectorTank_Rect[3] = { 44,101,44,31 };

	//selectorInfantry_Rect[3] = { 175,38,44,31 };
	SDL_Color White = { 255,255,255 };
	int height,width;
	myApp->win->GetWindowSize((uint&)width, (uint&)height);

	std::string money = std::to_string(myApp->player->playerMoney);
	horde = std::to_string(myApp->hordes->roundNumber);
	//timerHorde_temp = std::to_string(myApp->hordes->getRoundTimer());

	MainBarPanel = CreateImage(fPoint(width / 2, height - 87.5), SDL_Rect({0,0,1280,175}),mainBar);
	selectorInfantry = CreateSpawnBox(true, fPoint(width / 11-38, height - 140), selectorInfantry_Rect, selectorinGame_Tex);
	selectorDefenses = CreateSpawnBox(false, fPoint(width / 11 , height - 140), selectorDefenses_Rect, selectorinGame_Tex);
	selectorTank = CreateSpawnBox(false, fPoint(width / 11 + 38, height - 140), selectorTank_Rect, selectorinGame_Tex);

	//CREATOR UNITs
	ConscriptCreator = CreateUnitBox(CreateConscript, fPoint(70, height - 95), Conscript_Selection_Rect, unitsSelection_Tex, selectorInfantry,Timer_Texture,10,myApp->entities->infantryStats[(int)infantry_type::CONSCRIPT].cost,nullptr, (SDL_Scancode)myApp->controls->spawnHotKeys[0]);
	ConscriptCreator->Start();
	FlakCreator = CreateUnitBox(CreateFlak, fPoint(130, height - 95), Flak_Selection_Rect, unitsSelection_Tex, selectorInfantry, Timer_Texture, 20, myApp->entities->infantryStats[(int)infantry_type::BAZOOKA].cost,&myApp->entities->heavyUnitsUnlocked, (SDL_Scancode)myApp->controls->spawnHotKeys[1]);
	FlakCreator->Start();
	SniperCreator = CreateUnitBox(CreateSniper, fPoint(130, height - 45), Sniper_Selection_Rect, unitsSelection_Tex, selectorInfantry, Timer_Texture, 20, myApp->entities->infantryStats[(int)infantry_type::SNIPER].cost, nullptr, (SDL_Scancode)myApp->controls->spawnHotKeys[4]);
	SniperCreator->Start();
	ChronoCreator = CreateUnitBox(CreateChrono, fPoint(70, height - 45), Chrono_Selection_Rect, unitsSelection_Tex, selectorInfantry, Timer_Texture, 20, myApp->entities->infantryStats[(int)infantry_type::CHRONO].cost, nullptr, (SDL_Scancode)myApp->controls->spawnHotKeys[3]);
	ChronoCreator->Start();
	DesolatorCreator = CreateUnitBox(CreateDesolator, fPoint(192, height - 95), Desolator_Selection_Rect, unitsSelection_Tex, selectorInfantry, Timer_Texture, 20, myApp->entities->infantryStats[(int)infantry_type::DESOLATOR].cost, &myApp->entities->heavyUnitsUnlocked, (SDL_Scancode)myApp->controls->spawnHotKeys[2]);
	DesolatorCreator->Start();

	//UnitStats = CreateImage(fPoint(width / 1.45, height - 75), SDL_Rect({ 0,0,55,90 }), unitStats_text);
	//UnitFrame = CreateImage(fPoint(width / 1.58, height - 75), SDL_Rect({ 125,5,50,43 }), unitsSelection_Tex);

	Moneytext = CreateText(fPoint(width / 1.55, height - 140),money.c_str(),font_id::MOLOT);

	pauseMenuPanel = CreateImage(fPoint(width / 2, height / 2-100), SDL_Rect({ 0,0,185,355 }), pauseMenuPanel_Tex,true,nullptr,nullptr,Screen_Type::SCREEN_PAUSE);

	ReturnMainMenu = CreateVoidBox(QuitGame, fPoint(width / 2, height / 2), Pause_Button, PauseButton_text, pauseMenuPanel,Screen_Type::SCREEN_PAUSE);
	ReturnMainMenu_Label = CreateText(fPoint(width / 2, height / 2), "EXIT", font_id::MOLOT,White,false,pauseMenuPanel,1.0f,nullptr,Screen_Type::SCREEN_PAUSE);

	Options_Pause_button = CreateVoidBox(OptionsOpen, fPoint(width / 2, height / 2 - 50), Pause_Button, PauseButton_text, pauseMenuPanel, Screen_Type::SCREEN_PAUSE);
	Options_Pause_Label = CreateText(fPoint(width / 2, height / 2 - 50), "Options", font_id::MOLOT, White, false, pauseMenuPanel,1.0f,nullptr, Screen_Type::SCREEN_PAUSE);

	LoadGame_Button = CreateVoidBox(RequestLoad, fPoint(width / 2, height / 2 - 100), Pause_Button, PauseButton_text, pauseMenuPanel, Screen_Type::SCREEN_PAUSE);
	LoadGame_Label = CreateText(fPoint(width / 2, height / 2 - 100), "Load", font_id::MOLOT, White, false, pauseMenuPanel, 1.0f, nullptr, Screen_Type::SCREEN_PAUSE);

	if (!myApp->saveFileExists) {
		LoadGame_Button->Disable();
	}

	SaveGame_Button = CreateVoidBox(RequestSave, fPoint(width / 2, height / 2 - 150), Pause_Button, PauseButton_text, pauseMenuPanel, Screen_Type::SCREEN_PAUSE);
	SaveGame_Label = CreateText(fPoint(width / 2, height / 2 - 150), "Save", font_id::MOLOT, White, false, pauseMenuPanel, 1.0f, nullptr, Screen_Type::SCREEN_PAUSE);

	pauseMenuPanel->Deactivate();
	frameSelector = CreateImage(fPoint(width / 11, height - 140), SDL_Rect({ 0,0,134,38 }), selectorinGame_Tex);

	//--------------

	Horde_label = CreateText(fPoint(width / 2.5, 30), "HORDE  ", font_id::MOLOT, White, false,NULL,1.5);
	hordeNumber_Label = CreateText(fPoint(width / 2, 30),horde.c_str(), font_id::MOLOT, White, false, NULL, 1.5);

	WinIcon = CreateImage(fPoint(width / 2, height / 2.3), SDL_Rect({ 0,112,437,112 }), endingImages_Tex);
	ReturnMainMenu2 = CreateVoidBox(QuitGame, fPoint(width / 2, height / 1.75), Pause_Button, PauseButton_text, WinIcon);
	ReturnMainMenu_Label2 = CreateText(fPoint(width / 2, height / 1.75), "EXIT", font_id::MOLOT, White, false, WinIcon);
	WinIcon->Deactivate();

	LoseIcon = CreateImage(fPoint(width / 2, height / 2.3), SDL_Rect({ 437,112,437,112 }), endingImages_Tex);
	ReturnMainMenu3 = CreateVoidBox(QuitGame, fPoint(width / 2, height / 1.75), Pause_Button, PauseButton_text, LoseIcon);
	ReturnMainMenu_Label3 = CreateText(fPoint(width / 2, height / 1.75), "EXIT", font_id::MOLOT, White, false, LoseIcon);
	LoseIcon->Deactivate();

	incomingHordein = CreateImage(fPoint(width / 7, height / 2.5), SDL_Rect({ 0,66,320,78 }), InGame_Label_tex);
	timerHorde = CreateText(fPoint(width / 3.8f, height / 2.9f), timerHorde_temp.c_str(), font_id::MOLOT, White, false, NULL,2.0f);
	Enemies_left = CreateImage(fPoint(120, 50), SDL_Rect({ 0,0,245,46 }), InGame_Label_tex);
	Eniemies_left_Label = CreateText(fPoint(265, 50), "0", font_id::MOLOT, White, false, NULL, 1.0f);

	ConscriptPanel_Info = CreateUnitPanel(SDL_Rect({ 0,0,137,165 }), ConscriptCreator, Unit_Panels_tex);
	ConscriptPanel_Info->Start();
	SniperPanel_Info = CreateUnitPanel(SDL_Rect({ 137,165,137,165 }), SniperCreator, Unit_Panels_tex);
	DesolatorPanel_Info = CreateUnitPanel(SDL_Rect({ 0,165,137,165 }), DesolatorCreator, Unit_Panels_tex);
	FlakPanel_Info = CreateUnitPanel(SDL_Rect({ 137,0,137,165 }), FlakCreator, Unit_Panels_tex);
	ChronoPanel_Info = CreateUnitPanel(SDL_Rect({ 274,0,137,165 }), ChronoCreator, Unit_Panels_tex);


	Units_Life = CreateBuffBox(fPoint(370, height - 200), SDL_Rect({ 3,3,41,41 }), Buff_tex, &myApp->entities->unitBuff);
	Buildings_Life = CreateBuffBox(fPoint(430, height - 200), SDL_Rect({ 44,0,41,41 }), Buff_tex, &myApp->entities->buildingsBuff);
	HeavyUnits_able = CreateBuffBox(fPoint(480, height - 200), SDL_Rect({ 85,0,41,41 }), Buff_tex, &myApp->entities->heavyUnitsUnlocked);
	Money_Buff = CreateBuffBox(fPoint(530, height - 200), SDL_Rect({ 126,0,47,47 }), Buff_tex, &myApp->entities->incomeBuff1);
	Money2_Buff = CreateBuffBox(fPoint(580, height - 200), SDL_Rect({ 126,0,47,47 }), Buff_tex, &myApp->entities->incomeBuff2);
	Mone3_Buff = CreateBuffBox(fPoint(630, height - 200), SDL_Rect({ 126,0,47,47 }), Buff_tex, &myApp->entities->incomeBuff45);

	UnitBuff_Info = CreateUnitPanel(SDL_Rect({ 147,335,151,94 }), Units_Life, Unit_Panels_tex);
	BuildingBuff_Info = CreateUnitPanel(SDL_Rect({ 0,434,151,94 }), Buildings_Life, Unit_Panels_tex);
	HeavyArmorBuff_Info = CreateUnitPanel(SDL_Rect({ 0,335,151,94 }), HeavyUnits_able, Unit_Panels_tex);
	Money1Buff_Info = CreateUnitPanel(SDL_Rect({ 175,434,151,94 }), Money_Buff, Unit_Panels_tex);
	Money2Buff_Info = CreateUnitPanel(SDL_Rect({ 175,434,151,94 }), Money2_Buff, Unit_Panels_tex);
	Money3Buff_Info = CreateUnitPanel(SDL_Rect({ 0,535,151,94 }), Mone3_Buff, Unit_Panels_tex);

	MainMenuTemp_Image = CreateImage(fPoint(3000, 3000), SDL_Rect({ 0,0,0,0 }), Main_Menu_Temp_Tex, false, nullptr, nullptr, Screen_Type::SCREEN_MAINMENU);

	//MainMenuTemp_Image = CreateImage(fPoint(width / 2, height / 2), SDL_Rect({ 0,0,1280,720 }), Main_Menu_Temp_Tex);
	StartGame_Button = CreateVoidBox(StartGame,fPoint(width/2,height/2.0),TempButtonRect,StartGame_text,nullptr,Screen_Type::SCREEN_MAINMENU);
	StartGame_Label = CreateText(fPoint(width / 2, height / 2.0), "START GAME", font_id::MOLOT,White,false,StartGame_Button,1.0f,nullptr, Screen_Type::SCREEN_MAINMENU);

	ContinueGame_Button = CreateVoidBox(RequestMenuLoad, fPoint(width / 2, height * 0.625), TempButtonRect, StartGame_text, nullptr, Screen_Type::SCREEN_MAINMENU);
	ContinueGame_Label = CreateText(fPoint(width / 2, height * 0.625), "CONTINUE", font_id::MOLOT, White, false, ContinueGame_Button, 1.0f, nullptr, Screen_Type::SCREEN_MAINMENU);

	if (!myApp->saveFileExists) {
		ContinueGame_Button->Disable();
	}

	OptionsGame_Button = CreateVoidBox(OptionsOpen, fPoint(width - width / 2, height * 0.75), TempButtonRect, StartGame_text, nullptr, Screen_Type::SCREEN_MAINMENU);
	OptionsGame_Label = CreateText(fPoint(width - width / 2, height * 0.75), "OPTIONS", font_id::MOLOT, White, false, OptionsGame_Button, 1.0f, nullptr, Screen_Type::SCREEN_MAINMENU);

	ExitGame_Button = CreateVoidBox(CloseGame, fPoint(width - width / 2, height * 0.88), TempButtonRect, StartGame_text, nullptr, Screen_Type::SCREEN_MAINMENU);
	ExitGame_Label = CreateText(fPoint(width - width / 2, height * 0.88), "QUIT GAME", font_id::MOLOT, White, false, ExitGame_Button, 1.0f, nullptr, Screen_Type::SCREEN_MAINMENU);

	Credits_Button = CreateVoidBox(OpenCredits, fPoint(width / 1.33, height * 0.88), TempButtonRect, StartGame_text, nullptr, Screen_Type::SCREEN_MAINMENU);
	Credits_Label = CreateText(fPoint(width / 1.33, height * 0.88), "CREDITS", font_id::MOLOT, White, false, Credits_Button, 1.0f, nullptr, Screen_Type::SCREEN_MAINMENU);


	//CREDITS LINKS
	Lucho_Button = CreateVoidBox(LuchoPage, fPoint(width / 6, height / 2.4), mini_TempButtonRect, StartGame_text, nullptr, Screen_Type::SCREEN_OPTIONS);
	Lucho_Label = CreateText(fPoint(width / 3.47, height / 2.48), "Lucho Suaya", font_id::MOLOT, White, false, Lucho_Button, 1.0f, nullptr, Screen_Type::SCREEN_OPTIONS);
	Lucho_Rol = CreateText(fPoint(width / 4.17, height / 2.2), "Lead", font_id::MOLOT, White, false, Lucho_Button, 0.75f, nullptr, Screen_Type::SCREEN_OPTIONS);
	Lucho_Button->Deactivate();

	Jaume_Button = CreateVoidBox(JaumePage, fPoint(width / 6, height / 1.78), mini_TempButtonRect, StartGame_text, nullptr, Screen_Type::SCREEN_OPTIONS);
	Jaume_Label = CreateText(fPoint(width / 3.37, height / 1.85), "Jaume Avinyo", font_id::MOLOT, White, false, Jaume_Button, 1.0f, nullptr, Screen_Type::SCREEN_OPTIONS);
	Jaume_Rol = CreateText(fPoint(width / 3.77, height / 1.72), "Manager", font_id::MOLOT, White, false, Jaume_Button, 0.75f, nullptr, Screen_Type::SCREEN_OPTIONS);
	Jaume_Button->Deactivate();

	Roger_Button = CreateVoidBox(RogerPage, fPoint(width / 6, height / 1.41), mini_TempButtonRect, StartGame_text, nullptr, Screen_Type::SCREEN_OPTIONS);
	Roger_Label = CreateText(fPoint(width / 3.55, height / 1.46), "Roger Leon", font_id::MOLOT, White, false, Roger_Button, 1.0f, nullptr, Screen_Type::SCREEN_OPTIONS);
	Roger_Rol = CreateText(fPoint(width / 4.40, height / 1.38), "UI", font_id::MOLOT, White, false, Roger_Button, 0.75f, nullptr, Screen_Type::SCREEN_OPTIONS);
	Roger_Button->Deactivate();

	//controlc control v

	Adri_Button = CreateVoidBox(AdriPage, fPoint(width / 2.3, height / 1.78), mini_TempButtonRect, StartGame_text, nullptr, Screen_Type::SCREEN_OPTIONS);
	Adri_Label = CreateText(fPoint(width / 1.78, height / 1.83), "Adrian Font", font_id::MOLOT, White, false, Adri_Button, 1.0f, nullptr, Screen_Type::SCREEN_OPTIONS);
	Adri_Rol = CreateText(fPoint(width / 1.9, height / 1.70), "Artist", font_id::MOLOT, White, false, Adri_Button, 0.75f, nullptr, Screen_Type::SCREEN_OPTIONS);
	Adri_Button->Deactivate();

	Joan_Button = CreateVoidBox(JoanPage, fPoint(width / 1.35, height / 1.78), mini_TempButtonRect, StartGame_text, nullptr, Screen_Type::SCREEN_OPTIONS);
	Joan_Label = CreateText(fPoint(width / 1.17, height / 1.85), "Joan Marin", font_id::MOLOT, White, false, Joan_Button, 1.0f, nullptr, Screen_Type::SCREEN_OPTIONS);
	Joan_Rol = CreateText(fPoint(width / 1.22, height / 1.72), "Code", font_id::MOLOT, White, false, Joan_Button, 0.75f, nullptr, Screen_Type::SCREEN_OPTIONS);
	Joan_Button->Deactivate();

	Carles_Button = CreateVoidBox(CarlesPage, fPoint(width / 1.35, height / 1.41), mini_TempButtonRect, StartGame_text, nullptr, Screen_Type::SCREEN_OPTIONS);
	Carles_Label = CreateText(fPoint(width / 1.16, height / 1.46), "Carles Homs", font_id::MOLOT, White, false, Carles_Button, 1.0f, nullptr, Screen_Type::SCREEN_OPTIONS);
	Carles_Rol = CreateText(fPoint(width / 1.21, height / 1.38), "Design", font_id::MOLOT, White, false, Carles_Button, 0.75f, nullptr, Screen_Type::SCREEN_OPTIONS);
	Carles_Button->Deactivate();

	Rafel_Button = CreateVoidBox(RafelPage, fPoint(width / 2.3, height / 1.41), mini_TempButtonRect, StartGame_text, nullptr, Screen_Type::SCREEN_OPTIONS);
	Rafel_Label = CreateText(fPoint(width / 1.8, height / 1.46), "Rafel Brau", font_id::MOLOT, White, false, Rafel_Button, 1.0f, nullptr, Screen_Type::SCREEN_OPTIONS);
	Rafel_Rol = CreateText(fPoint(width / 1.95, height / 1.38), "Q&A", font_id::MOLOT, White, false, Rafel_Button, 0.75f, nullptr, Screen_Type::SCREEN_OPTIONS);
	Rafel_Button->Deactivate();


	ReturnfromCredits_Button= CreateVoidBox(CloseCredits, fPoint(width / 2, height / 1.15), TempButtonRect, StartGame_text, nullptr, Screen_Type::SCREEN_OPTIONS);
	ReturnfromCredits_Label= CreateText(fPoint(width / 2, height / 1.15), "RETURN", font_id::MOLOT, White, false, ReturnfromCredits_Button, 1.0f, nullptr, Screen_Type::SCREEN_OPTIONS);
	ReturnfromCredits_Button->Deactivate();

	


	Minimap_Display = new  MiniMap_UI();
	Tutorial = new TutorialBox(fPoint(width / 2, height / 2),SDL_Rect({ 0,0,906,657 }),Tutorial_Tex);
	AddElement(Tutorial);
	Tutorial->Deactivate();
	AddElement(Minimap_Display);
	InGame_Elements.push_back(Minimap_Display);

	Tutorial_Button = CreateVoidBox(TutorialOpen, fPoint(width / 4, height * 0.88), TempButtonRect, StartGame_text, nullptr, Screen_Type::SCREEN_MAINMENU);
	Tutorial_Label = CreateText(fPoint(width / 4, height * 0.88), "TUTORIAL", font_id::MOLOT, White, false, Tutorial_Button, 1.0f, nullptr, Screen_Type::SCREEN_MAINMENU);
	Tutorial_Arrow_Foreward = CreateVoidBox(NextPage_Tutorial, fPoint(width / 1.7, height / 1.15), mini_TempButtonRect, StartGame_text, nullptr, Screen_Type::SCREEN_OPTIONS);
	Tutorial_Arrow_Foreward_Label = CreateImage(fPoint(width /1.7, height / 1.15), SDL_Rect({ 1,169,32,22 }), StartGame_text, false,Tutorial_Arrow_Foreward, nullptr, Screen_Type::SCREEN_OPTIONS);
	Tutorial_Arrow_Back = CreateVoidBox(BackPage_Tutorial, fPoint(width / 2.45, height / 1.15), mini_TempButtonRect, StartGame_text, nullptr, Screen_Type::SCREEN_OPTIONS);
	Tutorial_Arrow_Back_Label = CreateImage(fPoint(width / 2.45, height / 1.15), SDL_Rect({ 0,191,32,22 }), StartGame_text, false, Tutorial_Arrow_Back, nullptr, Screen_Type::SCREEN_OPTIONS);

	ReturnfromTutorial_Button = CreateVoidBox(QuitTutorial, fPoint(width / 3.39, height / 1.10), TempButtonRect, StartGame_text, nullptr, Screen_Type::SCREEN_OPTIONS);
	ReturnfromTutorial_Label = CreateText(fPoint(width / 3.8, height / 1.12), "RETURN", font_id::MOLOT, White, false, ReturnfromTutorial_Button, 0.7f, nullptr, Screen_Type::SCREEN_OPTIONS);
	ReturnfromTutorial_Button->ChangeSize(0.6f);
	Tutorial_Arrow_Foreward->Deactivate();
	Tutorial_Arrow_Back->Deactivate();
	ReturnfromTutorial_Button->Deactivate();

	OptionsPanel = CreateImage(fPoint(width / 2, height / 2), SDL_Rect({ 0,0,906,657 }), Options_tex,false,nullptr,nullptr,Screen_Type::SCREEN_OPTIONS);

	VolumeSFX_Slide = CreateImage(fPoint(width / 2, height / 2.2), SDL_Rect({ 0,0,796,80 }), Volume_Slides, false,OptionsPanel, nullptr, Screen_Type::SCREEN_OPTIONS);
	VolumeMusic_Slide = CreateImage(fPoint(width / 2, height / 3.5), SDL_Rect({ 0,0,796,80 }), Volume_Slides, false, OptionsPanel, nullptr, Screen_Type::SCREEN_OPTIONS);
	SetHotkeys_Button = CreateVoidBox(Hotkeys_Options, fPoint(width / 2, height / 1.5), TempButtonRect, StartGame_text, OptionsPanel, Screen_Type::SCREEN_OPTIONS);
	SetHotkeys_Label = CreateText(fPoint(width / 2, height / 1.5), "CONTROLS", font_id::MOLOT, White, false, SetHotkeys_Button,1.0f, nullptr, Screen_Type::SCREEN_OPTIONS);

	Hotkey_Conscript = CreateVoidBox(Hotkeys_Options, fPoint(width / 2.6, height / 3.5), mini_TempButtonRect, StartGame_text, nullptr, Screen_Type::SCREEN_OPTIONS);
	Hotkey_Conscript_Label = CreateText(fPoint(width / 2.6, height / 3.5), "1", font_id::MOLOT, White, false, Hotkey_Conscript, 1.0f, nullptr, Screen_Type::SCREEN_OPTIONS);
	Conscript_Label = CreateText(fPoint(width / 3.5, height / 3.5), "Create Consrcipt", font_id::MOLOT, White, false, Hotkey_Conscript, 0.7f, nullptr, Screen_Type::SCREEN_OPTIONS);
	Hotkey_Conscript->Deactivate();

	Hotkey_Flak = CreateVoidBox(Hotkeys_Options, fPoint(width / 2.6, height / 2.3), mini_TempButtonRect, StartGame_text, nullptr, Screen_Type::SCREEN_OPTIONS);
	Hotkey_Flak_Label = CreateText(fPoint(width / 2.6, height / 2.3), "2", font_id::MOLOT, White, false, Hotkey_Flak, 1.0f, nullptr, Screen_Type::SCREEN_OPTIONS);
	Flak_Label = CreateText(fPoint(width / 3.5, height / 2.3), "Create Flak", font_id::MOLOT, White, false, Hotkey_Flak, 0.7f, nullptr, Screen_Type::SCREEN_OPTIONS);
	Hotkey_Flak->Deactivate();

	Hotkey_Desolator = CreateVoidBox(Hotkeys_Options, fPoint(width / 2.6, height / 1.7), mini_TempButtonRect, StartGame_text, nullptr, Screen_Type::SCREEN_OPTIONS);
	Hotkey_Desolator_Label = CreateText(fPoint(width / 2.6, height / 1.7), "3", font_id::MOLOT, White, false, Hotkey_Desolator, 1.0f, nullptr, Screen_Type::SCREEN_OPTIONS);
	Desolator_Label = CreateText(fPoint(width / 3.5, height / 1.7), "Create Desolator", font_id::MOLOT, White, false, Hotkey_Desolator, 0.7f, nullptr, Screen_Type::SCREEN_OPTIONS);
	Hotkey_Desolator->Deactivate();

	Hotkey_Chrono = CreateVoidBox(Hotkeys_Options, fPoint(width / 2.6, height / 1.35), mini_TempButtonRect, StartGame_text, nullptr, Screen_Type::SCREEN_OPTIONS);
	Hotkey_Chrono_Label = CreateText(fPoint(width / 2.6, height / 1.35), "4", font_id::MOLOT, White, false, Hotkey_Chrono, 1.0f, nullptr, Screen_Type::SCREEN_OPTIONS);
	Chrono_Label = CreateText(fPoint(width / 3.5, height / 1.35), "Create Chrono", font_id::MOLOT, White, false, Hotkey_Chrono, 0.7f, nullptr, Screen_Type::SCREEN_OPTIONS);
	Hotkey_Chrono->Deactivate();

	Hotkey_Sniper = CreateVoidBox(Hotkeys_Options, fPoint(width / 1.5, height / 3.5), mini_TempButtonRect, StartGame_text, nullptr, Screen_Type::SCREEN_OPTIONS);
	Hotkey_Sniper_Label = CreateText(fPoint(width / 1.5, height / 3.5), "5", font_id::MOLOT, White, false, Hotkey_Sniper, 1.0f, nullptr, Screen_Type::SCREEN_OPTIONS);
	Sniper_Label = CreateText(fPoint(width / 1.7, height / 3.5), "Create Sn iper", font_id::MOLOT, White, false, Hotkey_Sniper, 0.7f, nullptr, Screen_Type::SCREEN_OPTIONS);
	Hotkey_Sniper->Deactivate();

	Hotkey_Up = CreateVoidBox(Hotkeys_Options, fPoint(width / 1.5, height / 2.05), mini_TempButtonRect, StartGame_text, nullptr, Screen_Type::SCREEN_OPTIONS);
	Hotkey_Up_Label = CreateText(fPoint(width / 1.5, height / 2.05), "W", font_id::MOLOT, White, false, Hotkey_Up, 1.0f, nullptr, Screen_Type::SCREEN_OPTIONS);
	Hotkey_Up->Deactivate();

	Hotkey_Down = CreateVoidBox(Hotkeys_Options, fPoint(width / 1.5, height / 1.6), mini_TempButtonRect, StartGame_text, nullptr, Screen_Type::SCREEN_OPTIONS);
	Hotkey_Down_Label = CreateText(fPoint(width / 1.5, height / 1.6), "S", font_id::MOLOT, White, false, Hotkey_Down, 1.0f, nullptr, Screen_Type::SCREEN_OPTIONS);
	Hotkey_Down->Deactivate();

	Hotkey_Left = CreateVoidBox(Hotkeys_Options, fPoint(width / 1.72, height / 1.6), mini_TempButtonRect, StartGame_text, nullptr, Screen_Type::SCREEN_OPTIONS);
	Hotkey_Left_Label = CreateText(fPoint(width / 1.72, height / 1.6), "A", font_id::MOLOT, White, false, Hotkey_Left, 1.0f, nullptr, Screen_Type::SCREEN_OPTIONS);
	Hotkey_Left->Deactivate();

	Hotkey_Right = CreateVoidBox(Hotkeys_Options, fPoint(width / 1.32, height / 1.6), mini_TempButtonRect, StartGame_text, nullptr, Screen_Type::SCREEN_OPTIONS);
	Hotkey_Right_Label = CreateText(fPoint(width / 1.32, height / 1.6), "D", font_id::MOLOT, White, false, Hotkey_Right, 1.0f, nullptr, Screen_Type::SCREEN_OPTIONS);
	Hotkey_Right->Deactivate();

	ReturnOptions_Button = CreateVoidBox(QuitOptions, fPoint(width / 1.9, height / 1.135), TempButtonRect, StartGame_text,OptionsPanel, Screen_Type::SCREEN_OPTIONS);
	ReturnOptions_Label = CreateText(fPoint(width / 2, height / 1.15), "RETURN", font_id::MOLOT, White, false, ReturnOptions_Button,0.7f, nullptr, Screen_Type::SCREEN_OPTIONS);
	ReturnOptions_Button->ChangeSize(0.7f);

	OptionsPanel->Deactivate();


	Mouse_UI = CreateMouse(mouse_tex);

	SpawnSelectors.push_back(selectorInfantry);
	SpawnSelectors.push_back(selectorDefenses);
	SpawnSelectors.push_back(selectorTank);

	//for (std::list<UI_Element*>::iterator iter = InGame_Elements.begin(); iter != InGame_Elements.end(); iter = next(iter)) {
	//	if ((*iter)->active == true) {
	//		(*iter)->Deactivate();
	//	}
	//}
	//for (std::list<UI_Element*>::iterator iter = Main_Menu_Elements.begin(); iter != Main_Menu_Elements.end(); iter = next(iter)) {
	//		(*iter)->Activate();
	//		//ret = (*iter)->PreUpdate();
	//}
	//Main_Menu_Elements.push_back(MainMenuTemp_Image);
	//Main_Menu_Elements.push_back(StartGame_Button);
	//Main_Menu_Elements.push_back(StartGame_Label);

	myApp->video->PlayVideo("Video/Main_Menu_Background.ogv", { 0,0,1280,720 },true);

	//DeactivateScreen(Main_Menu_Elements);
	DeactivateScreen(InGame_Elements);
	//ActivateScreen(Main_Menu_Elements);

	return ret;
}

void User_Interface::ActivateScreen(std::list<UI_Element*> list) {

	for (std::list<UI_Element*>::iterator iter = list.begin(); iter != list.end(); iter = next(iter)) {
		if ((*iter)->active == false&&(*iter)->mustDestroy==false) {
			if((*iter)!=pauseMenuPanel)
				(*iter)->Activate();
		}
	}
}

void User_Interface::DeactivateScreen(std::list<UI_Element*> list) {

	for (std::list<UI_Element*>::iterator iter = list.begin(); iter != list.end(); iter = next(iter)) {
		if ((*iter)->active == true && (*iter)->mustDestroy == false) {
			(*iter)->Deactivate();
		}
	}
}
// Called each loop iteration
bool User_Interface::PreUpdate()
{
	BROFILER_CATEGORY("Module User_Interface Pre-Update", Profiler::Color::Pink);
	//myApp->video->PlayVideo("Video/Main_Menu_Background.ogv", { 0,-50,1280,850 });
	bool ret = true;

	for (std::list<UI_Element*>::iterator iter = screenElements.begin(); iter != screenElements.end(); iter = next(iter)) {
		if ((*iter)->active == true) {
			ret = (*iter)->PreUpdate();
		}
	}

	return ret;
}

// Called each frame (logic)
bool User_Interface::Update(float dt)
{
	BROFILER_CATEGORY("Module User_Interface Update", Profiler::Color::DeepPink);

	bool ret = true;

	//TODO LUCHO VARIABLE
	if (myApp->hordes->HordesDead() == false) {
		Eniemies_left_Label->ChangeString(std::to_string(myApp->hordes->remainingEnemies));
	}

	//if (myApp->groups->playerGroup.groupUnits.size() > 0) {

	//	UnitStats->Activate();
	//	UnitFrame->Activate();
	//}
	//else {

	//	UnitStats->Deactivate();
	//	UnitFrame->Deactivate();
	//}
	if (myApp->hordes->HordesDead()&&Current_Screen!=Screen_Type::SCREEN_MAINMENU && myApp->hordes->roundNumber != myApp->hordes->maxHordes) {

		incomingHordein->Activate();
		timerHorde->Activate();
		timerHorde->ChangeString(std::to_string(myApp->hordes->getRoundTimer()));
	}
	else {

		timerHorde->Deactivate();
		incomingHordein->Deactivate();
	}
	//if (unitCreationCD.ReadSec() >= 0.7) {
	//	myApp->player->startCreationUnit = false;
	//}

	for (std::list<UI_Element*>::iterator iter = screenElements.begin(); iter != screenElements.end(); iter = next(iter)) {
		if ((*iter)->active == true) {
			ret = (*iter)->Update(dt);
		}
	}

	return ret;
}

// Called each frame (graphic)
bool User_Interface::Draw()
{
	BROFILER_CATEGORY("Module UserInterface Draw", Profiler::Color::HotPink);

	bool ret = true;

	for (std::list<UI_Element*>::iterator iter = screenElements.begin(); iter != screenElements.end(); iter = next(iter)) {
		if ((*iter)->active == true && (*iter)->GetParent() == NULL) {	// All elements are listed, but the parent handles the drawing for its children
			if ((*iter)->GetType() != ui_type::LIFEBAR)
				ret = (*iter)->Draw();
		}
	}

	if (interfaceDebugDraw) {
		for (std::list<UI_Element*>::iterator iter = screenElements.begin(); iter != screenElements.end(); iter = next(iter)) {
			if ((*iter)->active == true && (*iter)->GetParent() == NULL) {	// All elements must have debug blitting AFTER standard blitting, no "blitting layers"
				ret = (*iter)->DebugDraw();
			}
		}
	}

	return true;
}

// Called each loop iteration
bool User_Interface::PostUpdate()
{
	BROFILER_CATEGORY("Module User_Interface Post-Update", Profiler::Color::LightPink);

	bool ret = true;

	std::list<UI_Element*>::iterator iter = screenElements.begin();

	while (iter != screenElements.end()) {
		if ((*iter)->mustDestroy) {
			iter = DestroyElement(iter);
		}
		else {
			ret = (*iter)->PostUpdate();
			iter = next(iter);
		}
	}

	return ret;
}

// Called before quitting
bool User_Interface::CleanUp()
{
	bool ret = true;

	for (std::list<UI_Element*>::iterator iter = screenElements.begin(); iter != screenElements.end(); iter = next(iter)) {
		ret = (*iter)->CleanUp();
		RELEASE((*iter));
	}
	screenElements.clear();

	return ret;
}

// const getter for atlas
SDL_Texture* User_Interface::GetAtlas() const
{
	return atlas;
}

void User_Interface::AddElement(UI_Element* element)
{
	screenElements.push_back(element);
}

std::list<UI_Element*>::iterator User_Interface::DestroyElement(std::list<UI_Element*>::iterator iter)	// Deletion by list location
{
	(*iter)->CleanUp();
	RELEASE((*iter));
	return screenElements.erase(iter);
}

void User_Interface::DestroyElement(UI_Element* element)	// Deletion by list content comparison
{
	if (element != nullptr) {
		element->CleanUp();
		screenElements.remove(element);	//WARNING: Check if the order is correct, this could lead to problems otherwise
		RELEASE(element);
	}
}

//Factories
Image* User_Interface::CreateImage(fPoint center, SDL_Rect texRect, SDL_Texture* tex, bool dynamic, UI_Element* parent, std::list<UI_Element*>* children, Screen_Type screen )
{
	Image* ret = nullptr;

	if (tex == NULL) {
		tex = GetAtlas();
	}
	else if (SDL_RectEmpty(&texRect)) {
		SDL_Rect tmpRect = { 0, 0, 0, 0 };
		myApp->tex->GetSize(tex, (uint&)tmpRect.w, (uint&)tmpRect.h);
		texRect = tmpRect;
	}

	ret = new Image(ui_type::IMAGE, center, texRect, tex, dynamic, parent, children);
	AddElement((UI_Element*)ret);

	if (screen == Screen_Type::SCREEN_NONE) {
		InGame_Elements.push_back(ret);
	}
	else if (screen == Screen_Type::SCREEN_MAINMENU)
		Main_Menu_Elements.push_back(ret);

	return ret;
}

LifeBar* User_Interface::CreateLifeBar(fPoint center, Unit* parent, SDL_Texture* tex, float* auxHealth, Screen_Type screen) {

	LifeBar* ret = nullptr;
	if (tex == NULL) {
		tex = GetAtlas();
	}

	ret = new LifeBar(center, parent, tex, ui_type::LIFEBAR,auxHealth);
	AddElement((UI_Element*)ret);


	if (screen == Screen_Type::SCREEN_NONE) {
		InGame_Elements.push_back(ret);
	}
	else if (screen == Screen_Type::SCREEN_MAINMENU)
		Main_Menu_Elements.push_back(ret);

	return ret;
}

Text* User_Interface::CreateText(fPoint center, const char* content, font_id id, SDL_Color color, bool dynamic, UI_Element* parent, float size, std::list<UI_Element*>* children, Screen_Type screen )
{
	Text* ret = nullptr;
	_TTF_Font* tmpFont;

	if (id >= font_id::MAX_FONTS) {
		tmpFont = myApp->fonts->defaultFont;
	}
	else {
		tmpFont = myApp->fonts->fontsList[(int)id]->fontPtr;
	}

	ret = new Text(content, color, tmpFont, center, dynamic, parent, children,size);
	AddElement((UI_Element*)ret);


	if (screen == Screen_Type::SCREEN_NONE) {
		InGame_Elements.push_back(ret);
	}
	else if( screen ==Screen_Type::SCREEN_MAINMENU)
		Main_Menu_Elements.push_back(ret);

	return ret;
}

Unit_Box* User_Interface::CreateUnitBox(void(*action)(void), fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex, UI_Element* parent, SDL_Texture* TimerTexture, int timeCreator,int unitCost,bool* _enabletoCraft, SDL_Scancode Hotkey, Screen_Type screen ) {

	Unit_Box* ret = nullptr;

	if (tex == NULL) {
		tex = GetAtlas();
	}

	ret = new Unit_Box(action, center, spriteList, tex, parent,TimerTexture,timeCreator,unitCost,_enabletoCraft,Hotkey);
	AddElement(ret);

	if (screen == Screen_Type::SCREEN_NONE) {
		InGame_Elements.push_back(ret);
	}
	else if (screen == Screen_Type::SCREEN_MAINMENU)
		Main_Menu_Elements.push_back(ret);

	return ret;
}

Unit_Panel* User_Interface::CreateUnitPanel(SDL_Rect sprite, Image* button , SDL_Texture* tex, Screen_Type screen ) {

	Unit_Panel* ret = nullptr;

	if (tex == NULL) {
		tex = GetAtlas();
	}

	ret = new Unit_Panel(sprite,button,tex);
	AddElement(ret);

	if (screen == Screen_Type::SCREEN_NONE) {
		InGame_Elements.push_back(ret);
	}
	else if (screen == Screen_Type::SCREEN_MAINMENU)
		Main_Menu_Elements.push_back(ret);

	return ret;
}

Buff_Box* User_Interface::CreateBuffBox(fPoint position, SDL_Rect rect, SDL_Texture* tex, bool* able, Screen_Type screen ) {

	Buff_Box* ret = nullptr;

	if (tex == NULL) {
		tex = GetAtlas();
	}

	ret = new Buff_Box(position,rect,tex,able);
	AddElement(ret);

	if (screen == Screen_Type::SCREEN_NONE) {
		InGame_Elements.push_back(ret);
	}
	else if (screen == Screen_Type::SCREEN_MAINMENU)
		Main_Menu_Elements.push_back(ret);

	return ret;
}

Mouse* User_Interface::CreateMouse(SDL_Texture*tex ) {
	Mouse* ret = nullptr;
	ret = new Mouse(tex);
	AddElement(ret);
	return ret;
}

Void_Box* User_Interface::CreateVoidBox(void(*action)(void), fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex, UI_Element* parent, Screen_Type screen)
{
	Void_Box* ret = nullptr;

	if (tex == NULL) {
		tex = GetAtlas();
	}

	ret = new Void_Box(action, center, spriteList, tex, parent);
	AddElement(ret);

	if (screen == Screen_Type::SCREEN_NONE) {
		InGame_Elements.push_back(ret);
	}
	else if (screen == Screen_Type::SCREEN_MAINMENU)
		Main_Menu_Elements.push_back(ret);

	return ret;
}

Check_Box* User_Interface::CreateCheckBox(bool* value, fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex, void(*action)(void), UI_Element* parent, Screen_Type screen)
{
	Check_Box* ret = nullptr;

	if (tex == NULL) {
		tex = GetAtlas();
	}


	ret = new Check_Box(value, action, center, spriteList, tex, parent);
	AddElement(ret);

	if (screen == Screen_Type::SCREEN_NONE) {
		InGame_Elements.push_back(ret);
	}
	else if (screen == Screen_Type::SCREEN_MAINMENU)
		Main_Menu_Elements.push_back(ret);

	return ret;
}


Spawn_Box* User_Interface::CreateSpawnBox(bool value, fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex, void(*action)(void), UI_Element* parent, Screen_Type screen)
{
	Spawn_Box* ret = nullptr;

	if (tex == NULL) {
		tex = GetAtlas();
	}

	ret = new Spawn_Box(value, action, center, spriteList, tex, parent);
	AddElement(ret);

	if (screen == Screen_Type::SCREEN_NONE) {
		InGame_Elements.push_back(ret);
	}
	else if (screen == Screen_Type::SCREEN_MAINMENU)
		Main_Menu_Elements.push_back(ret);

	return ret;
}

//UI_Element* User_Interface::CreateActionBox(void(*action)(void), fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex, bool dynamic, UI_Element* parent, std::list<UI_Element*>* children)
//{
//	UI_Element* ret = nullptr;
//
//	if (tex == NULL) {
//		tex = GetAtlas();
//	}
//
//	ret = new ActionBox<void>(action, center, spriteList, tex, dynamic, parent, children);
//	AddElement(ret);
//
//	return ret;
//}
//
//UI_Element* User_Interface::CreateCheckBox(void(*action)(bool*), bool* value, fPoint center, SDL_Rect spriteList[3], SDL_Texture* tex, bool dynamic, UI_Element* parent, List<UI_Element*>* children)
//{
//	UI_Element* ret = nullptr;
//
//	if (tex == NULL) {
//		tex = GetAtlas();
//	}
//
//	ret = new CheckBox(action, value, center, spriteList, tex, dynamic, parent, children);
//	AddElement(ret);
//
//	return ret;
//}
