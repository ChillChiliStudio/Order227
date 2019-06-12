#ifndef KEY_CONFIG_H
#define KEY_CONFIG_H	//@Carles

#include "ActionBox.h"

class Text;
struct SDL_Texture;

class Key_Config_Box : public Action_Box
{
public:
	typedef void(*event_function)(void);

	//Constructor
	Key_Config_Box(event_function action, fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex, UI_Element* parent = NULL, SDL_Texture* TimerTexture = NULL, int timeCreator = 0, int unitCost = 0, bool* _abletoCraft = nullptr, SDL_Scancode Hotkey = SDL_SCANCODE_0, ui_type type = ui_type::ACTION_BOX);
	bool Start() override;

	void ResetButton() { Queue = 0; Animation.Reset(); startCreationUnit = false; }
	int Queue;

protected:
	//State Entry

	void OnPress() override;
	void OnPressRight() override;
	void OnHotkey();
	bool Draw() override;
	void DoAction();

protected:

	SDL_Scancode* keyValue;
};

#endif //KEY_CONFIG_H