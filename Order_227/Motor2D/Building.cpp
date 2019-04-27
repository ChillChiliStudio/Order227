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
	myApp->gui->CreateLifeBar(fPoint(position.x, position.y), NULL, myApp->entities->lifeBar_tex, &health);

	return true;
}


bool Building::Update(float dt)
{
	if (incomeTimer.ReadSec() >= 2) {

		myApp->player->playerMoney += 80;
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

	for (int i = 0; i < HALF_UNITS_INITIAL_SIZE; ++i) {

		if (myApp->entities->EntitiesArray[i] != this) {

			if (this->position.y > myApp->entities->EntitiesArray[i]->position.y)
				order += 1;
			else
				order -= 1;

		}
	}
}