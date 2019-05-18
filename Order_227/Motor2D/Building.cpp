#include "Building.h"
#include "Render.h"
#include "Textures.h"
#include "Player.h"
#include "Entity_Manager.h"
#include "UserInterface.h"
#include "Text.h"
#include "Scene.h"

Building::Building()
{}

Building::Building(fPoint position, building_type building_type, entity_faction _faction) : Entity(position, entity_type::BUILDING, faction)
{
	buildingType = building_type;
	faction = entity_faction::COMMUNIST;
}

bool Building::Start() {


	//myApp->gui->CreateLifeBar(fPoint(position.x, position.y), NULL, myApp->entities->lifeBar_tex, &health);
	CurrentAnim = (&myApp->entities->BuildingAnimationArray[int(buildingType)][int(Building_State::SPAWN)]);
	maxHealth = health;

	myApp->entities->buildingsArray;
	myApp->gui->CreateLifeBar(position, NULL, myApp->entities->lifeBar_tex, &health);
	//CurrentAnim = (&myApp->entities->BuildingAnimationArray[int(buildingType)][0]);


	return true;
}

bool Building::Update(float dt)
{

	if(faction == entity_faction::COMMUNIST) {

		if (rewardGiven == false)
			GiveReward();

		if (health <= 0) {

			faction = entity_faction::NEUTRAL;
			health = 0;
			TakeReward();
		}
		else if (health > 0 && health < maxHealth)
			repairable = true;
	}
	else if (faction == entity_faction::NEUTRAL && health > 0)
			faction = entity_faction::COMMUNIST;


	if (repairable) {
		int unitsArraound = 0;
		for (int i = 0; i < myApp->entities->unitPool.size(); i++) {
			if (myApp->entities->unitPool[i].active && this->position.x - myApp->entities->unitPool[i].position.x < 30 && this->position.x - myApp->entities->unitPool[i].position.x > -30
				&& this->position.y - myApp->entities->unitPool[i].position.y < 30 && this->position.y - myApp->entities->unitPool[i].position.y > -30) {
				unitsArraound++;
			}
		}
		for (int i = 0; i < myApp->entities->launcherPool.size(); i++) {
			if (myApp->entities->unitPool[i].active && this->position.x - myApp->entities->unitPool[i].position.x < 30 && this->position.x - myApp->entities->unitPool[i].position.x > -30
				&& this->position.y - myApp->entities->unitPool[i].position.y < 30 && this->position.y - myApp->entities->unitPool[i].position.y > -30) {
				unitsArraound++;
			}
		}
		for (int i = 0; i < unitsArraound; i++) {
			Repair();
		}

	}


	CurrentAnim.AdvanceAnimation(dt);

	Draw();

	if (myApp->map->mapDebugDraw)
		DebugDraw();

	if (CurrentAnim.Finished()==true && health> maxHealth/2) {

		CurrentAnim = (&myApp->entities->BuildingAnimationArray[int(buildingType)][int(Building_State::IDLE)]);
	}
	else if (health <= 0 && destroyed == false) {
		CurrentAnim = (&myApp->entities->BuildingAnimationArray[int(buildingType)][int(Building_State::DESTROYED)]);
		destroyed = true;
	}

	if (this == myApp->entities->mainBase && health <= 0) {

		myApp->gui->LoseIcon->Activate();
		//myApp->hordes->hordeActive = false;
		//myApp->gui->pauseMenuPanel->Deactivate();
		//myApp->gui->MainMenuTemp_Image->Activate();
		//myApp->entities->ResetAll();

	}


	return true;
}


void Building::GiveReward() {

	rewardGiven = true;
	myApp->player->playerIncome += income;

	if (buildingType == building_type::TANK_FACTORY)
		myApp->entities->heavyUnitsUnlocked = true; //TODO: Tocar UI con esto

	if (buildingType == building_type::HTPC)
		unitBuff = true; //Tocar en ActivateUnits() que si es true, le suba velocidad y vida de los buffs

	else if (buildingType == building_type::EPC) {

		healthRecovery *= 1.5;
		myApp->entities->mainBase->health += MainBaseLifeBuff;

		for(int i = 0; i < myApp->entities->buildingsArray.size(); i++) {

			if (myApp->entities->buildingsArray[i].buildingType != building_type::COMMAND_CENTER)
				myApp->entities->buildingsArray[i].maxHealth += StrategicPointsLifeBuff;

			myApp->entities->buildingsArray[i].health = maxHealth;
		}
	}

}


void Building::TakeReward() {

	rewardGiven = false;
	myApp->player->playerIncome -= income;

	if (buildingType == building_type::TANK_FACTORY)
		myApp->entities->heavyUnitsUnlocked = false; //TODO: Tocar UI con esto

	if (buildingType == building_type::HTPC)
		unitBuff = false;

	else if (buildingType == building_type::EPC) {

		healthRecovery /= 1.5;
		myApp->entities->mainBase->health -= MainBaseLifeBuff;

		for (int i = 0; i < myApp->entities->buildingsArray.size(); i++) {

			if (myApp->entities->buildingsArray[i].buildingType != building_type::COMMAND_CENTER)
				myApp->entities->buildingsArray[i].maxHealth -= StrategicPointsLifeBuff;

		}
	}
}


bool Building::CleanUp()
{
	return true;
}

bool Building::Draw()
{


	spriteRect = CurrentAnim.GetTheActualCurrentFrame();
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

float Building::Repair()
{
	health += healthRecovery;

	if (health >= maxHealth)
		health = maxHealth;

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
