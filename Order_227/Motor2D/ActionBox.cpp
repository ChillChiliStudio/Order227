#include "ActionBox.h"
#include "SDL/include/SDL_rect.h"

//Constructor
Action_Box::Action_Box(ui_type type, fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex, UI_Element* parent)
	: Button(type, center, spriteList[(int)button_state::IDLE], tex, false, parent, NULL)
{
	stateSprites = new SDL_Rect[(int)button_state::MAX_TYPES];

	for (int i = 0; i < (int)button_state::MAX_TYPES; i++) {
		stateSprites[i] = spriteList[i];	
	}
};

Action_Box::~Action_Box()
{
	RELEASE(sprite);
	RELEASE_ARRAY(stateSprites);
}

//Enable/Disable
void Action_Box::Enable()
{
	buttonStatus = button_state::IDLE;
	*sprite = stateSprites[(int)buttonStatus];
}

void Action_Box::Disable()
{
	buttonStatus = button_state::DISABLED;
	*sprite = stateSprites[(int)buttonStatus];
}

//On state entry
void Action_Box::OnIdle()
{
	buttonStatus = button_state::IDLE;
	*sprite = stateSprites[(int)buttonStatus];
}

void Action_Box::OnHover()
{
	buttonStatus = button_state::HOVERING;
	*sprite = stateSprites[(int)buttonStatus];
}

void Action_Box::OnPress()
{
	//myApp->audio->PlayFx(myApp->audio->buttonSfx.id, 0);
	buttonStatus = button_state::PRESSING;
	*sprite = stateSprites[(int)buttonStatus];
}