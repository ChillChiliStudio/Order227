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
#include "ButtonActions.h"


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

void User_Interface::loadAnim() {

	int x = 0, y = 0;

	for (int i = 0; i < 57; ++i) {
		if (x < 60 * 17) {
			Timer_anim.PushBack(SDL_Rect({ x,y,60,48 }));
			x += 60;
		}
		else {
			y += 48;
			x = 0;
		}
	}
}

// Called before the first frame
bool User_Interface::Start()
{
	bool ret = true;

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

	SDL_Rect TempButtonRect[4];
	TempButtonRect[0] = {626,0,290,84};
	TempButtonRect[1] = { 626,0,290,84 };
	TempButtonRect[2] = { 938,0,290,84 };
	TempButtonRect[3] = {1250,0,290,84 };

	SDL_Rect Pause_Button[4];
	Pause_Button[0] = { 0,0,168,42 };
	Pause_Button[1] = { 0,0,168,42 };
	Pause_Button[2] = { 0,42,168,42 };
	Pause_Button[3] = { 0,42,168,42 };


	Conscript_Selection_Rect[0] = { 120,0,60,48 };
	Conscript_Selection_Rect[1] = { 120,0,60,48 };
	Conscript_Selection_Rect[2] = { 120,0,60,48 };
	Conscript_Selection_Rect[3] = { 120,0,60,48 };


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

	MainBarPanel = CreateImage(fPoint(width / 2, height - 87.5), SDL_Rect({0,0,1280,175}),mainBar);
	selectorInfantry = CreateSpawnBox(true, fPoint(width / 11-38, height - 140), selectorInfantry_Rect, selectorinGame_Tex);
	selectorDefenses = CreateSpawnBox(false, fPoint(width / 11 , height - 140), selectorDefenses_Rect, selectorinGame_Tex);
	selectorTank = CreateSpawnBox(false, fPoint(width / 11 + 38, height - 140), selectorTank_Rect, selectorinGame_Tex);
	ConscriptCreator = CreateUnitBox(CreateConscript, fPoint(70, height - 95), Conscript_Selection_Rect, unitsSelection_Tex, selectorInfantry,Timer_Texture,10);

	UnitStats = CreateImage(fPoint(width / 1.45, height - 75), SDL_Rect({ 0,0,55,90 }), unitStats_text);
	UnitFrame = CreateImage(fPoint(width / 1.58, height - 75), SDL_Rect({ 125,5,50,43 }), unitsSelection_Tex);

	Moneytext = CreateText(fPoint(width / 1.55, height - 140),money.c_str(),font_id::MOLOT);
	
	pauseMenuPanel = CreateImage(fPoint(width / 2, height / 2-100), SDL_Rect({ 0,0,185,355 }), pauseMenuPanel_Tex,true);
	ReturnMainMenu = CreateVoidBox(QuitGame, fPoint(width / 2, height / 2), Pause_Button, PauseButton_text, pauseMenuPanel);
	ReturnMainMenu_Label = CreateText(fPoint(width / 2, height / 2), "EXIT", font_id::MOLOT,White,false,pauseMenuPanel);
	pauseMenuPanel->Deactivate();
	frameSelector = CreateImage(fPoint(width / 11, height - 140), SDL_Rect({ 0,0,134,38 }), selectorinGame_Tex);



	MainMenuTemp_Image = CreateImage(fPoint(width / 2, height / 2), SDL_Rect({ 0,0,1280,720 }), Main_Menu_Temp_Tex);
	StartGame_Button = CreateVoidBox(StartGame,fPoint(width/2,height/1.8),TempButtonRect,StartGame_text,MainMenuTemp_Image);
	StartGame_Label = CreateText(fPoint(width / 2, height / 1.8), "START GAME", font_id::MOLOT,White,false,StartGame_Button);

	ExitGame_Button = CreateVoidBox(CloseGame, fPoint(width / 2, height / 1.2), TempButtonRect, StartGame_text, MainMenuTemp_Image);
	ExitGame_Label = CreateText(fPoint(width / 2, height / 1.2), "QUIT GAME", font_id::MOLOT, White, false, ExitGame_Button);
	


	SpawnSelectors.push_back(selectorInfantry);
	SpawnSelectors.push_back(selectorDefenses);
	SpawnSelectors.push_back(selectorTank);
	Main_Menu_Elements.push_back(MainMenuTemp_Image);
	Main_Menu_Elements.push_back(StartGame_Button);
	Main_Menu_Elements.push_back(StartGame_Label);

	return ret;
}

// Called each loop iteration
bool User_Interface::PreUpdate()
{
	BROFILER_CATEGORY("Module User_Interface PreUpdate", Profiler::Color::Pink);

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
	BROFILER_CATEGORY("Module User_Interface UpdateTick", Profiler::Color::DeepPink);
	
	bool ret = true;

	if (unitCreationCD.ReadSec() >= 10) {
		myApp->player->startCreationUnit = false;
	}

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
	BROFILER_CATEGORY("Module User_Interface Update", Profiler::Color::HotPink);

	bool ret = true;

	for (std::list<UI_Element*>::iterator iter = screenElements.begin(); iter != screenElements.end(); iter = next(iter)) {
		if ((*iter)->active == true && (*iter)->GetParent() == NULL) {	// All elements are listed, but the parent handles the drawing for its children
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
	BROFILER_CATEGORY("Module User_Interface PostUpdate", Profiler::Color::LightPink);

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
	element->CleanUp();
	screenElements.remove(element);	//WARNING: Check if the order is correct, this could lead to problems otherwise
	RELEASE(element);
}

//Factories
Image* User_Interface::CreateImage(fPoint center, SDL_Rect texRect, SDL_Texture* tex, bool dynamic, UI_Element* parent, std::list<UI_Element*>* children)
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

	return ret;
}

Text* User_Interface::CreateText(fPoint center, const char* content, font_id id, SDL_Color color, bool dynamic, UI_Element* parent, std::list<UI_Element*>* children)
{
	Text* ret = nullptr;
	_TTF_Font* tmpFont;

	if (id >= font_id::MAX_FONTS) {
		tmpFont = myApp->fonts->defaultFont;
	}
	else {
		tmpFont = myApp->fonts->fontsList[(int)id]->fontPtr;
	}

	ret = new Text(content, color, tmpFont, center, dynamic, parent, children);
	AddElement((UI_Element*)ret);

	return ret;
}
Unit_Box* User_Interface::CreateUnitBox(void(*action)(void), fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex, UI_Element* parent, SDL_Texture* TimerTexture, int timeCreator) {
	
	Unit_Box* ret = nullptr;

	if (tex == NULL) {
		tex = GetAtlas();
	}

	ret = new Unit_Box(action, center, spriteList, tex, parent,TimerTexture,timeCreator);
	AddElement(ret);

	return ret;
}

Void_Box* User_Interface::CreateVoidBox(void(*action)(void), fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex, UI_Element* parent)
{
	Void_Box* ret = nullptr;
	
	if (tex == NULL) {
		tex = GetAtlas();
	}

	ret = new Void_Box(action, center, spriteList, tex, parent);
	AddElement(ret);

	return ret;
}

Check_Box* User_Interface::CreateCheckBox(bool* value, fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex, void(*action)(void), UI_Element* parent)
{
	Check_Box* ret = nullptr;

	if (tex == NULL) {
		tex = GetAtlas();
	}

	ret = new Check_Box(value, action, center, spriteList, tex, parent);
	AddElement(ret);

	return ret;
}


Spawn_Box* User_Interface::CreateSpawnBox(bool value, fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex, void(*action)(void), UI_Element* parent)
{
	Spawn_Box* ret = nullptr;

	if (tex == NULL) {
		tex = GetAtlas();
	}

	ret = new Spawn_Box(value, action, center, spriteList, tex, parent);
	AddElement(ret);

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