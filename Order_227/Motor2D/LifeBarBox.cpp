#include "LifeBarBox.h"
#include "Render.h"
#include "UserInterface.h"


LifeBar::LifeBar(fPoint center, Unit* parent,SDL_Texture* tex,ui_type type,float* auxHealth)
	:UI_Element(type, center), graphics(tex)
{
	if (auxHealth != NULL) {
		life = auxHealth;
		totalLife = (*auxHealth);
		position = { center.x - (LowHealth.w / 2) + 90, center.y - (LowHealth.h / 2) - 20 };
	}
	else
	{
		life = &parent->health;
		totalLife = parent->health;
		position = { parent->position.x - (LowHealth.w / 2) + 10, parent->position.y - (LowHealth.h / 2) - 6 };
		Currentparent = parent;

	}

	LowHealth = { 0,0,62,6 };
	HighHealth = { 62,0,62,6 };
	myApp->gui->Main_Menu_Elements.push_back(this);
}


bool LifeBar::Update(float dt) {

	if ((*life) <= 0) {
		mustDestroy = true;
	}

	if(Currentparent!=nullptr)
		position = { Currentparent->position.x - (LowHealth.w / 2)+10, Currentparent->position.y - (LowHealth.h / 2)-6 };

	HighHealth.w = (*life)*LowHealth.w / totalLife;

	return true;
}

bool LifeBar::DebugDraw() const
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
	myApp->render->DrawLine((int)position.x, (int)position.y, (int)(position.x + LowHealth.w), (int)position.y, 255, 0, 0, 255, false);	// Right
	myApp->render->DrawLine((int)position.x, (int)position.y, (int)position.x, (int)position.y + LowHealth.h, 255, 0, 0, 255, false);	// Down
																																	// Down-Right Corner
	myApp->render->DrawLine((int)(position.x + LowHealth.w), (int)(position.y + LowHealth.h), (int)position.x, (int)(position.y + LowHealth.h), 255, 0, 0, 255, false);	// Up
	myApp->render->DrawLine((int)(position.x + LowHealth.w), (int)(position.y + LowHealth.h), (int)(position.x + LowHealth.w), (int)position.y, 255, 0, 0, 255, false);	// Left

	return ret;
}
bool LifeBar::Draw() {

	myApp->render->Blit(graphics, (int)position.x, (int)position.y, &LowHealth);
	myApp->render->Blit(graphics, (int)position.x, (int)position.y, &HighHealth);

	return true;
}