#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Render.h"
#include "Window.h"
#include "Map.h"
#include "PathFinding.h"
#include "UserInterface.h"
#include "ParamBox.h"
#include "Scene.h"

Scene::Scene() : Module()
{
	name.assign("scene");
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

void EatShit(bool yolo/*, float mhm*/)
{
	//yolo = true;
	//mhm = 0.5f;
	myApp->mustShutDown = yolo;
}

SDL_Rect testSpriteList[4] = { { 0, 100, 200, 200 }, { 0, 100, 200, 200 }, { 0, 100, 200, 200 }, { 0, 100, 200, 200 } };

// Called before the first frame
bool Scene::Start()
{
	if(myApp->map->Load("iso_walk.tmx") == true)
	{
		int w, h;
		uchar* data = NULL;
		if(myApp->map->CreateWalkabilityMap(w, h, &data))
			myApp->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);
	}

	debug_tex = myApp->tex->Load("maps/path2.png");

	myApp->gui->CreateImage({ 150, 150 }, { 0, 0, 300, 300 }, NULL, true);
	myApp->gui->CreateText({ 250, 250 }, "walop", { 255, 0, 0, 255 });
	myApp->gui->CreateCheckBox(&myApp->mustShutDown, { 500, 500 }, testSpriteList);
	//myApp->gui->CreateParamBox<bool>(&EatShit, true, { 500, 500 }, testSpriteList);
	//NewActionBox<bool> (&EatShit, { 500, 500 }, testSpriteList, NULL);

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{

	if(myApp->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		myApp->render->camera.y += 1;

	if(myApp->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		myApp->render->camera.y -= 1;

	if(myApp->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		myApp->render->camera.x += 1;

	if(myApp->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		myApp->render->camera.x -= 1;

	myApp->map->Draw();
	myApp->gui->Draw();

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if(myApp->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}