#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H	//@Carles

#include "Module.h"
#include "Animation.h"
#include "Timer.h"
#include "SDL/include/SDL_scancode.h"

#define CURSOR_WIDTH 2

enum class ui_type;
enum class font_id;
class UI_Element;
class Image;
class Text;
class Void_Box;
class Check_Box;
class Spawn_Box;
class Unit_Box;
class Button;
class LifeBar;
class Unit;
class Mouse;
class Unit_Panel;
class MiniMap_UI;
class Buff_Box;
class TutorialBox;
//class Window;
struct _TTF_Font;
struct SDL_Rect;
struct SDL_Texture;


enum class Screen_Type {

	SCREEN_NONE = -1,
	SCREEN_MAINMENU,
	SCREEN_INGAME,
	SCREEN_PAUSE,
	SCREEN_OPTIONS,
	SCREEN_WIN,
	SCREEN_LOSE,
	SCREEN_MAX
};


class User_Interface : public Module
{
public:

	User_Interface();

	// Destructor
	virtual ~User_Interface();

	// Called when before render is available
	bool Awake(pugi::xml_node&);

	// Call before first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each frame (logic)
	bool Update(float dt);

	// Called each frame (graphic)
	bool Draw();

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();
	
public:
	SDL_Texture* GetAtlas() const;
	
	void AddElement(UI_Element* element);
	void DestroyElement(UI_Element* element);
	void ActivateScreen(std::list<UI_Element*> list);
	void DeactivateScreen(std::list<UI_Element*> list);
	void SetUp_Hotkey_Icons() {};

	Animation loadAnim();

	std::list<UI_Element*>::iterator DestroyElement(std::list<UI_Element*>::iterator iter);

	//TODO: Factories should return their specific class type
	Image* CreateImage(fPoint center, SDL_Rect texRect = { 0, 0, 0, 0 }, SDL_Texture* tex = NULL, bool dynamic = false, UI_Element* parent = NULL, std::list<UI_Element*>* children = NULL,Screen_Type screen =Screen_Type::SCREEN_NONE);
	Text* CreateText(fPoint center, const char* content, font_id id, SDL_Color color = { 255, 255, 255, 255 }, bool dynamic = false, UI_Element* parent = NULL,float size=1.0f, std::list<UI_Element*>* children = NULL, Screen_Type screen = Screen_Type::SCREEN_NONE);
	//UI_Element* CreateInputText();
	Void_Box* CreateVoidBox(void(*action)(void), fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex = NULL, UI_Element* parent = NULL, Screen_Type screen = Screen_Type::SCREEN_NONE);
	Check_Box* CreateCheckBox(bool* value, fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex = NULL, void(*action)(void) = NULL, UI_Element* parent = NULL, Screen_Type screen = Screen_Type::SCREEN_NONE);
	Spawn_Box* CreateSpawnBox(bool value, fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex = NULL, void(*action)(void) = NULL, UI_Element* parent = NULL, Screen_Type screen = Screen_Type::SCREEN_NONE);
	Unit_Box* CreateUnitBox(void(*action)(void), fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex = NULL, UI_Element* parent = NULL, SDL_Texture* TimerTexture = NULL, int timeCreator = 0,int unitCost=0,bool* _enabletoCraft=nullptr,SDL_Scancode Hotkey = SDL_SCANCODE_0, Screen_Type screen = Screen_Type::SCREEN_NONE);
	LifeBar* CreateLifeBar(fPoint center, Unit* parent = nullptr, SDL_Texture* tex = NULL, float* auxHealth=NULL, Screen_Type screen = Screen_Type::SCREEN_NONE);
	Mouse* CreateMouse( SDL_Texture*tex);
	Unit_Panel* CreateUnitPanel(SDL_Rect sprite, Image* button = nullptr, SDL_Texture* tex = NULL, Screen_Type screen = Screen_Type::SCREEN_NONE);
	Buff_Box* CreateBuffBox(fPoint position, SDL_Rect rect, SDL_Texture* tex = NULL, bool* able = nullptr, Screen_Type screen = Screen_Type::SCREEN_NONE);

	template <class T_param>
	UI_Element* CreateParamBox(void(*action)(T_param), T_param parameter, fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex = NULL, UI_Element* parent = NULL)
	{
		Param_Box<T_param>* ret = nullptr;

		if (tex == NULL) {
			tex = GetAtlas();
		}

		ret = GenerateParamBox<T_param>(action, parameter, center, spriteList, tex, parent);
		AddElement(ret);

		return (UI_Element*)ret;
	}

	//Window* CreateWindowPanel(fPoint center, p2List<Image*> children, SDL_Rect* texRect = NULL, SDL_Texture* tex = NULL, Text* label = NULL, UI_Element* parent = NULL);
	
public:

	Screen_Type Current_Screen = Screen_Type::SCREEN_MAINMENU;

	bool interfaceDebugDraw = false;
	bool OnPause = false;

	Text* fpsText = nullptr;
	Text* mouseDebugMark = nullptr;

	std::string horde;
	std::string timerHorde_temp;
	//uint defaultScale;	//IMPROVE: Future use
	//Animation Timer_anim;
	TutorialBox* Tutorial = nullptr;
	Buff_Box* Units_Life = nullptr;
	Buff_Box* Buildings_Life = nullptr;
	Buff_Box* HeavyUnits_able = nullptr;
	Buff_Box* Money_Buff = nullptr;
	Buff_Box* Money2_Buff = nullptr;
	Buff_Box* Mone3_Buff = nullptr;

	Void_Box* SetHotkeys_Button = nullptr;
	Text* SetHotkeys_Label = nullptr;
	Image* OptionsPanel = nullptr;
	Image* pauseMenuPanel = nullptr;
	Text* Moneytext = nullptr;
	Image* MainMenuTemp_Image = nullptr;
	Void_Box* StartGame_Button = nullptr;
	Text* StartGame_Label = nullptr;
	Void_Box* ContinueGame_Button = nullptr;
	Text* ContinueGame_Label = nullptr;
	Void_Box* LoadGame_Button = nullptr;
	Text* LoadGame_Label = nullptr;
	Void_Box* SaveGame_Button = nullptr;
	Text* SaveGame_Label = nullptr;
	Void_Box* OptionsGame_Button = nullptr;
	Text* OptionsGame_Label = nullptr;
	Void_Box* ExitGame_Button = nullptr;
	Text* ExitGame_Label = nullptr;
	Void_Box* ReturnOptions_Button = nullptr;
	Text* ReturnOptions_Label = nullptr;
	Image* VolumeSFX_Slide = nullptr;
	Image* VolumeMusic_Slide = nullptr;
	Void_Box* Tutorial_Button = nullptr;
	Text* Tutorial_Label = nullptr;
	Void_Box*Tutorial_Arrow_Foreward = nullptr;
	Image* Tutorial_Arrow_Foreward_Label = nullptr;
	Void_Box*Tutorial_Arrow_Back = nullptr;
	Image* Tutorial_Arrow_Back_Label = nullptr;
	Void_Box* ReturnfromTutorial_Button = nullptr;
	Text* ReturnfromTutorial_Label = nullptr;

	Text* Damage_Label = nullptr;
	Text* Health_Label = nullptr;
	Text* Speed_Label = nullptr;
	Text* Cadency_Label = nullptr;

	Text* Horde_label = nullptr;
	Text* hordeNumber_Label = nullptr;
	Image* incomingHordein = nullptr;
	Image* Enemies_left = nullptr;
	Text* timerHorde = nullptr;
	Text* Eniemies_left_Label;
	MiniMap_UI* Minimap_Display = nullptr;

	Image* WinIcon = nullptr;
	Image* LoseIcon = nullptr;
	std::list<Spawn_Box*> SpawnSelectors;
	std::list<UI_Element*> Main_Menu_Elements;
	std::list<UI_Element*> InGame_Elements;


	Void_Box* Options_Pause_button= nullptr;
	Text* Options_Pause_Label = nullptr;

	Void_Box* ReturnMainMenu2 = nullptr;
	Text* ReturnMainMenu_Label2 = nullptr;

	Void_Box* ReturnMainMenu3 = nullptr;
	Text* ReturnMainMenu_Label3 = nullptr;

	Mouse* Mouse_UI = nullptr;

	Unit_Panel* ConscriptPanel_Info = nullptr;
	Unit_Panel* SniperPanel_Info = nullptr;
	Unit_Panel* ChronoPanel_Info = nullptr;
	Unit_Panel* DesolatorPanel_Info = nullptr;
	Unit_Panel* FlakPanel_Info = nullptr;

	Unit_Panel* UnitBuff_Info = nullptr;
	Unit_Panel* BuildingBuff_Info = nullptr;
	Unit_Panel* HeavyArmorBuff_Info = nullptr;
	Unit_Panel* Money1Buff_Info = nullptr;
	Unit_Panel* Money2Buff_Info = nullptr;
	Unit_Panel* Money3Buff_Info = nullptr;

	Void_Box* Hotkey_Conscript = nullptr;
	Text* Hotkey_Conscript_Label = nullptr;
	Text* Conscript_Label = nullptr;

	Void_Box* Hotkey_Flak = nullptr;
	Text* Hotkey_Flak_Label = nullptr;
	Text* Flak_Label = nullptr;

	Void_Box* Hotkey_Desolator = nullptr;
	Text* Hotkey_Desolator_Label = nullptr;
	Text* Desolator_Label = nullptr;

	Void_Box* Hotkey_Chrono = nullptr;
	Text* Hotkey_Chrono_Label = nullptr;
	Text* Chrono_Label = nullptr;

	Void_Box* Hotkey_Sniper = nullptr;
	Text* Hotkey_Sniper_Label = nullptr;
	Text* Sniper_Label = nullptr;

	Void_Box* Hotkey_Up = nullptr;
	Text* Hotkey_Up_Label = nullptr;
	Void_Box* Hotkey_Down = nullptr;
	Text* Hotkey_Down_Label = nullptr;
	Void_Box* Hotkey_Left = nullptr;
	Text* Hotkey_Left_Label = nullptr;
	Void_Box* Hotkey_Right = nullptr;
	Text* Hotkey_Right_Label = nullptr;

	Unit_Box* ConscriptCreator = nullptr;
	Unit_Box* FlakCreator = nullptr;
	Unit_Box* SniperCreator = nullptr;
	Unit_Box* ChronoCreator = nullptr;
	Unit_Box* DesolatorCreator = nullptr;

private:

	std::list<UI_Element*> screenElements;


	SDL_Texture* Timer_Texture = nullptr;

	Image* MainBarPanel = nullptr;

	Void_Box* ReturnMainMenu = nullptr;
	Text* ReturnMainMenu_Label = nullptr;

	Spawn_Box* selectorInfantry=nullptr;
	Spawn_Box* selectorDefenses = nullptr;
	Spawn_Box* selectorTank = nullptr;
	
	Image* frameSelector = nullptr;
	Image* UnitStats = nullptr;
	Image* UnitFrame = nullptr;
	Image* miniMap_Temp = nullptr;

	SDL_Rect selectorInfantry_Rect[4];
	SDL_Rect selectorDefenses_Rect[4];
	SDL_Rect selectorTank_Rect[4];

	SDL_Rect Conscript_Selection_Rect[4];
	SDL_Rect Flak_Selection_Rect[4];
	SDL_Rect Sniper_Selection_Rect[4];
	SDL_Rect Chrono_Selection_Rect[4];
	SDL_Rect Desolator_Selection_Rect[4];
	
	SDL_Texture* Tutorial_Tex = nullptr;
	SDL_Texture* Volume_Slides = nullptr;
	SDL_Texture* Options_tex = nullptr;
	SDL_Texture* miniMap_tex = nullptr;
	SDL_Texture* unitsSelection_Tex= nullptr;
	SDL_Texture* endingImages_Tex = nullptr;
	SDL_Texture* unitStats_text = nullptr;
	SDL_Texture* PauseButton_text = nullptr;
	SDL_Texture* StartGame_text = nullptr;
	SDL_Texture* Main_Menu_Temp_Tex = nullptr;
	SDL_Texture* selectorinGame_Tex = nullptr;
	SDL_Texture* pauseMenuPanel_Tex = nullptr;
	SDL_Texture* mainBar = nullptr;
	SDL_Texture* atlas = nullptr;
	SDL_Texture* mouse_tex = nullptr;
	SDL_Texture* Unit_Panels_tex = nullptr;
	SDL_Texture* InGame_Label_tex = nullptr;
	SDL_Texture* Buff_tex = nullptr;
	std::string atlasFileName;

};

#endif // __USER_INTERFACE_H__