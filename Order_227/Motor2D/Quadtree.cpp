#include "Defs.h"
#include "Log.h"
#include "SDL/include/SDL.h"
#include "Window.h"

#include "Quadtree.h"
#include "Render.h"
#include "App.h"

Quadtree::Quadtree(uint maxLevels, SDL_Rect section, uint level) :maxLevels(maxLevels), level(level), section(section)
{
	divided = false;
	
	if (this->section.w % 2 != 0) {
		this->section.w++;
	}
	if (this->section.h % 2 != 0) {
		this->section.h++;
	}
}

void Quadtree::DrawQuadtree() const
{
	myApp->render->DrawLine(section.x, section.y, section.x, section.y + section.h, 0, 0, 255);
	myApp->render->DrawLine(section.x, section.y, section.x + section.w, section.y, 0, 0, 255);
	myApp->render->DrawLine(section.x, section.y + section.h, section.x + section.w, section.y + section.h, 0, 0, 255);
	myApp->render->DrawLine(section.x + section.w, section.y, section.x + section.w, section.y + section.h, 0, 0, 255);
}



bool Quadtree::CheckVisibility()
{
	uint screen_w;
	uint screen_h;
	myApp->win->GetWindowSize(screen_w, screen_h);

	if (-myApp->render->camera.x - SCREEN_MARGIN > (section.x + section.w) ||
		(-myApp->render->camera.x + int(screen_w) + SCREEN_MARGIN) < section.x ||
		-myApp->render->camera.y - SCREEN_MARGIN > (section.y + section.h) ||
		(-myApp->render->camera.y + int(screen_h) + SCREEN_MARGIN) < section.y)
		return false;

	return true;
}

