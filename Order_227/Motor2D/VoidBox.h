#ifndef VOID_BOX_H
#define VOID_BOX_H	//@Carles

#include "ActionBox.h"

class Void_Box : public Action_Box	///https://en.cppreference.com/w/cpp/language/lambda
{
public:
	typedef void(*event_function)(void);

	//Constructor
	Void_Box(event_function action, fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex, UI_Element* parent = NULL, ui_type type = ui_type::ACTION_BOX);
	void ChangeSize(float newsize) { scale = newsize; }

protected:
	//State Entry
	 void OnPress() override;

protected:
	event_function action;

};

#endif //VOID_BOX_H