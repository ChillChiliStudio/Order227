#include "Building.h"
#include "Render.h"
#include "Textures.h"
#include "Player.h"
#include"Entity_Manager.h"

Building::Building(fPoint position, building_type building_type, entity_faction faction) : Entity(position, entity_type::BUILDING, faction)
{}


bool Building::Start() {

	texture = myApp->tex->Load("textures/buildings/mainbase.png");
	incomeTimer.Start();
	return true;
}


bool Building::Update(float dt)
{

	if (incomeTimer.ReadSec() >= 2) {

		myApp->player->playerMoney += income;
		incomeTimer.Start();
	}

	Draw();
	//myApp->render->Blit(texture, (int)position.x, (int)position.y, &buildingBlitRect);
	return true;
}


bool Building::CleanUp()
{

	return true;
}


bool Building::Draw()
{

	UpdateBlitOrder();
	myApp->render->Push(order, texture, position.x, position.y, &buildingBlitRect);

	return true;
}


void Building::UpdateBlitOrder() {

	for (int i = 0; i < BUILDINGS_ARRAY_SIZE; ++i) {

		if (myApp->entities->buildingsArray[i] != this) {

			if (this->position.y > myApp->entities->buildingsArray[i]->position.y)
				order += 1;
			else
				order -= 1;

		}
	}

}