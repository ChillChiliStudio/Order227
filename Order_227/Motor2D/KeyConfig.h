#ifndef KEY_CONFIG_H
#define KEY_CONFIG_H	//@Carles

#include "ActionBox.h"
#include "SDL/include/SDL.h"

class Text;
struct SDL_Texture;

class Key_Config_Box : public Action_Box
{
public:
	typedef void(*event_function)(int*);

	//Constructor
	Key_Config_Box(event_function action, fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex, UI_Element* parent = NULL, int* inputKey = nullptr, ui_type type = ui_type::ACTION_BOX);

public:
	void OnIdle() override;
	void UpdateText();
	Text* keyText;

protected:
	//State Entry
	void OnHover() override;
	void OnPress() override;

protected:

	event_function action;
	int* keyValue;
};

#endif //KEY_CONFIG_H