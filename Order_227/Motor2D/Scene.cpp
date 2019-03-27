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


	pugi::xml_parse_result result = SP_Doc.load_file("SpawningPoints.xml");
	
	if (result == NULL)
		LOG("SPAWNING POINTS DOCUMENT COULDN'T LOAD!");
	else {

		pugi::xml_node SP_Node = SP_Doc.child("Spawning_Points");
		for (SP_Node = SP_Node.child("SP"); SP_Node; SP_Node = SP_Node.next_sibling("SP")) {

			int x = SP_Node.attribute("x").as_int();
			int y = SP_Node.attribute("y").as_int();

			Spawning_Point* new_SP = new Spawning_Point(iPoint(x, y));
			SpawningPoints_Array.push_back(new_SP);
		}
	}
	
	debug_tex = myApp->tex->Load("maps/path2.png");

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{

	//Activate Spawn
	if (myApp->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN && SpawningPoints_Array.size() > 0)
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
	for (int i = 0; i < SpawningPoints_Array.size(); i++) {

		myApp->render->DrawQuad(SpawningPoints_Array[i]->SP_Rect, 255, 100, 100);

		//Square Enemies Draw
		if (SpawningPoints_Array[i]->enemies.empty() == false) {

			for (int j = 0; j < SpawningPoints_Array[i]->enemies.size(); j++) {

				myApp->render->DrawQuad(SpawningPoints_Array[i]->enemies[j], 0, 0, 255);
				SpawningPoints_Array[i]->enemies[j].y++;
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
	for (int i = SpawningPoints_Array.size() - 1; i >= 0; i--)
		RELEASE(SpawningPoints_Array[i]);

	SpawningPoints_Array.clear();

	return true;
}


void Scene::ChooseSpawningPoints() {

	round_threat += 10;
	
	//Reseting spawning points (put them at false to choose between them)
	for (int i = 0; i < SpawningPoints_Array.size(); i++)
		SpawningPoints_Array[i]->active = false;
	

	int r1 = rand() % SpawningPoints_Array.size(); //For ranges not starting at 0: rand()%X + Y --> Range of rands between Y and X
	int r2 = rand() % SpawningPoints_Array.size(); //Rand num between 0 and array's size (if 4 SP, then 0-4)

	while (r2 == r1)
		r2 = rand() % SpawningPoints_Array.size(); //This can be pretty unpredictable & uncontrolling shit

	SpawningPoints_Array[r1]->active = true;
	SpawningPoints_Array[r2]->active = true;

	for (int i = 0; i < SpawningPoints_Array.size(); i++)
		if (SpawningPoints_Array[i]->active == true)
			SpawningPoints_Array[i]->FillEnemies(round_threat);

}