#ifndef MAIN_BASE_H
#define MAIN_BASE_H

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"

class Main_Base : public Entity
{

public:

	Main_Base(fPoint pos);
	~Main_Base();

	bool Update(float dt);
	bool Draw();

	//bool SpawnTroop()

public:

	SDL_Rect MainBaseRect;


};

#endif