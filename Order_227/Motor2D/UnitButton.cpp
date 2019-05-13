#include "App.h"
#include "Render.h"
#include "UserInterface.h"
#include "UnitButton.h"
#include "SDL/include/SDL_rect.h"
#include "Player.h"
#include "Text.h"

//Constructor
Unit_Box::Unit_Box(event_function action, fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex, UI_Element* parent, SDL_Texture* TimerTexture, int timeCreator,int unitCost, bool* _abletoCraft,ui_type type)
	: Action_Box(type, center, spriteList, tex, parent), action(action)
{
	UnitCost = unitCost;
	Timer_Texture = TimerTexture;
	Timer = timeCreator;
	Animation = myApp->gui->loadAnim();
	Animation.speed = Timer ;
	ActiveTimer = false;
	startCreationUnit = false;
	if (_abletoCraft != nullptr)
		abletoCraft = _abletoCraft;
	else {
		abletoCraft = new bool();
		(*abletoCraft) = true;
	}
};

//State Entry
void Unit_Box::OnPress() {
	//myApp->audio->PlayFx(myApp->audio->buttonSfx.id, 0);

	buttonStatus = button_state::PRESSING;
	*sprite = stateSprites[(int)buttonStatus];
	if ((*abletoCraft) == true) {
		if (myApp->player->playerMoney > UnitCost&& startCreationUnit == false) {
			ActiveTimer = true;
			startCreationUnit = true;
			unitCreationCD.Start();
			myApp->player->playerMoney -= UnitCost;
			myApp->gui->Moneytext->ChangeString(std::to_string(myApp->player->playerMoney));
			action();
		}
	}
}


bool Unit_Box::Draw() {

	bool ret = true;

	//if (startCreationUnit == true && unitCreationCD.Read() >= Timer)
	//	startCreationUnit = false;
	if ((*abletoCraft) == false) {
		*sprite = stateSprites[3];
	}
	else
		*sprite = stateSprites[0];

	if (active == true) {
		myApp->render->Blit(graphics, (int)position.x, (int)position.y, sprite, SDL_FLIP_NONE, false);

		if (ActiveTimer) {
			myApp->render->Blit(Timer_Texture, (int)position.x, (int)position.y, &Animation.AdvanceAnimation(myApp->GetDT()), SDL_FLIP_NONE, false);
			if (Animation.Finished()) {
				ActiveTimer = false;
				startCreationUnit = false;
			}
		}
		else {
			Animation.Reset();
		}
	}
	return ret;
}