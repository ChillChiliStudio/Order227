#include "Mouse.h"
#include "Render.h"
#include "UserInterface.h"
#include "Input.h"
#include "Entity_Manager.h"
#include "GroupManager.h"
#include "Map.h"


Mouse::Mouse(SDL_Texture* tex, ui_type type)
	:UI_Element(type, center), graphics(tex)
{

	loadMouseAnims(Animation_Arr[0],0,1);
	loadMouseAnims(Animation_Arr[3], 18, 32);
	loadMouseAnims(Animation_Arr[1], 33, 43);
	loadMouseAnims(Animation_Arr[1], 33, 43);

	loadMouseAnims(Animation_Arr[2], 55, 59);
	loadMouseAnims(Animation_Arr[4], 60, 65);

	Animation_Arr[2].speed = 5.0f;
	Animation_Arr[4].speed = 5.0f;

	currentState = Mouse_States::IDLE;

	myApp->gui->Main_Menu_Elements.push_back(this);
	CurrentAnimation = &Animation_Arr[0];
}

void Mouse::loadMouseAnims(Animation &Anim,int initial,int end) {

	int x = 0, y = 0;
	//if (x > 17)
	//	x = 0;
	for (int i = 0; i < end; ++i) {
		if (x < 55 * 18) {
			if(i>=initial)
				Anim.PushBack(SDL_Rect({ x,y,55,43 }));
			x += 55;
		}
		else {
			y += 43;
			x = 0;
		}
	}
	Anim.speed = 10.0f;

}

bool Mouse::Update(float dt) {

	SDL_Point mousePos;
	myApp->input->GetMousePosition(mousePos.x, mousePos.y);

	position.x = mousePos.x;
	position.y = mousePos.y;
	if (currentState == Mouse_States::IDLE) {
		offset.x = offset.y = 0;
	}
	else
		offset.x = offset.y = -20;
	CheckCurrentState();

	

	return true;
}

Mouse_States Mouse::CheckCurrentState()
{
	switch (currentState) {
	case Mouse_States::IDLE:
		CurrentAnimation = &Animation_Arr[(int)currentState];
		if (myApp->groups->playerGroup.groupUnits.size() > 0) {
			currentState = Mouse_States::SELECTUNITS;
			
		}
		break;
	case Mouse_States::SELECTUNITS:
		CurrentAnimation = &Animation_Arr[(int)currentState];
		if (myApp->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN) {
			currentState = Mouse_States::PRESSING;
			break;
		}
		else if (myApp->groups->playerGroup.groupUnits.size() == 0) {
			currentState = Mouse_States::IDLE;
			break;
		}else if (onTopEnemy()) {
			currentState = Mouse_States::ONTOPENEMY;
		}
		break;
	case Mouse_States::ONTOPENEMY:
		CurrentAnimation = &Animation_Arr[int(currentState)];
		if (onTopEnemy() == false)
			currentState=Mouse_States::IDLE;
		if (myApp->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
			currentState = Mouse_States::PRESSONENEMY;
		break;
	case Mouse_States::PRESSONENEMY:
		CurrentAnimation = &Animation_Arr[int(currentState)];
		if (CurrentAnimation->Finished()) {
			currentState = Mouse_States::IDLE;
			CurrentAnimation->Reset();
		}
		break;
	case Mouse_States::PRESSING:
		CurrentAnimation = &Animation_Arr[int(currentState)];
		if (CurrentAnimation->Finished()) {	
			currentState = Mouse_States::IDLE;
			CurrentAnimation->Reset();
		}
		break;
	}

	return currentState;
}

bool Mouse::onTopEnemy() {

	iPoint mousePos = myApp->render->ScreenToWorld(position.x, position.y);


	for (std::vector<Unit>::iterator item = myApp->entities->unitPool.begin(); item != myApp->entities->unitPool.end(); item = next(item)) {
		if ((*item).active == true && (*item).IsDead() == false && (*item).faction ==entity_faction::CAPITALIST) {
		
			if (mousePos.x > (*item).position.x&&mousePos.x<=(*item).position.x + (*item).entityRect.w&&mousePos.y>(*item).position.y&&mousePos.y < (*item).position.y + (*item).entityRect.h)
				return true;
			}
		}
	return false;
	}

bool Mouse::DebugDraw() const
{
	bool ret = true;

	/* COLORS
	Red: 255, 0, 0
	Green: 0, 255, 0
	Blue: 0, 0, 255
	Yellow: 255, 255, 0
	Magenta: 255, 0, 255
	Cyan: 0, 255, 255
	White: 255, 255, 255
	Black: 0, 0, 0
	Brown: 160, 128, 96
	*/

	// Top-Left Corner
	//myApp->render->DrawLine((int)position.x, (int)position.y, (int)(position.x + LowHealth.w), (int)position.y, 255, 0, 0, 255, true);	// Right
	//myApp->render->DrawLine((int)position.x, (int)position.y, (int)position.x, (int)position.y + LowHealth.h, 255, 0, 0, 255, true);	// Down
	//																																// Down-Right Corner
	//myApp->render->DrawLine((int)(position.x + LowHealth.w), (int)(position.y + LowHealth.h), (int)position.x, (int)(position.y + LowHealth.h), 255, 0, 0, 255, true);	// Up
	//myApp->render->DrawLine((int)(position.x + LowHealth.w), (int)(position.y + LowHealth.h), (int)(position.x + LowHealth.w), (int)position.y, 255, 0, 0, 255, true);	// Left

	return ret;
}

bool Mouse::Draw() {


	myApp->render->Blit(graphics, position.x+offset.x, position.y+offset.y, &CurrentAnimation->AdvanceAnimation(myApp->GetDT()), SDL_FLIP_NONE, false);


	return true;
}