#include "Defs.h"
#include "Point.h"
#include "Log.h"
#include "SDL/include/SDL.h"

#include "App.h"
#include "Render.h"
#include "EntityQuadtree.h"
#include "Entity_Manager.h"
#include "Entity.h"

EntityQuadtree::EntityQuadtree(uint maxLevels, SDL_Rect section, uint level): Quadtree(maxLevels, section, level)
{
	for (int i = 0; i < 4; ++i)
		nodes[i] = nullptr;

}

void EntityQuadtree::CleanUp()
{
	entitiesVector.clear();

	if (divided) {
		for (int i = 0; i < 4; ++i) {
			nodes[i]->CleanUp();
			nodes[i] = nullptr;
		}
	}
}

void EntityQuadtree::Split(int times)
{
	if (level < maxLevels && divided == false)
	{
		nodes[0] = new EntityQuadtree(maxLevels, { section.x,section.y, section.w / 2, section.h / 2 }, level + 1);
		nodes[1] = new EntityQuadtree(maxLevels, { section.x + section.w / 2,section.y, section.w / 2,section.h / 2 }, level + 1);
		nodes[2] = new EntityQuadtree(maxLevels, { section.x,section.y + section.h / 2 , section.w / 2, section.h / 2 },  level + 1);
		nodes[3] = new EntityQuadtree(maxLevels, { section.x + section.w / 2,section.y + section.h / 2, section.w / 2,section.h / 2 }, level + 1);
		divided = true;

		if (times > 1)
		{
			nodes[0]->Split(times - 1);
			nodes[1]->Split(times - 1);
			nodes[2]->Split(times - 1);
			nodes[3]->Split(times - 1);
		}
	}
}

void EntityQuadtree::DrawQuadtree()
{
	if (CheckVisibility() == true)
	{
		if (level == 1)
		{
			myApp->render->DrawLine(section.x, section.y, section.x, section.y + section.h, 255, 0, 0);
			myApp->render->DrawLine(section.x, section.y, section.x + section.w, section.y, 255, 0, 0);
			myApp->render->DrawLine(section.x, section.y + section.h, section.x + section.w, section.y + section.h, 255, 0, 0);
			myApp->render->DrawLine(section.x + section.w, section.y, section.x + section.w, section.y + section.h, 255, 0, 0);
		}

		if (!entitiesVector.empty())
			myApp->render->DrawQuad(section, 255, 0, 0, 50);

		if (divided == true)
		{
			myApp->render->DrawLine(section.x + section.w / 2, section.y, section.x + section.w / 2, section.y + section.h, 255, 0, 0);
			myApp->render->DrawLine(section.x, section.y + section.h / 2, section.x + section.w, section.y + section.h / 2, 255, 0, 0);

			for (int i = 0; i < 4; ++i)
			{
				nodes[i]->DrawQuadtree();
			}
		}
	}
}

void EntityQuadtree::FillTree()
{
	if (level != 1)
		return;

	for (int i = 0; i < myApp->entities->entitiesVector.size(); ++i)
	{
		if (myApp->entities->entitiesVector[i]!=nullptr && myApp->entities->entitiesVector[i]->active )
			AddEntity(myApp->entities->entitiesVector[i]);
	}
}

void EntityQuadtree::ClearTree()
{
	entitiesVector.clear();

	if (divided){
		
		for (int i = 0; i < 4; ++i){
			nodes[i]->ClearTree();
		}
	}
}

void EntityQuadtree::AddEntity(Entity* entity)
{
	if (level == maxLevels)
		entitiesVector.push_back(entity);
	
	else {
		for (int i = 0; i < 4; ++i) {
			if (nodes[i]->CheckTouch(entity->entityRect))
				nodes[i]->AddEntity(entity);

		}
	}
}

bool EntityQuadtree::CheckTouch(const SDL_Rect& rect)
{
	if (rect.x > section.x + section.w ||
		rect.x + rect.w < section.x ||
		rect.y > section.y + section.h ||
		rect.y + rect.h < section.y)
		return false;

	return true;
}

EntityQuadtree* EntityQuadtree::NodeAt(int x, int y)
{
	if (divided) {
		for (int i = 0; i < 4; ++i) {
			if (nodes[i]->CheckTouch({ x,y,0,0 }))
			{
				if (nodes[i]->level == maxLevels)
					return nodes[i];

				else
					return nodes[i]->NodeAt(x, y);
			}
		}
	}

	else
		return this;
}