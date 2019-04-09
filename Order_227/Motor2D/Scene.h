#ifndef SCENE_H
#define SCENE_H

#include "Module.h"
#include "Spawning_Point.h"
#include <vector>
#include <time.h>

#include "SDL/include/SDL.h"

struct SDL_Texture;

class Scene : public Module
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

	
	void ChooseSpawningPoints();

	void CreateUnitOnPos(iPoint mouseScreenPos);

	void entitiesSelection();

	SDL_Texture* TestTexture = nullptr;
private:

	//Spawning
	int roundThreat = 0;
	int threatIncremental = 0;
	int roundNumber = 0;
	int rectangle_width;
	int rectangle_height;

	std::vector<Spawning_Point*> SpawningPoints_Array;
	pugi::xml_document SP_Doc;

	iPoint mousePos;
	iPoint mouseScreenPos;

	iPoint origin;
	iPoint rectangle_origin;
};

#endif // SCENE_H