#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Fonts.h"
#include "Render.h"
#include "Window.h"
#include "Map.h"
#include "Audio.h"
#include "PathFinding.h"
#include "Entity_Manager.h"
#include "UserInterface.h"
#include "Horde_Manager.h"
#include "ParamBox.h"
#include "Scene.h"
#include "GroupManager.h"


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
	return true;
}

// Called before the first frame
bool Scene::Start()
{
	srand((uint)time(NULL));
	if(myApp->map->Load("Map1_0.tmx") == true)
	{
		int w, h;
		uchar* data = NULL;
		if(myApp->map->CreateWalkabilityMap(w, h, &data))
			myApp->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);
	}



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

	if (myApp->entities->mainBase->health <= 0) {
		myApp->gui->LoseIcon->Activate();
		//myApp->hordes->hordeActive = false;
		//myApp->gui->pauseMenuPanel->Deactivate();
		//myApp->gui->MainMenuTemp_Image->Activate();
		//myApp->entities->ResetAll();
	}
	if (myApp->hordes->roundNumber >= 6) {
		myApp->gui->WinIcon->Activate();
	}
	myApp->map->Draw();

	myApp->gui->Draw();
	return true;
}


// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	//if(myApp->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	//	ret = false;

	myApp->gui->Draw();

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}
