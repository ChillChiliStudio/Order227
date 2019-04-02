#include "Defs.h"
#include "Log.h"
#include "SDL/include/SDL.h"

#include "Quadtree.h"
#include "Render.h"
#include "App.h"

Quadtree::Quadtree(uint max_levels, SDL_Rect section, uint level) :max_levels(max_levels), level(level), section(section)
{
	divided = false;
}

void Quadtree::DrawQuadtree() const
{
	myApp->render->DrawLine(section.x, section.y, section.x, section.y + section.h, 0, 0, 255);
	myApp->render->DrawLine(section.x, section.y, section.x + section.w, section.y, 0, 0, 255);
	myApp->render->DrawLine(section.x, section.y + section.h, section.x + section.w, section.y + section.h, 0, 0, 255);
	myApp->render->DrawLine(section.x + section.w, section.y, section.x + section.w, section.y + section.h, 0, 0, 255);
}

bool Quadtree::CheckTouch(const SDL_Rect& rect)
{
	//Middle point of the tile
	iPoint pos(rect.x + rect.w / 2, rect.y + rect.h / 2);

	if (pos.x < section.x ||
		pos.x > section.x + section.w ||
		pos.y < section.y ||
		pos.y > section.y + section.h)
		return false;

	return true;
}

