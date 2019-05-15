#ifndef _TILEQUADTREE_H__
#define _TILEQUADTREE_H__

#include "Quadtree.h"

struct TileData
{
	TileData() {}
	TileData(uint id, iPoint position) :id(id), position(position) {};

	uint id;
	iPoint position;
};

class TileQuadtree : public Quadtree
{
public:

	TileQuadtree(uint maxLevels, SDL_Rect section, uint size, uint level = 1);

	void CleanUp();

	void Split();

	void InsertTile(TileData tile);

	void DrawMap();

	//DEBUG

	void DrawQuadtree();



protected:

	TileQuadtree* nodes[4];		//Subnodes of the current node

	TileData* tiles;			//Array which contains all the tiles

	uint	size;				//Amount of tiles this node can store
	uint	tilesContained;	//Amount of tiles this node currently stores


};

#endif // !_TILEQUADTREE_H__

