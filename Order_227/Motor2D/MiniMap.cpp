#include "Minimap.h"

#include "App.h"
#include "Window.h"
#include "Render.h"
#include "Textures.h"
#include "Input.h"

#include "Map.h"
#include "Entity.h"
#include "Entity_Manager.h"

#include "Log.h"

#include <list>

MiniMap::MiniMap()
{
	name = "minimap";
	minimapPosition = {442, 570};

}

MiniMap::~MiniMap() {}

bool MiniMap::Awake(pugi::xml_node & config)
{
	minimap_width = config.attribute("width").as_int();
	minimap_height = config.attribute("height").as_int();

	return true;
}

bool MiniMap::Start()
{
	if (myApp->map->active)
	{
		map_width = myApp->map->data.width * myApp->map->data.tile_width;
		minimap_scale = minimap_width / map_width;

		x_offset = myApp->map->data.tile_width / 2 * minimap_scale ;
		y_offset = myApp->map->data.tile_height / 2 * minimap_scale;

		minimap_tex = myApp->tex->Load("maps/minimap_texture.png");
	}

	return true;
}


bool MiniMap::Update(float dt)
{
	if (myApp->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		int map_x, map_y;

		if (MinimapCoords(map_x, map_y))
		{
			myApp->render->camera.x = -map_x + myApp->win->width / 2;
			myApp->render->camera.y = -map_y + myApp->win->height / 2;
		}
	}

	return true;
}

bool MiniMap::PostUpdate()
{
	myApp->render->Blit(minimap_tex, minimapPosition.x, minimapPosition.y, NULL, SDL_FLIP_NONE, false);
	MinimapBorders();
	//DrawEntities();
	DrawCamera();

	return true;
}

bool MiniMap::CleanUp()
{
	//myApp->tex->UnLoad(tex);
	myApp->tex->UnLoad(minimap_tex);

	SDL_DestroyTexture(minimap_tex);

	return true;
}

bool MiniMap::MinimapCoords(int& map_x, int& map_y)
{
	int mouse_x, mouse_y;
	myApp->input->GetMousePosition(mouse_x, mouse_y);

	if (mouse_x >= minimapPosition.x && mouse_x <= minimap_width+ minimapPosition.x && mouse_y >= minimapPosition.y && mouse_y <= minimap_height+minimapPosition.y)
	{
		map_x =  ((mouse_x-minimapPosition.x) - minimap_width / 2) / minimap_scale;
		map_y = (mouse_y-minimapPosition.y) / minimap_scale;
	}

	else
		return false;

	return true;
}

void MiniMap::DrawCamera()
{
	SDL_Rect map_camera = { minimapPosition.x + ((-myApp->render->camera.x)) * minimap_scale + minimap_width / 2,
								minimapPosition.y + -myApp->render->camera.y * minimap_scale,
								 myApp->render->camera.w * minimap_scale,
								  myApp->render->camera.h * minimap_scale };

	myApp->render->DrawQuad(map_camera, 255, 255, 0, 255, false, false);
}

void MiniMap::MinimapBorders()
{
	myApp->render->DrawLine(minimapPosition.x + x_offset, minimapPosition.y + minimap_height / 2 + y_offset, minimapPosition.x + minimap_width / 2 + x_offset, minimapPosition.y + y_offset, 255, 255, 255, 255, false);
	myApp->render->DrawLine(minimapPosition.x + minimap_width + x_offset, minimapPosition.y + minimap_height / 2 + y_offset, minimapPosition.x + minimap_width / 2 + x_offset, minimapPosition.y + y_offset, 255, 255, 255, 255, false);
	myApp->render->DrawLine(minimapPosition.x + minimap_width + x_offset, minimapPosition.y+ minimap_height / 2 + y_offset, minimapPosition.x + minimap_width / 2 + x_offset, minimapPosition.y + minimap_height + y_offset, 255, 255, 255, 255, false);
	myApp->render->DrawLine(minimapPosition.x + x_offset, minimapPosition.y+ minimap_height / 2 + y_offset, minimapPosition.x + minimap_width / 2 + x_offset, minimapPosition.y + minimap_height + y_offset, 255, 255, 255, 255, false);
}

//void MiniMap::DrawEntities()
//{
//	int pos_x, pos_y;
//
//	for (std::list<j2Entity*>::iterator item = myApp->entity_manager->entities.begin(); item != myApp->entity_manager->entities.end(); ++item)
//	{
//		// TODO 6: Initialize the variables "pos_x" and "pos_y" to get the position of an entity IN the minimap
//		pos_x = (*item)->position.x * minimap_scale;
//		pos_y = (*item)->position.y * minimap_scale;
//
//		// TODO 7: Fill the missing parameters of DrawQuad() function.
//		// Take into account that it is an isometric map
//		if ((*item)->type == ENTITY_TYPE::ENEMY)
//			myApp->render->DrawQuad({ pos_x + minimap_width / 2, pos_y - y_offset, 4, 4 }, 255, 0, 0, 255, true, false);
//
//		else if ((*item)->type == ENTITY_TYPE::PLAYER)
//			myApp->render->DrawQuad({ pos_x + minimap_width / 2, pos_y - y_offset, 4, 4 }, 0, 255, 0, 255, true, false);
//	}
//}