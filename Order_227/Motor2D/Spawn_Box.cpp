#include "Spawn_Box.h"
#include "SDL/include/SDL_rect.h"

//Constructor
Spawn_Box::Spawn_Box(bool value, event_function action, fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex, UI_Element* parent)
	: Void_Box(action, center, spriteList, tex, parent, ui_type::CHECK_BOX), value(value)/*, valueStatus(GetValueState())*/
{
	stateSprites = new SDL_Rect[(int)button_state::MAX_TYPES];

	for (int i = 0; i < (int)button_state::MAX_TYPES; i++) {
		stateSprites[i] = spriteList[i];
	}
	valueStatus = GetValueState();
	if(value==true)
		Enable();
};

//Enable/Disable
void Spawn_Box::Enable()
{
	buttonStatus = button_state::IDLE;
	valueStatus = GetValueState();
	*sprite = stateSprites[(int)valueStatus];
}

void Spawn_Box::Disable()
{
	buttonStatus = button_state::DISABLED;
	*sprite = stateSprites[(int)buttonStatus];
}

//State Entry
void Spawn_Box::OnIdle()
{
	buttonStatus = button_state::IDLE;
	valueStatus = GetValueState();
	*sprite = stateSprites[(int)valueStatus];
}

void Spawn_Box::OnHover()
{
	//if (!value) {
		buttonStatus = button_state::HOVERING;
		*sprite = stateSprites[(int)button_state::HOVERING];
	//}
}

void Spawn_Box::OnPress()
{
	//myApp->audio->PlayFx(myApp->audio->buttonSfx.id, 0);
	buttonStatus = button_state::PRESSING;
	SwitchValue();
	valueStatus = GetValueState();
	*sprite = stateSprites[(int)valueStatus];

	if (action != nullptr) {
		action();
	}
}

//Value Methods
bool Spawn_Box::GetValue()
{
	return value;
}

value_state Spawn_Box::GetValueState()
{
		switch (value) {
		case true:
			return value_state::ON;
		case false:
			return value_state::OFF;
		}
}

bool Spawn_Box::SwitchValue()
{
	value = !value;
	return value;
}
