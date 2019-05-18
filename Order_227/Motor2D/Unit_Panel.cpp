#include "Unit_Panel.h"
#include "Render.h"
#include "UserInterface.h"
#include "Input.h"
#include "Text.h"
#include "Fonts.h"


Unit_Panel::Unit_Panel( SDL_Rect rect, Image* button,SDL_Texture* tex,ui_type type)
	:UI_Element(type, center), graphics(tex)
{
	Panel_Rect = rect;
	Currentbutton = button;
}

bool Unit_Panel::Start() {

	Stats = new Text("HOLA", SDL_Color{ 255,0,0 }, myApp->fonts->fontsList[(int)font_id::MOLOT]->fontPtr);
	Stats->position.x = position.x + 18;
	Stats->position.y = position.y + 15;
	Stats->active = active;

	return true;
}

bool Unit_Panel::Update(float dt) {

	SDL_Point mousePos;
	myApp->input->GetMousePosition(mousePos.x, mousePos.y);
	position.x = mousePos.x;
	position.y = mousePos.y;
	if (Stats != nullptr) {
		Stats->position.x = mousePos.x;
		Stats->position.y = mousePos.y-400;

	}

	ActiveElement();

	return true;
}



bool Unit_Panel::DebugDraw() const
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

bool Unit_Panel::Draw() {
	
	if (enableButton)
		myApp->render->Blit(graphics, position.x, position.y-Panel_Rect.h, &Panel_Rect, SDL_FLIP_NONE, false);

	return true;
}

bool Unit_Panel::onTop() {

	bool ret = false;

	SDL_Point mousePos;
	myApp->input->GetMousePosition(mousePos.x, mousePos.y);

	if (mousePos.x > Currentbutton->position.x&&mousePos.x<Currentbutton->position.x + Currentbutton->sprite->w&&mousePos.y>Currentbutton->position.y&&mousePos.y < Currentbutton->position.y + Currentbutton->sprite->h)
		ret = true;

	return ret;
}

bool Unit_Panel::ActiveElement() {
	bool ret = false;

	if (onTop())
		enableButton = true;
	else
		enableButton = false;

	return ret;
}