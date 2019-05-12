#include "Building.h"
#include "Render.h"
#include "Textures.h"
#include "Player.h"
#include "Entity_Manager.h"
#include "UserInterface.h"
#include "Text.h"

Building::Building()
{}

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

	if (myApp->map->mapDebugDraw) {
		DebugDraw();
	}

	return true;
}

bool Building::CleanUp()
{
	return true;
}

bool Building::Draw()
{
	myApp->render->Push(order, texture, (int)position.x, (int)position.y, &spriteRect);

	return true;
}

bool Building::DebugDraw()
{
	return myApp->render->DrawQuad(entityRect, 255, 0, 0, 255, false);
}

float Building::Hurt(float damage)
{
	health -= damage;

	if (health <= 0.0f) {
		//Die();
	}

	return health;
}

bool Building::IsDead()
{
	bool ret = false;

	if (health <= 0.0f) {
		ret = true;
	}

	return ret;
}

bool Building::IsVisible()
{
	return true;	//TODO: Make function with Fog of War
}