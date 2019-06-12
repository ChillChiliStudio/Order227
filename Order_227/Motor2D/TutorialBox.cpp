#include "TutorialBox.h"
#include "Render.h"
#include "UserInterface.h"
#include "Input.h"
#include "Text.h"
#include "Fonts.h"


TutorialBox::TutorialBox(fPoint position, SDL_Rect rect, SDL_Texture* tex, ui_type type)
	:Image(type, position, rect, tex), graphics(tex)
{
	Rect = rect;
}

bool TutorialBox::Update(float dt) {

	if (Front == true) {
		Front = false;
		movingforeward = true;
	}
	if (movingforeward) {
		if (amount_Moved < Rect.w) {
				amount_Moved += velocitySlideshare;
				Rect.x += velocitySlideshare;
		}
		else if(amount_Moved>Rect.w){
			int temp = amount_Moved - Rect.w;
			Rect.x = Rect.x - temp;
			movingforeward = false;
			amount_Moved = 0;
			currentPage++;
		}
	}
	if (Back == true) {
		Back = false;
		movingBackward = true;
	}
	if (movingBackward) {
		if (amount_Moved < Rect.w) {
			amount_Moved += velocitySlideshare;
			Rect.x -= velocitySlideshare;
		}
		else if (amount_Moved > Rect.w) {
			int temp = amount_Moved - Rect.w;
			Rect.x = Rect.x + temp;
			movingBackward = false;
			amount_Moved = 0;
			currentPage--;
		}
	}
	return true;
}



bool TutorialBox::DebugDraw() const
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

bool TutorialBox::Draw() {

	if(active)
		myApp->render->Blit(graphics, position.x, position.y, &Rect, SDL_FLIP_NONE, false);


	return true;
}
