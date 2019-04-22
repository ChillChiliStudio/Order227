#include "App.h"
#include "Render.h"
#include "UserInterface.h"
#include "UnitButton.h"
#include "SDL/include/SDL_rect.h"
#include "Player.h"

//Constructor
Unit_Box::Unit_Box(event_function action, fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex, UI_Element* parent, SDL_Texture* TimerTexture, int timeCreator, ui_type type)
	: Action_Box(type, center, spriteList, tex, parent), action(action)
{
	Timer_Texture = TimerTexture;
	Timer = timeCreator;
	Animation = &myApp->gui->Timer_anim;
	Animation->speed = Timer / 100.0f;
	ActiveTimer = false;
};

//State Entry
void Unit_Box::OnPress() {
	//myApp->audio->PlayFx(myApp->audio->buttonSfx.id, 0);

	buttonStatus = button_state::PRESSING;
	*sprite = stateSprites[(int)buttonStatus];

	if (myApp->player->playerMoney >= 150&& myApp->player->startCreationUnit==false) {
		ActiveTimer = true;
		myApp->player->startCreationUnit = true;
		myApp->gui->unitCreationCD.Start();
		myApp->player->playerMoney -= 150;
		action();
	}
}

bool Unit_Box::Draw() {

	bool ret = true;

	if (active == true) {
		myApp->render->Blit(graphics, (int)position.x, (int)position.y, sprite, SDL_FLIP_NONE, false);

		if (ActiveTimer) {
			myApp->render->Blit(Timer_Texture, (int)position.x, (int)position.y, &Animation->GetCurrentFrame(myApp->GetDT()), SDL_FLIP_NONE, false);
			if (Animation->Finished()) {
				ActiveTimer = false;
			}
		}
		else {
			Animation->Reset();
		}
	}
	return ret;
}