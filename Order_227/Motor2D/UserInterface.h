#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H	//@Carles

#include "Module.h"
#include "Animation.h"
#include "Timer.h"

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
//class Window;
struct _TTF_Font;
struct SDL_Rect;
struct SDL_Texture;


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
	void loadAnim();

	std::list<UI_Element*>::iterator DestroyElement(std::list<UI_Element*>::iterator iter);

	//TODO: Factories should return their specific class type
	Image* CreateImage(fPoint center, SDL_Rect texRect = { 0, 0, 0, 0 }, SDL_Texture* tex = NULL, bool dynamic = false, UI_Element* parent = NULL, std::list<UI_Element*>* children = NULL);
	Text* CreateText(fPoint center, const char* content, font_id id, SDL_Color color = { 255, 255, 255, 255 }, bool dynamic = false, UI_Element* parent = NULL, std::list<UI_Element*>* children = NULL);
	//UI_Element* CreateInputText();
	Void_Box* CreateVoidBox(void(*action)(void), fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex = NULL, UI_Element* parent = NULL);
	Check_Box* CreateCheckBox(bool* value, fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex = NULL, void(*action)(void) = NULL, UI_Element* parent = NULL);
	Spawn_Box* CreateSpawnBox(bool value, fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex = NULL, void(*action)(void) = NULL, UI_Element* parent = NULL);
	Unit_Box* CreateUnitBox(void(*action)(void), fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex = NULL, UI_Element* parent = NULL, SDL_Texture* TimerTexture = NULL, int timeCreator = 0);

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
	bool interfaceDebugDraw = false;
	//uint defaultScale;	//IMPROVE: Future use
	Animation Timer_anim;
	Timer unitCreationCD;
	Image* pauseMenuPanel = nullptr;

	std::list<Spawn_Box*> SpawnSelectors;

private:
	std::list<UI_Element*> screenElements;

	SDL_Texture* Timer_Texture = nullptr;

	Image* MainBarPanel = nullptr;

	Spawn_Box* selectorInfantry=nullptr;
	Spawn_Box* selectorDefenses = nullptr;
	Spawn_Box* selectorTank = nullptr;
	Unit_Box* ConscriptCreator = nullptr;
	Image* frameSelector = nullptr;

	SDL_Rect selectorInfantry_Rect[4];
	SDL_Rect selectorDefenses_Rect[4];
	SDL_Rect selectorTank_Rect[4];

	SDL_Rect Conscript_Selection_Rect[4];

	SDL_Texture* unitsSelection_Tex;

	SDL_Texture* selectorinGame_Tex = nullptr;
	SDL_Texture* pauseMenuPanel_Tex = nullptr;
	SDL_Texture* mainBar = nullptr;
	SDL_Texture* atlas = nullptr;
	std::string atlasFileName;

};

#endif // __USER_INTERFACE_H__