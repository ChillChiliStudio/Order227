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




TileQuadtree::TileQuadtree(uint max_levels, SDL_Rect section, uint size, uint level) : Quadtree(max_levels, section, level), size(size)
{
	//Set the subnodes as nullptr
	for (int i = 0; i < 4; i++)
		nodes[i] = nullptr;

	/*We will only store the tiles in the bottom nodes,
	so, if this node will have subnodes, it won't need room for tiles*/
	if (level != max_levels)
		tiles = NULL;

	else
	{
		tiles = new TileData[size];

		for (int i = 0; i < size; ++i)
			tiles[i] = TileData(0, { 0,0 });

	}

	tiles_contained = 0;

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
	if (level < max_levels && divided == false)
	{
		nodes[0] = new TileQuadtree(max_levels, { section.x,section.y, section.w / 2, section.h / 2 }, size / 4, level + 1);
		nodes[1] = new TileQuadtree(max_levels, { section.x + section.w / 2,section.y, section.w / 2,section.h / 2 }, size / 4, level + 1);
		nodes[2] = new TileQuadtree(max_levels, { section.x,section.y + section.h / 2 , section.w / 2, section.h / 2 }, size / 4, level + 1);
		nodes[3] = new TileQuadtree(max_levels, { section.x + section.w / 2,section.y + section.h / 2, section.w / 2,section.h / 2 }, size / 4, level + 1);
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
	if (level == max_levels)
	{
		tiles[tiles_contained++] = tile;
	}

	//In case there are lower subnodes, it will be stored there
	else if (level < max_levels)
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
					if (nodes[i]->tiles_contained < nodes[i]->size)
					{
						nodes[i]->InsertTile(tile);
						break;
					}
				}
			}
		}
	}
}

bool TileQuadtree::CheckVisibility()
{
	uint screen_w;
	uint screen_h;
	myApp->win->GetWindowSize(screen_w, screen_h);

	if (-myApp->render->camera.x > (section.x + section.w) ||
		(-myApp->render->camera.x + int(screen_w)) < section.x ||
		-myApp->render->camera.y > (section.y + section.h) ||
		(-myApp->render->camera.y + int(screen_h)) < section.y)
		return false;

	return true;
}

void TileQuadtree::DrawMap()
{
	if (CheckVisibility())
	{
		if (level == max_levels)
		{
			for (int i = 0; i < tiles_contained; ++i)
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

			if (level < max_levels - 1)
			{
				for (int i = 0; i < 4; ++i)
				{
					nodes[i]->DrawQuadtree();
				}
			}
		}
	}
}