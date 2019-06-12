#include "KeyConfig.h"
#include "App.h"
#include "Controls.h"
#include "Text.h"

//Constructor
Key_Config_Box::Key_Config_Box(event_function action, fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex, UI_Element* parent, int* inputKey, ui_type type)
	: Action_Box(type, center, spriteList, tex, parent), action(action), keyValue(inputKey)
{}

//On state entry
void Key_Config_Box::OnIdle()
{
	if (!myApp->controls->awaitingInput) {
		buttonStatus = button_state::IDLE;
		*sprite = stateSprites[(int)buttonStatus];
	}
}

void Key_Config_Box::OnHover()
{
	if (!myApp->controls->awaitingInput) {
		buttonStatus = button_state::HOVERING;
		*sprite = stateSprites[(int)buttonStatus];
	}
}

void Key_Config_Box::OnPress()
{
	if (myApp->controls->buttonUsed == nullptr) {
		buttonStatus = button_state::PRESSING;
		*sprite = stateSprites[(int)buttonStatus];
		action(keyValue);
		myApp->controls->buttonUsed = this;
	}
}

void Key_Config_Box::UpdateText()
{
	keyText->ChangeString(myApp->controls->TranslateKeycode(*keyValue).c_str());
}