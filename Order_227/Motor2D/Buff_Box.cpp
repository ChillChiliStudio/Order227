#include "Buff_Box.h"
#include "Render.h"
#include "UserInterface.h"
#include "Input.h"
#include "Text.h"
#include "Fonts.h"


Buff_Box::Buff_Box(fPoint position, SDL_Rect rect, SDL_Texture* tex,bool* able, ui_type type)
	:UI_Element(type, position), graphics(tex)
{
	enableButton = able;
	Rect = rect;
}

bool Buff_Box::Update(float dt) {


	return true;
}



bool Buff_Box::DebugDraw() const
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

bool Buff_Box::Draw() {

	if ((*enableButton)==true)
		myApp->render->Blit(graphics, position.x, position.y, &Rect, SDL_FLIP_NONE, false);

	return true;
}
