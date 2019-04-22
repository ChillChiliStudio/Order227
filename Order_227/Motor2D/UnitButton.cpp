#include "App.h"
#include "Render.h"
#include "UserInterface.h"
#include "UnitButton.h"
#include "SDL/include/SDL_rect.h"


//Constructor
Unit_Box::Unit_Box(event_function action, fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex, UI_Element* parent, SDL_Texture* TimerTexture, int timeCreator, ui_type type)
	: Action_Box(type, center, spriteList, tex, parent), action(action)
{
	//stateSprites = new SDL_Rect[(int)button_state::MAX_TYPES];

	//for (int i = 0; i < (int)button_state::MAX_TYPES; i++) {
	//	stateSprites[i] = spriteList[i];
	//}
	Timer_Texture = TimerTexture;
	Timer = timeCreator;
	Animation = &myApp->gui->Timer_anim;
	Animation->speed = Timer / 10;
};

//State Entry
void Unit_Box::OnPress() {
	//myApp->audio->PlayFx(myApp->audio->buttonSfx.id, 0);
	buttonStatus = button_state::PRESSING;
	*sprite = stateSprites[(int)buttonStatus];
	ActiveTimer = true;
	action();
}

bool Unit_Box::Draw() {

	bool ret = true;

	//myApp->render->Blit(graphics, (int)position.x, (int)position.y, sprite, SDL_FLIP_NONE, false);
	//
	//if(ActiveTimer)
	//myApp->render->Blit(Timer_Texture, (int)position.x, (int)position.y,&Animation->GetCurrentFrame(myApp->GetDT()));
	//else {
	//	Animation->Reset();
	//}
	return ret;
}