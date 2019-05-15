#ifndef _ENTITY_QUADTREE_H
#define _ENTITY_QUADTREE_H

#include "Quadtree.h"
#include <Vector>



class Entity;

class EntityQuadtree:public Quadtree
{
public:

	EntityQuadtree(uint maxLevels, SDL_Rect section, uint level = 1);

	void CleanUp(){}

	void Split(int times=1);

	//DEBUG

	void DrawQuadtree();

public:

	void FillTree();
	void ClearTree();

private:
	
	void AddEntity(Entity* entity);

public:

	std::vector<Entity*> entitiesVector;

private:

	EntityQuadtree* nodes[4];

};

#endif
