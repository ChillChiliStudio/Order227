#include "Building.h"
#include "Render.h"
#include "Textures.h"
#include "Player.h"
#include "Entity_Manager.h"
#include "UserInterface.h"
#include "Text.h"

Building::Building(fPoint position, building_type building_type, entity_faction faction) : Entity(position, entity_type::BUILDING, faction)
{}


bool Building::Start() {

	incomeTimer.Start();
	return true;
}


bool Building::Update(float dt)
{

	if (incomeTimer.ReadSec() >= 2) {

		myApp->player->playerMoney += 60;
		myApp->gui->Moneytext->ChangeString(std::to_string(myApp->player->playerMoney));
		incomeTimer.Start();
	}

	Draw();
	return true;
}


bool Building::CleanUp()
{

	return true;
}


bool Building::Draw()
{

	UpdateBlitOrder();
	myApp->render->Push(order, texture, (int)position.x, (int)position.y, &buildingBlitRect);

	return true;
}


void Building::UpdateBlitOrder() {

	for (int i = 0; i < UNITS_ARRAY_SIZE; ++i) {

		if (myApp->entities->entitiesArray[i] != this) {

			if (this->position.y > myApp->entities->entitiesArray[i]->position.y)
				order += 1;
			else
				order -= 1;

		}
	}
}