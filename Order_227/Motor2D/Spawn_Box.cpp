#include "Spawn_Box.h"
#include "SDL/include/SDL_rect.h"
#include "UserInterface.h"

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
	if (!value) {
		buttonStatus = button_state::HOVERING;
		*sprite = stateSprites[(int)button_state::HOVERING];
	}
}

void Spawn_Box::OnPress()
{
	//myApp->audio->PlayFx(myApp->audio->buttonSfx.id, 0);
	buttonStatus = button_state::PRESSING;
	SwitchValue();
	valueStatus = GetValueState();
	*sprite = stateSprites[(int)valueStatus];
	ActivateChildren();
	for (std::list<Spawn_Box*>::iterator iter = myApp->gui->SpawnSelectors.begin(); iter != myApp->gui->SpawnSelectors.end(); iter = next(iter)) 
	{
		if (this != (*iter)) {
			(*iter)->value=false;
			(*iter)->DeactivateChildren();
			(*iter)->valueStatus = (*iter)->GetValueState();
			*(*iter)->sprite = (*iter)->stateSprites[(int)(*iter)->valueStatus];
		}
	}

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
	value_state ret;

	switch (value) {
	case true:
		ret = value_state::ON;
		break;
	case false:
		ret = value_state::OFF;
		break;
	}

	return ret;
}

bool Spawn_Box::SwitchValue()
{
	value = !value;
	return value;
}
