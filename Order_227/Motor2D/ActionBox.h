#ifndef ACTION_BOX_H
#define ACTION_BOX_H	//@Carles

#include "Button.h"

class Action_Box : public Button
{
public:
	//Constructor
	Action_Box(ui_type type, fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex, UI_Element* parent = NULL);

	virtual ~Action_Box();

public:
	//Enable/Disable
	virtual void Enable();
	virtual void Disable();

protected:
	//State Entry
	virtual void OnIdle();
	virtual void OnHover();
	virtual void OnPress();

protected:
	SDL_Rect* stateSprites = nullptr;	//Disabled, Idle, Hover, Pressed
};

#endif //ACTION_BOX_H