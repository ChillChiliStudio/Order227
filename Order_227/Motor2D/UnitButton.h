#ifndef UNIT_BOX_H
#define UNIT_BOX_H	//@Carles

#include "ActionBox.h"
#include "Animation.h"
#include "Timer.h"

class Text;
struct SDL_Texture;

class Unit_Box : public Action_Box
{
public:
	typedef void(*event_function)(void);

	//Constructor
	Unit_Box(event_function action, fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex, UI_Element* parent = NULL, SDL_Texture* TimerTexture = NULL, int timeCreator = 0, int unitCost=0,bool* _abletoCraft=nullptr,SDL_Scancode Hotkey=SDL_SCANCODE_0, ui_type type = ui_type::ACTION_BOX);
	bool Start() override;

	int Queue;

protected:
	//State Entry

	void OnPress() override;
	void OnPressRight() override;
	void OnHotkey();
	bool Draw() override;
	void DoAction();


protected:
	
	SDL_Scancode ButtonHotkey;
	Text* Queue_Info = nullptr;

	bool startCreationUnit;
	int UnitCost;
	event_function action;
	Animation Animation;
	SDL_Texture* Timer_Texture=nullptr;
	Timer unitCreationCD;
	int Timer;
	bool ActiveTimer;
	bool* abletoCraft = nullptr;
};

#endif //VOID_BOX_H