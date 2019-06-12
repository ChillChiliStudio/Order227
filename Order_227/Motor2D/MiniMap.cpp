#include "Minimap.h"

#include "App.h"
#include "Window.h"
#include "Render.h"
#include "Textures.h"
#include "Input.h"
#include "UserInterface.h"
#include "Map.h"
#include "Entity.h"
#include "Image.h"
#include "Entity_Manager.h"

#include "Log.h"

#include <list>

MiniMap::MiniMap()
{
	name = "minimap";
	minimapPosition = {370, 570};

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
		minimapScale = minimap_width / map_width;

		x_offset = myApp->map->data.tile_width / 2 * minimapScale ;
		y_offset = myApp->map->data.tile_height / 2 * minimapScale;

		minimap_tex = myApp->tex->Load("maps/minimap_texture.png");
	}

	return true;
}


bool MiniMap::Update(float dt)
{
	if (myApp->gui->Current_Screen != Screen_Type::SCREEN_MAINMENU && myApp->gui->pauseMenuPanel->active!=true) {
		if (myApp->input->GetMouseButtonDown(SDL_BUTTON_LEFT) != KEY_IDLE)
		{
			int map_x, map_y;

			if (MinimapCoords(map_x, map_y) && minimapClicked)
			{
				myApp->render->camera.x = -map_x + myApp->win->width / 2;
				myApp->render->camera.y = -map_y + myApp->win->height / 2;

			}
		}

		if (myApp->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
			minimapClicked = false;
	}
	return true;
}

bool MiniMap::PostUpdate()
{

	//MinimapBorders();


	return true;
}

bool MiniMap::CleanUp()
{
	//myApp->tex->UnLoad(tex);
	myApp->tex->UnLoad(minimap_tex);

	SDL_DestroyTexture(minimap_tex);

	return true;
}

bool MiniMap::Draw() {

	myApp->render->Blit(minimap_tex, minimapPosition.x, minimapPosition.y, NULL, SDL_FLIP_NONE, false);
	DrawEntities();
	DrawCamera();
	return true;
}

bool MiniMap::MinimapCoords(int& map_x, int& map_y)
{
	int mouse_x, mouse_y;
	myApp->input->GetMousePosition(mouse_x, mouse_y);

	if (mouse_x >= minimapPosition.x && mouse_x <= minimap_width+ minimapPosition.x && mouse_y >= minimapPosition.y && mouse_y <= minimap_height+minimapPosition.y)
	{
		if (myApp->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
			minimapClicked = true;

			map_x = ((mouse_x - minimapPosition.x) - minimap_width / 2) / minimapScale;
			map_y = (mouse_y - minimapPosition.y) / minimapScale;
		
	}

	else
		return false;

	return true;
}

void MiniMap::DrawCamera()
{
	SDL_Rect map_camera = { minimapPosition.x + ((-myApp->render->camera.x)) * minimapScale + minimap_width / 2,
								minimapPosition.y + -myApp->render->camera.y * minimapScale,
								 myApp->render->camera.w * minimapScale,
								  myApp->render->camera.h * minimapScale };

	myApp->render->DrawQuad(map_camera, 255, 255, 0, 255, false, false);
}

void MiniMap::MinimapBorders()
{
	myApp->render->DrawLine(minimapPosition.x + x_offset, minimapPosition.y + minimap_height / 2 + y_offset, minimapPosition.x + minimap_width / 2 + x_offset, minimapPosition.y + y_offset, 255, 255, 255, 255, false);
	myApp->render->DrawLine(minimapPosition.x + minimap_width + x_offset, minimapPosition.y + minimap_height / 2 + y_offset, minimapPosition.x + minimap_width / 2 + x_offset, minimapPosition.y + y_offset, 255, 255, 255, 255, false);
	myApp->render->DrawLine(minimapPosition.x + minimap_width + x_offset, minimapPosition.y+ minimap_height / 2 + y_offset, minimapPosition.x + minimap_width / 2 + x_offset, minimapPosition.y + minimap_height + y_offset, 255, 255, 255, 255, false);
	myApp->render->DrawLine(minimapPosition.x + x_offset, minimapPosition.y+ minimap_height / 2 + y_offset, minimapPosition.x + minimap_width / 2 + x_offset, minimapPosition.y + minimap_height + y_offset, 255, 255, 255, 255, false);
}

void MiniMap::DrawEntities()
{
	SDL_Rect entityRect = { 0,0,3,3 };

	for (int i = 0; i < myApp->entities->unitPool.size();++i){

		if (myApp->entities->unitPool[i].active){

			entityRect.x = myApp->entities->unitPool[i].position.x*minimapScale + minimapPosition.x + x_offset + minimap_width / 2;
			entityRect.y = myApp->entities->unitPool[i].position.y*minimapScale + minimapPosition.y + y_offset;

			switch (myApp->entities->unitPool[i].faction)
			{
			case entity_faction::CAPITALIST:
				myApp->render->DrawQuad(entityRect, 255, 0, 0, 255, true, false);
				break;

			case entity_faction::COMMUNIST:
				myApp->render->DrawQuad(entityRect, 0, 255, 0, 255, true, false);
				break;
			}

		}
	}

	for (int i = 0; i < myApp->entities->launcherPool.size(); ++i) {

		if (myApp->entities->launcherPool[i].active) {

			entityRect.x = myApp->entities->launcherPool[i].position.x*minimapScale + minimapPosition.x + x_offset + minimap_width / 2;
			entityRect.y = myApp->entities->launcherPool[i].position.y*minimapScale + minimapPosition.y + y_offset;

			switch (myApp->entities->launcherPool[i].faction)
			{
			case entity_faction::CAPITALIST:
				myApp->render->DrawQuad(entityRect, 255, 0, 0, 255, true, false);
				break;

			case entity_faction::COMMUNIST:
				myApp->render->DrawQuad(entityRect, 0, 255, 0, 255, true, false);
				break;
			}

		}
	}

	entityRect.w = 8;
	entityRect.h = 8;

	for (int i = 0; i < myApp->entities->buildingsArray.size(); ++i)
	{
		entityRect.x = myApp->entities->buildingsArray[i].position.x*minimapScale + minimapPosition.x + x_offset + minimap_width / 2;
		entityRect.y = myApp->entities->buildingsArray[i].position.y*minimapScale + minimapPosition.y + y_offset;

		switch (myApp->entities->buildingsArray[i].faction)
		{
		case entity_faction::COMMUNIST:
			myApp->render->DrawQuad(entityRect,0, 255, 0, 255, true, false);
			myApp->render->DrawQuad(entityRect, 255, 255, 255, 255, false, false);
			break;
		case entity_faction::NEUTRAL:
			myApp->render->DrawQuad(entityRect, 140, 140, 140, 255, true, false);
			myApp->render->DrawQuad(entityRect, 255, 255, 255, 255, false, false);

			break;
		}
	}
}