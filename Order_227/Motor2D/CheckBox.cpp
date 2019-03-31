#include "CheckBox.h"
#include "SDL/include/SDL_rect.h"

//Constructor
Check_Box::Check_Box(bool* value, event_function action, fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex, UI_Element* parent)
	: Void_Box(action, center, spriteList, tex, parent, ui_type::CHECK_BOX), value(value)/*, valueStatus(GetValueState())*/
{
	valueStatus = GetValueState();
};

//Enable/Disable
void Check_Box::Enable()
{
	buttonStatus = button_state::IDLE;
	valueStatus = GetValueState();
	*sprite = stateSprites[(int)valueStatus];
}

void Check_Box::Disable()
{
	buttonStatus = button_state::DISABLED;
	*sprite = stateSprites[(int)buttonStatus];
}

//State Entry
void Check_Box::OnIdle()
{
	buttonStatus = button_state::IDLE;
	valueStatus = GetValueState();
	*sprite = stateSprites[(int)valueStatus];
}

void Check_Box::OnHover()
{
	buttonStatus = button_state::HOVERING;
	*sprite = stateSprites[(int)button_state::HOVERING];
}

void Check_Box::OnPress()
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
bool Check_Box::GetValue()
{
	return value;
}

value_state Check_Box::GetValueState()
{
	switch (*value) {
	case true:
		return value_state::ON;
	case false:
		return value_state::OFF;
	}
}

bool Check_Box::SwitchValue()
{
	*value = !*value;
	return *value;
}