#include <iostream> 
#include <sstream>

#include "Defs.h"
#include "Log.h"

#include "Window.h"
#include "Input.h"
#include "Render.h"
#include "Textures.h"
#include "Audio.h"
#include "Scene.h"
#include "Map.h"
#include "Pathfinding.h"
#include "Fonts.h"
#include "Entity_Manager.h"
#include "UserInterface.h"
#include "App.h"
#include "GroupManager.h"
#include "Player.h"
#include "Horde_Manager.h"
#include "MiniMap.h"
#include "Controls.h"

#include "Text.h"

#include "Brofiler/Brofiler.h"

// Constructor
App::App(int argc, char* args[]) : argc(argc), args(args)
{
	input = new Input();
	win = new Window();
	render = new Render();
	tex = new Textures();
	audio = new Audio();
	scene = new Scene();
	map = new Map();
	pathfinding = new PathFinding();
	entities = new Entity_Manager();
	fonts = new Fonts();
	gui = new User_Interface();
	groups = new GroupManager();
	player = new Player();
	hordes = new Horde_Manager();
	minimap = new MiniMap();
	controls = new Controls();

	// Ordered for awake / Start / Update
	// Reverse order of CleanUp
	AddModule(input);
	AddModule(controls);
	AddModule(win);
	AddModule(tex);
	AddModule(map);
	AddModule(pathfinding);
	AddModule(scene);
	AddModule(entities);
	AddModule(audio);
	AddModule(fonts);
	AddModule(gui);
	AddModule(groups);
	AddModule(player);
	AddModule(hordes);
	AddModule(minimap);
	// render last to swap buffer
	AddModule(render);
}

// Destructor
App::~App()
{
	// release modules
	std::list<Module*>::iterator item = modules.begin();
	
	for (; item != modules.end(); item = next(item))
		RELEASE(*item);

	modules.clear();
}

void App::AddModule(Module* module)
{
	module->Init();
	modules.push_back(module);
}

// Called before render is available
bool App::Awake()
{
	bool ret = false;

	pugi::xml_document	config_file;
	pugi::xml_document	save_file;
	pugi::xml_node		config;
	pugi::xml_node		app_config;

	config = myApp->LoadSaveFile(save_file);

	if (!config.empty()) {	//Mark the existance of a save file
		saveFileExists = true;
	}

	config = LoadConfig(config_file);

	if(config.empty() == false)
	{
		// self-config
		ret = true;
		float fps = (float)frame_time.Read();
		app_config = config.child("app");
		title.assign(app_config.child("title").child_value());
		organization.assign(app_config.child("organization").child_value());
		debugMode = app_config.child("debug").attribute("active").as_bool(false);
	
		save_game.assign(app_config.child("save").child_value());
		load_game.assign(app_config.child("load").child_value());

		int cap = app_config.attribute("framerate_cap").as_int(-1);

		if (cap > 0)
			capped_ms = 1000 / cap;
		
	}

	if(ret == true)
	{
		std::list<Module*>::iterator item;
		item = modules.begin();

		while(item != modules.end() && ret == true)
		{
			ret = (*item)->Awake(config.child((*item)->name.data())); 
			item = next(item);
		}


	}

	return ret;
}

// Called before the first frame
bool App::Start()
{

	bool ret = true;
	std::list<Module*>::iterator item = modules.begin();

	while (item != modules.end() && ret == true) {

		ret = (*item)->Start();
		item = next(item);
	}

	time_since_start.Start();
	return ret;
}

// Called each loop iteration
bool App::Update()
{
	bool ret = true;
	PrepareUpdate();

	if(ret == true)
		ret = PreUpdate();

	if(ret == true)
		ret = DoUpdate();

	if(ret == true)
		ret = PostUpdate();

	if (input->GetWindowEvent(WE_QUIT) == true || mustShutDown)
		ret = false;

	FinishUpdate();

	return ret;
}

// ---------------------------------------------
pugi::xml_node App::LoadConfig(pugi::xml_document& config_file) const
{
	pugi::xml_node ret;
	pugi::xml_parse_result result = config_file.load_file("config.xml");

	if(result == NULL)
		LOG("Could not load map xml file config.xml. pugi error: %s", result.description());
	else
		ret = config_file.child("config");

	return ret;
}

pugi::xml_node App::LoadSaveFile(pugi::xml_document& save_file) const
{
	pugi::xml_node ret;
	pugi::xml_parse_result result = save_file.load_file(save_game.c_str());

	if (result == NULL)
		LOG("Could not load save map xml file. pugi error: %s", result.description());
	else
		ret = save_file.child("game_state");

	return ret;
}

// ---------------------------------------------
void App::PrepareUpdate()
{
	frame_count++;
	last_sec_frame_count++;

	dt = frame_time.ReadSec();
	frame_time.Start();
}

// ---------------------------------------------
void App::FinishUpdate()
{
	BROFILER_CATEGORY("App Delay", Profiler::Color::Gray);

	//Savegame
	if (want_to_save == true)
		SavegameNow();

	if (want_to_load == true)
		LoadGameNow();

	//Framerate Calcs
	if (last_sec_frame_time.Read() > 1000) {

		last_sec_frame_time.Start();
		prev_last_sec_frame_count = last_sec_frame_count;
		last_sec_frame_count = 0;
	}

	//For performance information purposes
	float avg_fps = float(frame_count) / time_since_start.ReadSec();
	float secs_since_start = time_since_start.ReadSec();
	uint32 last_frame_ms = frame_time.Read();
	uint32 frames_on_last_update = prev_last_sec_frame_count;

	if (capped_ms > 0 && last_frame_ms < capped_ms)
		SDL_Delay(capped_ms - last_frame_ms);

	if (map->mapDebugDraw) {
		std::string tmp = std::to_string(prev_last_sec_frame_count);
		gui->fpsText->ChangeString(tmp);
	}
}

// Call modules before each loop iteration
bool App::PreUpdate()
{
	BROFILER_CATEGORY("App Pre-Update", Profiler::Color::DarkRed);

	bool ret = true;
	Module* pModule = NULL;
	std::list<Module*>::iterator item = modules.begin();

	for (; item != modules.end() && ret == true; item = next(item)) {

		pModule = (*item);
		if (pModule->active == false)
			continue;

		ret = (*item)->PreUpdate();
	}

	return ret;
}

// Call modules on each loop iteration
bool App::DoUpdate()
{
	BROFILER_CATEGORY("App Update", Profiler::Color::Red);

	bool ret = true;
	Module* pModule = NULL;

	std::list<Module*>::iterator item = modules.begin();
	for (; item != modules.end() && ret == true; item = next(item)) {

		pModule = *item;
		if (pModule->active == false)
			continue;

		ret = (*item)->Update(dt);
	}

	return ret;
}

// Call modules after each loop iteration
bool App::PostUpdate()
{
	BROFILER_CATEGORY("App Post-Update", Profiler::Color::OrangeRed);

	bool ret = true;
	Module* pModule = NULL;
	std::list<Module*>::iterator item = modules.begin();

	for (; item != modules.end() && ret == true; item = next(item)) {

		pModule = *item;
		if (pModule->active == false)
			continue;

		ret = (*item)->PostUpdate();
	}

	return ret;
}

// Called before quitting
bool App::CleanUp()
{
	bool ret = true;
	std::list<Module*>::reverse_iterator item = modules.rbegin();
	
	while (item != modules.rend() && ret == true) {

		ret = (*item)->CleanUp();
		item++;
	}

	return ret;
}

// ---------------------------------------
int App::GetArgc() const
{
	return argc;
}

// ---------------------------------------
const char* App::GetArgv(int index) const
{
	if(index < argc)
		return args[index];
	else
		return NULL;
}

// ---------------------------------------
const char* App::GetTitle() const
{
	return title.data();
}

// ---------------------------------------
const char* App::GetOrganization() const
{
	return organization.data();

}

// ---------------------------------------
float App::GetDT() const
{
	return dt;
}

// Load / Save
void App::LoadGame()
{
	// we should be checking if that file actually exist
	// from the "GetSaveGames" list
	want_to_load = true;
}

void App::SaveGame() const
{
	// we should be checking if that file actually exist
	// from the "GetSaveGames" list ... should we overwrite ?

	want_to_save = true;
}

// ---------------------------------------
bool App::LoadGameNow()
{
	bool ret = false;

	pugi::xml_document data;
	pugi::xml_node root;

	load_game = save_game;	// @Carles

	pugi::xml_parse_result result = data.load_file(load_game.c_str());

	if (result != NULL)
	{
		LOG("Loading new Game State from %s...", load_game.c_str());

		root = data.child("game_state");

		std::list<Module*>::iterator item = modules.begin();
		ret = true;

		for (item; item != modules.end() && ret == true; item = next(item))
		{
			ret = (*item)->Load(root.child((*item)->name.c_str()));
		}

		data.reset();
		if (ret == true)
			LOG("...finished loading");
		else
			LOG("...loading process interrupted with error on module %s", (*item != NULL) ? (*item)->name.c_str() : "unknown");
	}
	else
		LOG("Could not parse game state xml file %s. pugi error: %s", load_game.c_str(), result.description());

	if (myApp->scene->firstGame) {
		myApp->entities->ActivateObjects();
		myApp->scene->firstGame = false;
	}

	//Alternate Start Game that loads game after reading data
	myApp->gui->hordeNumber_Label->ChangeString(std::to_string(myApp->hordes->roundNumber));
	myApp->gui->MainMenuTemp_Image->Deactivate();
	myApp->gui->Current_Screen = Screen_Type::SCREEN_INGAME;
	myApp->scene->SwitchMusic(Screen_Type::SCREEN_INGAME);
	Mix_Resume(-1);
	myApp->scene->ActivateGameOverMusic = true;
	myApp->gui->OnPause = false;
	myApp->gui->WinIcon->Deactivate();
	myApp->gui->LoseIcon->Deactivate();

	want_to_load = false;
	return ret;
}

bool App::SavegameNow()
{
	bool ret = true;

	std::string save_game_file = save_game.c_str();	// @Carles, IMPROVE: Could work with a list of saved files instead of having just one

	LOG("Saving Game State to %s...", save_game_file.c_str());

	// xml object were we will store all data
	pugi::xml_document data;
	pugi::xml_node root;

	root = data.append_child("game_state");

	std::list<Module*>::iterator item = modules.begin();
	ret = true;

	for (item; item != modules.end() && ret == true; item = next(item))
	{
		ret = (*item)->Save(root.append_child((*item)->name.c_str()));
	}

	if (ret == true)
	{
		data.save_file(save_game_file.c_str());
		LOG("... finished saving", );
	}
	else
		LOG("Save process halted from an error in module %s", (*item != NULL) ? (*item)->name.c_str() : "unknown");

	data.reset();
	want_to_save = false;
	return ret;
}