#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Fonts.h"
#include "Render.h"
#include "Window.h"
#include "Map.h"
#include "PathFinding.h"
#include "Entity_Manager.h"
#include "UserInterface.h"
#include "ButtonActions.h"
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

// Called before the first frame
bool Scene::Start()
{
	srand(time(NULL));
	if(myApp->map->Load("iso.tmx") == true)
	{
		int w, h;
		uchar* data = NULL;
		if(myApp->map->CreateWalkabilityMap(w, h, &data))
			myApp->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);
	}

	//Spawning Points Load
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

	//RECODE OR EREASE
	//myApp->entities->CreateEntity(entity_type::OBJECT, fPoint(10.0f, 10.0f));

	TestTexture = myApp->tex->Load("textures/troops/allied/gi.png");

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	//RECODE OR EREASE
	
	////Activate Spawn with F --> Just for DEBUG PURPOSES
	//if (myApp->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN && SpawningPoints_Array.size() > 0)
	//	ChooseSpawningPoints();

	////Delete enemies with J --> Just for DEBUG PURPOSES
	//if (myApp->input->GetKey(SDL_SCANCODE_J) == KEY_DOWN && myApp->entities->entities_list.size() > 0) {
	//	for (std::list<Entity*>::iterator item = myApp->entities->entities_list.begin(); item != myApp->entities->entities_list.end(); item = next(item)) {
	//		if ((*item)->GetType() == entity_type::UNIT) {
	//			myApp->entities->DestroyEntity(*item);
	//		}
	//	}
	//}

	//if (myApp->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
	//	for (std::list<Entity*>::iterator item = myApp->entities->entities_list.begin(); item != myApp->entities->entities_list.end(); item = next(item)) {
	//		if ((*item)->GetType() == entity_type::UNIT) {
	//			Unit* tmpUnit = (Unit*)(*item);

	//			/*int mouseX, mouseY;	//TODO Carles: DO NOT TOUCH >:c
	//			myApp->input->GetMousePosition(mouseX, mouseY);
	//			iPoint wololo = myApp->render->ScreenToWorld(mouseX, mouseY);
	//			myApp->map->WorldToMap(wololo.x, wololo.y);
	//			tmpUnit->StartMove(wololo);*/
	//		}
	//	}
	//}

	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{

	if(myApp->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		myApp->render->camera.y += 200*dt;

	if(myApp->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		myApp->render->camera.y -= 200*dt;

	if(myApp->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		myApp->render->camera.x += 200*dt;

	if(myApp->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		myApp->render->camera.x -= 200*dt;

	myApp->map->Draw();


	//RECODE OR EREASE
	////Spawn Point Draw
	//for (int i = 0; i < SpawningPoints_Array.size(); i++) {

	//	myApp->render->DrawQuad(SpawningPoints_Array[i]->SP_Rect, 255, 100, 100);

	//	if (SpawningPoints_Array[i]->Enemies_to_Spawn.size() > 0 && SpawningPoints_Array[i]->SpawnTime.Read() > 500) {

	//		fPoint SP_Pos = fPoint(SpawningPoints_Array[i]->position.x, SpawningPoints_Array[i]->position.y);
	//		SpawningPoints_Array[i]->SpawnTime.Start();

	//		myApp->entities->CreateUnit(unit_type::INFANTRY, fPoint(SP_Pos), faction_enum::CAPITALIST);
	//		SpawningPoints_Array[i]->Enemies_to_Spawn.pop_back();

	//	}
	//}

	////mouse selection code bellow
	//
	//myApp->input->GetMousePosition(mousePos.x, mousePos.y);
	//mouseScreenPos = myApp->render->ScreenToWorld(mousePos.x, mousePos.y);

	//if (myApp->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN) {
	//	CreateUnitOnPos(mouseScreenPos);
	//}

	////group movement bellow
	//SDL_Rect goToRect = { 10,10,20,10 };
	//if (myApp->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_REPEAT) {
	//	goToRect.x = mouseScreenPos.x;
	//	goToRect.y = mouseScreenPos.y;	
	//	myApp->render->DrawQuad(goToRect, 0, 255, 100, 255, true);
	//}

	entitiesSelection();
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

	for (int i = SpawningPoints_Array.size() - 1; i >= 0; i--)
		RELEASE(SpawningPoints_Array[i]);

	SpawningPoints_Array.clear();
	myApp->tex->UnLoad(TestTexture);

	return true;
}

void Scene::ChooseSpawningPoints()
{
	//Restarting round if reached 20 - for MVP this should be 5!
	if (roundNumber == 5) {

		roundNumber = 0;
		roundThreat = 0;
	}

	//This is VERY HARDCODED - I'm not proud of this, but will work for now (fck this shit) - Is just the manager of round threat TODO
	if (roundNumber == 0 || roundNumber == 1)
		threatIncremental = 10;
	else if (roundNumber == 3 || roundNumber == 6)
		threatIncremental = 20;
	else if (roundNumber == 9 || roundNumber == 11)
		threatIncremental = 30;
	else if (roundNumber == 15 || roundNumber == 19)
		threatIncremental = 40;
	else
		threatIncremental = 0;

	roundNumber++;
	roundThreat += threatIncremental;
	
	//Reseting spawning points (put them at false to choose between them)
	for (int i = 0; i < SpawningPoints_Array.size(); i++)
		SpawningPoints_Array[i]->active = false;
	

	//If we want more spawning points just put more of these
	int r1 = rand() % SpawningPoints_Array.size(); //For ranges not starting at 0: rand()%X + Y --> Range of rands between Y and X
	int r2 = rand() % SpawningPoints_Array.size(); //Rand num between 0 and array's size (if 4 SP, then 0-4)

	while (r2 == r1)
		r2 = rand() % SpawningPoints_Array.size(); //This can be pretty unpredictable & uncontrolling shit

	SpawningPoints_Array[r1]->active = true;
	SpawningPoints_Array[r2]->active = true;

	for (int i = 0; i < SpawningPoints_Array.size(); i++)
		if (SpawningPoints_Array[i]->active == true)
			SpawningPoints_Array[i]->FillEnemies(roundThreat);

}

void Scene::CreateUnitOnPos(iPoint mouseScreenPos_)
{
	fPoint position;
	position.x = (float)mouseScreenPos_.x-30;
	position.y = (float)mouseScreenPos_.y-35;
	myApp->entities->CreateSoldier(position, soldier_type::BASIC, entity_faction::NEUTRAL);
}

void Scene::entitiesSelection()
{
	SDL_Rect SRect;
	rectangle_width = mouseScreenPos.x - rectangle_origin.x;
	rectangle_height = mouseScreenPos.y - rectangle_origin.y;

	if (myApp->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
		rectangle_origin = mouseScreenPos;

	else if (std::abs(rectangle_width) >= 5 && std::abs(rectangle_height) >= 5 && myApp->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
		// --- Rectangle size ---
		int width = mouseScreenPos.x - rectangle_origin.x;
		int height = mouseScreenPos.y - rectangle_origin.y;

		// --- Draw Rectangle ---
		SRect = { rectangle_origin.x, rectangle_origin.y, width, height };
		myApp->render->DrawQuad(SRect, 0, 200, 100, 255, false);

		// --- Once we get to the negative side of SRect numbers must be adjusted ---
		if (width < 0) {
			SRect.x = mouseScreenPos.x;
			SRect.w *= -1;
		}
		if (height < 0) {
			SRect.y = mouseScreenPos.y;
			SRect.h *= -1;
		}
		//units selection
		myApp->entities->SelectUnit(SRect);
	}
	// 1 click selection
	if (myApp->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
		myApp->entities->SelectUnit(mouseScreenPos);
	}

	//groups management
	if (myApp->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
		myApp->entities->CreateGroupForPlayer();	
	}
}
