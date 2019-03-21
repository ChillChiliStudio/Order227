#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Render.h"
#include "Window.h"
#include "Map.h"
#include "PathFinding.h"
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

// Called before the first frame
bool Scene::Start()
{
	srand(time(NULL));

	if(myApp->map->Load("iso_walk.tmx") == true)
	{
		int w, h;
		uchar* data = NULL;
		if(myApp->map->CreateWalkabilityMap(w, h, &data))
			myApp->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);
	}

	int posX = 80;
	int posY = 120;

	for (int i = 0; i < 4; i++) {

		Spawning_Point* SpawnPoint = new Spawning_Point(iPoint(posX, posY));

		SpawnPoint->active = false;
		SpawnPoint->threat = 10;
		posX += 40;
		posY += 40;
		posX *= -1;

		SpawningPointArray.push_back(SpawnPoint);
	}
	
	debug_tex = myApp->tex->Load("maps/path2.png");

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{

	//Activate Spawn
	if (myApp->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
		ChooseSpawningPoints();

	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{

	if(myApp->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		myApp->render->camera.y += 10;

	if(myApp->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		myApp->render->camera.y -= 10;

	if(myApp->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		myApp->render->camera.x += 10;

	if(myApp->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		myApp->render->camera.x -= 10;

	myApp->map->Draw();

	//Spawn Point Draw
	for (int i = 0; i < SpawningPointArray.size(); i++) {

		myApp->render->DrawQuad(SpawningPointArray[i]->SP_Rect, 255, 100, 100);

		//Square Enemies Draw
		if (SpawningPointArray[i]->enemies.empty() == false) {

			for (int j = 0; j < SpawningPointArray[i]->enemies.size(); j++) {

				myApp->render->DrawQuad(SpawningPointArray[i]->enemies[j], 0, 0, 255);
				SpawningPointArray[i]->enemies[j].y++;
			}
		}

	}
	
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

	//for (int i = 0; i < SpawningPointArray.size(); i++)
	for (int i = SpawningPointArray.size() - 1; i >= 0; i--)
		RELEASE(SpawningPointArray[i]);

	SpawningPointArray.clear();

	return true;
}


void Scene::ChooseSpawningPoints() {

	for (int i = 0; i < SpawningPointArray.size(); i++)
		SpawningPointArray[i]->active = false;

	int r1 = rand() % SpawningPointArray.size(); //For ranges not starting at 0: rand()%X + Y --> Range of rands between Y and X
	int r2 = rand() % SpawningPointArray.size(); //Rand num between 0 and array's size (if 4 SP, then 0-4)

	while (r2 == r1)
		r2 = rand() % SpawningPointArray.size(); //This can be pretty unpredictable & uncontrolling shit

	SpawningPointArray[r1]->active = true;
	SpawningPointArray[r2]->active = true;

	for (int i = 0; i < SpawningPointArray.size(); i++)
		if (SpawningPointArray[i]->active == true)
			SpawningPointArray[i]->FillEnemies();

}