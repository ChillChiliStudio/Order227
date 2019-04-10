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
	virtual void Enable() override;
	virtual void Disable() override;

protected:
	//State Entry
	virtual void OnIdle() override;
	virtual void OnHover() override;
	virtual void OnPress() override;

protected:
	SDL_Rect* stateSprites = nullptr;	//Disabled, Idle, Hover, Pressed
};

#endif //ACTION_BOX_H