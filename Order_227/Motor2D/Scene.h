#ifndef SCENE_H
#define SCENE_H

#include "Module.h"
#include "Spawning_Point.h"
#include <vector>
#include <ctime>
#include "Audio.h"
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



private:

	//Music Management
	void ManageMusic();
	//char* ChooseNextSong(char* currSong);
	bool LoadGameMusic();

	pugi::xml_document Music_Doc;
	std::list<Music_Song*> gameSongslist;

	Music_Song *current_song = nullptr;
	Music_Track *current_track = nullptr;
	
};

#endif // SCENE_H
