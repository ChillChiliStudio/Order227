#ifndef UNIT_BOX_H
#define UNIT_BOX_H	//@Carles

#include "ActionBox.h"
#include "Animation.h"

class SDL_Texture;

class Unit_Box : public Action_Box
{
public:
	typedef void(*event_function)(void);

	//Constructor
	Unit_Box(event_function action, fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex, UI_Element* parent = NULL, SDL_Texture* TimerTexture = NULL, int timeCreator = 0, ui_type type = ui_type::ACTION_BOX);

protected:
	//State Entry
	void OnPress() override;

	bool Draw() override;

protected:

	event_function action;
	Animation* Animation;
	SDL_Texture* Timer_Texture;
	int Timer;
	bool ActiveTimer;
};

#endif //VOID_BOX_H