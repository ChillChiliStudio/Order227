#include "App.h"
#include "Render.h"
#include "UserInterface.h"
#include "UnitButton.h"
#include "SDL/include/SDL_rect.h"
#include "Player.h"
#include "Text.h"
#include "Fonts.h"

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
	//Queue_Info = new Text("0", SDL_Color{255,255,255}, myApp->fonts->defaultFont);

	//Queue_Info->position.x = position.x+15;
	//Queue_Info->position.y = position.y+15;
	//Queue_Info->active = false;
};

//State Entry
void Unit_Box::OnPress() {
	//myApp->audio->PlayFx(myApp->audio->buttonSfx.id, 0);

	buttonStatus = button_state::PRESSING;
	*sprite = stateSprites[(int)buttonStatus];
	if ((*abletoCraft) == true) {
		if (myApp->player->playerMoney > UnitCost) {
			if(startCreationUnit==false)
				DoAction();
			/*ActiveTimer = true;
			startCreationUnit = true;
			unitCreationCD.Start();
			myApp->player->playerMoney -= UnitCost;
			myApp->gui->Moneytext->ChangeString(std::to_string(myApp->player->playerMoney));
			action();*/
			
			Queue_Info->active = true;
			Queue_Info->ChangeString(std::to_string(Queue));
			Queue++;
		
		}
	}

}
bool Unit_Box::Start() {

	Queue_Info = new Text("0", SDL_Color{ 255,255,255 }, myApp->fonts->defaultFont);

	Queue_Info->position.x = position.x + 15;
	Queue_Info->position.y = position.y + 15;
	Queue_Info->active = false;

	return true;
}

void Unit_Box::DoAction() {

	ActiveTimer = true;
	startCreationUnit = true;
	unitCreationCD.Start();
	myApp->player->playerMoney -= UnitCost;
	myApp->gui->Moneytext->ChangeString(std::to_string(myApp->player->playerMoney));
	action();

}



bool Unit_Box::Draw() {

	if (Queue >= 1&&ActiveTimer==false&&startCreationUnit==false) {
		if(Queue>1)
			DoAction();
		Queue--;
		Queue_Info->ChangeString(std::to_string(Queue));
	
	}
	else if (Queue == 0) {
		Queue_Info->active = false;

	}

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
			Queue_Info->Draw();
			if (Animation.Finished()) {
				ActiveTimer = false;
				startCreationUnit = false;
				Animation.Reset();
			}
		}
		else {
			Animation.Reset();
		}
	}
	return ret;
}