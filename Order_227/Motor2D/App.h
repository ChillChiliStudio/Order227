#ifndef APP_H
#define APP_H

#include "Module.h"
#include "PugiXml\src\pugixml.hpp"

#include "Timer.h"
#include <list>


// Modules
class Window;
class Input;
class Render;
class Textures;
class Audio;
class Scene;
class Map;
class PathFinding;
class Fonts;
class Entity_Manager;
class User_Interface;
class GroupManager;
class Player;
class Horde_Manager;
class MiniMap;
class Video;
class Controls;

class App
{
public:

	// Constructor
	App(int argc, char* args[]);

	// Destructor
	virtual ~App();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update();

	// Called before quitting
	bool CleanUp();

	// Add a new module to handle
	void AddModule(Module* module);

	// Exposing some properties for reading
	int GetArgc() const;
	const char* GetArgv(int index) const;
	const char* GetTitle() const;
	const char* GetOrganization() const;
	float GetDT() const;

	void LoadGame();
	void SaveGame() const;

	// Load xml files
	pugi::xml_node LoadConfig(pugi::xml_document&) const;
	pugi::xml_node LoadSaveFile(pugi::xml_document&) const;

private:
	// Call modules before each loop iteration
	void PrepareUpdate();

	// Call modules before each loop iteration
	void FinishUpdate();

	// Call modules before each loop iteration
	bool PreUpdate();

	// Call modules on each loop iteration
	bool DoUpdate();

	// Call modules after each loop iteration
	bool PostUpdate();

	// Load / Save
	bool LoadGameNow();
	bool SavegameNow();

public:

	// Modules
	Window*				win = nullptr;
	Input*				input = nullptr;
	Render*				render = nullptr;
	Textures*			tex = nullptr;
	Audio*				audio = nullptr;
	Scene*				scene = nullptr;
	Map*				map = nullptr;
	PathFinding*		pathfinding = nullptr;
	Entity_Manager*		entities = nullptr;
	Fonts*			    fonts = nullptr;
	User_Interface*  	gui = nullptr;
	Video*				video = nullptr;
	GroupManager*       groups = nullptr;
	Player*             player = nullptr;
	Horde_Manager*		hordes = nullptr;
	MiniMap*			minimap = nullptr;
	Controls*			controls = nullptr;

	bool debugMode;
	bool saveFileExists = false;
	bool mustShutDown = false;
	bool playingGame = false;

private:

	int					argc;
	char**				args;

	std::string			title;
	std::string			organization;

	std::list<Module*>modules;

	//Save & Load
	mutable bool		want_to_save;
	bool				want_to_load;

	mutable std::string	save_game;
	std::string			load_game;

	//Framerate
	uint				frame_count = 0;
	Timer				time_since_start;
	Timer				frame_time;
	Timer				last_sec_frame_time;

	uint32				last_sec_frame_count = 0;
	uint32				prev_last_sec_frame_count = 0;
	float				dt = 0.0f;
	int					capped_ms = -1;

};

extern App* myApp;

#endif
