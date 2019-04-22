#ifndef SCENE_H
#define SCENE_H

#include "Module.h"
#include "Spawning_Point.h"
#include <vector>
#include <time.h>
#include "PerfTimer.h"

#include "SDL/include/SDL.h"

struct SDL_Texture;

class Scene : public Module	// TODO: Change class name to Recycle Bin, as it is what everyone uses it for
{

public:

	Scene();

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

public:

	void ChooseSpawningPoints();
	std::vector<Spawning_Point*> SpawningPoints_Array;

private:

	//Spawning & SP
	int roundThreat = 0;
	int threatIncremental = 0;
	int roundNumber = 0;

	pugi::xml_document SP_Doc;

	char* ingame_song3_loop = nullptr;
	char* ingame_song3_intro = nullptr;
	char* ingame_song3_outro = nullptr;

	char* ingame_song4_intro = nullptr;
	char* ingame_song4_loop = nullptr;

};

#endif // SCENE_H
