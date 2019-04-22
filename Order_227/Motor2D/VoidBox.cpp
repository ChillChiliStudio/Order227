#include "VoidBox.h"
#include "SDL/include/SDL_rect.h"

//Constructor
Void_Box::Void_Box(event_function action, fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex, UI_Element* parent, ui_type type)
	: Action_Box(type, center, spriteList, tex, parent), action(action)
{};

//State Entry
void Void_Box::OnPress() {
	//myApp->audio->PlayFx(myApp->audio->buttonSfx.id, 0);
	buttonStatus = button_state::PRESSING;
	*sprite = stateSprites[(int)buttonStatus];
	action();
}