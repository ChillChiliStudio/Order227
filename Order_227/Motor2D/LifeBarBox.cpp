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
		life = &parent->stats.health;
		totalLife = parent->stats.health;
		position = { parent->position.x - (LowHealth.w / 2) + 10, parent->position.y - (LowHealth.h / 2) - 6 };
		Currentparent = parent;

	}

	LowHealth = { 0,0,62,6 };
	if (parent!=nullptr&&parent->faction == entity_faction::CAPITALIST) {
		HighHealth = { 62,6,62,6 };

	}
	else
	HighHealth = { 62,0,62,6 };
	myApp->gui->Main_Menu_Elements.push_back(this);
}


bool LifeBar::Update(float dt) {
	Draw();
	if (Currentparent != nullptr) {
		position = { Currentparent->position.x - (LowHealth.w / 2) + 10, Currentparent->position.y - (LowHealth.h / 2) - 6 };
		if((*life) <= 0 || Currentparent->active == false)
			mustDestroy = true;
	}

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
	myApp->render->DrawLine((int)position.x, (int)position.y, (int)(position.x + LowHealth.w), (int)position.y, 255, 0, 0, 255, true);	// Right
	myApp->render->DrawLine((int)position.x, (int)position.y, (int)position.x, (int)position.y + LowHealth.h, 255, 0, 0, 255, true);	// Down
																																	// Down-Right Corner
	myApp->render->DrawLine((int)(position.x + LowHealth.w), (int)(position.y + LowHealth.h), (int)position.x, (int)(position.y + LowHealth.h), 255, 0, 0, 255, true);	// Up
	myApp->render->DrawLine((int)(position.x + LowHealth.w), (int)(position.y + LowHealth.h), (int)(position.x + LowHealth.w), (int)position.y, 255, 0, 0, 255, true);	// Left

	return ret;
}

bool LifeBar::Draw() {
	if (Currentparent != nullptr) {
		if (Currentparent->selected == true && Currentparent->faction == entity_faction::COMMUNIST) {
			myApp->render->Blit(graphics, (int)position.x, (int)position.y, &HighHealth);
			//myApp->render->Blit(graphics, (int)position.x, (int)position.y, &HighHealth);
		}
		else if (Currentparent->faction == entity_faction::CAPITALIST) {
			myApp->render->Blit(graphics, (int)position.x, (int)position.y, &HighHealth);
			//myApp->render->Blit(graphics, (int)position.x, (int)position.y, &HighHealth
		}
		
	}
	else {
		myApp->render->Blit(graphics, (int)position.x - 100, (int)position.y, &LowHealth, SDL_FLIP_NONE, true, 1.7f);
		myApp->render->Blit(graphics, (int)position.x - 100, (int)position.y, &HighHealth, SDL_FLIP_NONE, true, 1.7f);

	}

	return true;
}