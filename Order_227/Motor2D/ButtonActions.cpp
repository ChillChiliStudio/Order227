#include "ButtonActions.h"
#include "App.h"
#include "Entity_Manager.h"
#include "time.h"
#include "Map.h"

void CreateConscript() {
	//srand(time(NULL));
	


	//TODO NEED TO DESHARCODE
	fPoint randomPos = {0,0};
	int temp = rand() % 2;

	switch (temp)
	{
	case 0:
		randomPos.x = 48;
		randomPos.y = rand() % 5 + 40;
		break;
	case 1:
		randomPos.x = rand() % 5 + 43;
		randomPos.y = 45;
		break;
	}

	iPoint tempPoint =myApp->map->MapToWorld(iPoint(randomPos.x, randomPos.y));
	fPoint test = myApp->entities->mainBase->position;
	myApp->entities->ActivateInfantry(fPoint(tempPoint.x,tempPoint.y), infantry_type::BASIC, entity_faction::COMMUNIST);

}