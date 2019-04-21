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

#include "UserInterface.h"
#include "UIElement.h"
#include "Image.h"
#include "Text.h"
#include "Button.h"
#include "ActionBox.h"
#include "ParamBox.h"
#include "VoidBox.h"
#include "CheckBox.h"

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

// Called before the first frame
bool User_Interface::Start()
{
	bool ret = true;

	atlas = myApp->tex->Load(atlasFileName.c_str());
	mainBar = myApp->tex->Load("ui/Principal_Down_Bar.png");

	pauseMenuPanel_Tex= myApp->tex->Load("ui/Pause_Panel.png");;
	selectorinGame_Tex = myApp->tex->Load("ui/inGame_selector_Units.png");

	int height,width;
	myApp->win->GetWindowSize((uint&)width, (uint&)height);

	MainBarPanel = CreateImage(fPoint(width / 2, height - 87.5), SDL_Rect({0,0,1280,175}),mainBar);
	pauseMenuPanel = CreateImage(fPoint(width / 2, height / 2-100), SDL_Rect({ 0,0,185,355 }), pauseMenuPanel_Tex);

	frameSelector = CreateImage(fPoint(width / 11, height - 140), SDL_Rect({ 0,0,120,38 }), selectorinGame_Tex);


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

	if (mustDebugDraw) {
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
UI_Element* User_Interface::CreateImage(fPoint center, SDL_Rect texRect, SDL_Texture* tex, bool dynamic, UI_Element* parent, std::list<UI_Element*>* children)
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

	return (UI_Element*)ret;
}

UI_Element* User_Interface::CreateText(fPoint center, const char* content, font_id id, SDL_Color color, bool dynamic, UI_Element* parent, std::list<UI_Element*>* children)
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

	return (UI_Element*)ret;
}

UI_Element* User_Interface::CreateVoidBox(void(*action)(void), fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex, UI_Element* parent)
{
	Void_Box* ret = nullptr;
	
	if (tex == NULL) {
		tex = GetAtlas();
	}

	ret = new Void_Box(action, center, spriteList, tex, parent);
	AddElement(ret);

	return (UI_Element*)ret;
}

UI_Element* User_Interface::CreateCheckBox(bool* value, fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex, void(*action)(void), UI_Element* parent)
{
	Check_Box* ret = nullptr;

	if (tex == NULL) {
		tex = GetAtlas();
	}

	ret = new Check_Box(value, action, center, spriteList, tex, parent);
	AddElement(ret);

	return (UI_Element*)ret;
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