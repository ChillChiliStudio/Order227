#include "Defs.h"
#include "Point.h"
#include "Log.h"
#include "SDL/include/SDL.h"
#include <cassert>

#include "App.h"
#include "Map.h"
#include "Render.h" 
#include "Window.h"
#include "Textures.h"

#include "TileQuadtree.h"



TileQuadtree::TileQuadtree(uint maxLevels, SDL_Rect section, uint size, uint level) : Quadtree(maxLevels, section, level), size(size)
{
	//Set the subnodes as nullptr
	for (int i = 0; i < 4; i++)
		nodes[i] = nullptr;

	/*We will only store the tiles in the bottom nodes,
	so, if this node will have subnodes, it won't need room for tiles*/
	if (level != maxLevels)
		tiles = NULL;

	else
	{
		tiles = new TileData[size];

		for (uint i = 0; i < size; ++i)
			tiles[i] = TileData(0, { 0,0 });

	}

	tilesContained = 0;

}

void TileQuadtree::CleanUp()
{
	if (divided == true)
	{
		for (int i = 0; i < 4; i++)
		{
			nodes[i]->CleanUp();
			nodes[i] = nullptr;
		}

	}
	RELEASE(tiles);
}

void TileQuadtree::Split()
{
	if (level < maxLevels && divided == false)
	{
		nodes[0] = new TileQuadtree(maxLevels, { section.x,section.y, section.w / 2, section.h / 2 }, size / 4, level + 1);
		nodes[1] = new TileQuadtree(maxLevels, { section.x + section.w / 2,section.y, section.w / 2,section.h / 2 }, size / 4, level + 1);
		nodes[2] = new TileQuadtree(maxLevels, { section.x,section.y + section.h / 2 , section.w / 2, section.h / 2 }, size / 4, level + 1);
		nodes[3] = new TileQuadtree(maxLevels, { section.x + section.w / 2,section.y + section.h / 2, section.w / 2,section.h / 2 }, size / 4, level + 1);
		divided = true;
	}

}

void TileQuadtree::InsertTile(TileData tile)
{
	SDL_Rect tile_rect;
	tile_rect.x = tile.position.x;
	tile_rect.y = tile.position.y;
	tile_rect.w = myApp->map->data.tile_width;
	tile_rect.h = myApp->map->data.tile_height;

	//If the node is in the lowest level, store the tile here
	if (level == maxLevels)
	{
		tiles[tilesContained++] = tile;
	}

	//In case there are lower subnodes, it will be stored there
	else if (level < maxLevels)
	{
		if (this->CheckTouch({ tile_rect }))
		{
			//If there are no subnodes yet, split the current node
			if (divided == false)
				Split();

			for (int i = 3; i >= 0; --i)
			{
				if (nodes[i]->CheckTouch({ tile_rect }))
				{
					if (nodes[i]->tilesContained < nodes[i]->size)
					{
						nodes[i]->InsertTile(tile);
						break;
					}
				}
			}
		}
	}
}

void TileQuadtree::DrawMap()
{
	if (CheckVisibility())
	{
		if (level == maxLevels)
		{
			for (uint i = 0; i < tilesContained; ++i)
			{
				TileData tile = tiles[i];
				TileSet* tileset = myApp->map->GetTilesetFromTileId(tile.id);
				SDL_Rect rect = tileset->GetTileRect(tile.id);
				
				myApp->render->Blit(tileset->texture, tile.position.x+tileset->offset_x, tile.position.y+tileset->offset_y, &rect);
			}
		}

		if (divided == true)
		{
			for (int i = 0; i < 4; ++i)
			{
				nodes[i]->DrawMap();
			}
		}
	}
}

void TileQuadtree::DrawQuadtree()
{
	if (CheckVisibility() == true)
	{
		if (level == 1)
		{
			myApp->render->DrawLine(section.x, section.y, section.x, section.y + section.h, 0, 0, 255);
			myApp->render->DrawLine(section.x, section.y, section.x + section.w, section.y, 0, 0, 255);
			myApp->render->DrawLine(section.x, section.y + section.h, section.x + section.w, section.y + section.h, 0, 0, 255);
			myApp->render->DrawLine(section.x + section.w, section.y, section.x + section.w, section.y + section.h, 0, 0, 255);
		}

		if (divided == true)
		{
			myApp->render->DrawLine(section.x + section.w / 2, section.y, section.x + section.w / 2, section.y + section.h, 0, 0, 255);
			myApp->render->DrawLine(section.x, section.y + section.h / 2, section.x + section.w, section.y + section.h / 2, 0, 0, 255);

			if (level < maxLevels - 1)
			{
				for (int i = 0; i < 4; ++i)
				{
					nodes[i]->DrawQuadtree();
				}
			}
		}
	}
}

bool TileQuadtree::CheckTouch(const SDL_Rect& rect)
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