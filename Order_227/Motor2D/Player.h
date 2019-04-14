#ifndef PLAYER_H_
#define PLAYER_H_

#include "Module.h"
#include <vector>
#include <time.h>
#include "Point.h"
#include "SDL/include/SDL.h"

class Player : public Module {
public:
	Player();
	~Player();

public:

	bool Awake();
	bool Start() override;
	bool PreUpdate() override;
	bool Update(float dt) override;
	bool CleanUp() override;

	void CreateUnitOnPos(iPoint mouseScreenPos);
	void entitiesSelection();

public:

	int rectangle_width;
	int rectangle_height;

	iPoint mousePos;
	iPoint mouseScreenPos;

	iPoint origin;
	iPoint rectangle_origin;

};








#endif 
