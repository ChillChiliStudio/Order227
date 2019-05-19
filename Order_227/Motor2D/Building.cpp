#include "Building.h"
#include "Render.h"
#include "Textures.h"
#include "Player.h"
#include "Entity_Manager.h"
#include "UserInterface.h"
#include "Text.h"
#include "Scene.h"
#include "Geometry.h"

Building::Building()
{}

Building::Building(fPoint position, building_type building_type, entity_faction _faction) : Entity(position, entity_type::BUILDING, faction)
{
	buildingType = building_type;
	faction = _faction;
}

bool Building::Start() {


	//myApp->gui->CreateLifeBar(fPoint(position.x, position.y), NULL, myApp->entities->lifeBar_tex, &health);
	CurrentAnim = (&myApp->entities->BuildingAnimationArray[int(buildingType)][int(Building_State::SPAWN)]);

	myApp->entities->buildingsArray;
	myApp->gui->CreateLifeBar(position, NULL, myApp->entities->lifeBar_tex, &health);
	//CurrentAnim = (&myApp->entities->BuildingAnimationArray[int(buildingType)][0]);

	if (buildingType == building_type::COMMAND_CENTER) {

		int Aux = myApp->audio->VarsXsound_Buildings[int(buildingType)][(int)BuildingsType_Sounds::SPAWN];
		myApp->audio->PlayFx(myApp->audio->SoundBuilding_Array[int(buildingType)][(int)BuildingsType_Sounds::SPAWN][rand() % Aux]);

	}
	else
		health = 0;

	rewardGiven = false;

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
		else if (health >= 0 && health < maxHealth && repairable == false)
			repairable = true;
	}
	else if (faction == entity_faction::NEUTRAL && health > (maxHealth/2))
			faction = entity_faction::COMMUNIST;


	if (repairable == true) {

		uint unitsArround = 0;

		for (int i = 0; i < myApp->entities->unitPool.size(); i++) {

			if (myApp->entities->unitPool[i].active && myApp->entities->unitPool[i].faction == entity_faction::COMMUNIST) {

				if (InsideSquareRadius(centerPos, repairRadius, myApp->entities->unitPool[i].centerPos)
					&& InsideRadius(centerPos, repairRadius, myApp->entities->unitPool[i].centerPos)) {
					unitsArround++;
				}
			}
		}
		for (int i = 0; i < myApp->entities->launcherPool.size(); i++) {

			if (myApp->entities->launcherPool[i].active == true && myApp->entities->launcherPool[i].faction == entity_faction::COMMUNIST) {

				if (InsideSquareRadius(centerPos, repairRadius, myApp->entities->launcherPool[i].centerPos)
					&& InsideRadius(centerPos, repairRadius, myApp->entities->launcherPool[i].centerPos)) {
					unitsArround++;
				}

			}
		}

		if (unitsArround > 0) {
			Repair(unitsArround);
		}
	}

	//draw center
	CurrentAnim.AdvanceAnimation(dt);

	Draw();
	myApp->render->DrawCircle(centerPos.x, centerPos.y, (int)repairRadius, 255, 0, 0, 255);

	if (myApp->map->mapDebugDraw) {
		DebugDraw();
	}

	if (health > 0 && destroyed == true)
		destroyed = false;
	else if (health <= 0 && destroyed == false)
		destroyed = true;

	if (CurrentAnim.Finished()==true && health> maxHealth/2) {

		CurrentAnim = (&myApp->entities->BuildingAnimationArray[int(buildingType)][int(Building_State::IDLE)]);
	}
	else if (health <= 0 && destroyed == false) {

		CurrentAnim = (&myApp->entities->BuildingAnimationArray[int(buildingType)][int(Building_State::DESTROYED)]);

		int Aux = myApp->audio->VarsXsound_Buildings[int(buildingType)][(int)BuildingsType_Sounds::DESTROYED];
		myApp->audio->PlayFx(myApp->audio->SoundBuilding_Array[int(buildingType)][(int)BuildingsType_Sounds::DESTROYED][rand() % Aux], 0, CHANNEL_BUILDINGS, centerPos, true);


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
	myApp->player->UpdateText();

	if (buildingType == building_type::TANK_FACTORY)
		myApp->entities->heavyUnitsUnlocked = true; //TODO: Tocar UI con esto

	if (buildingType == building_type::HTPC)
		AddUnitsBuff(); //Tocar en ActivateUnits() que si es true, le suba velocidad y vida de los buffs

	else if (buildingType == building_type::EPC) {

		myApp->entities->buildingsBuff = true;

		for (int i = 0; i < myApp->entities->buildingsArray.size(); i++) {

			if (myApp->entities->buildingsArray[i].buildingType != building_type::COMMAND_CENTER)
				myApp->entities->buildingsArray[i].maxHealth += StrategicPointsLifeBuff;
			else
				myApp->entities->buildingsArray[i].maxHealth += MainBaseLifeBuff;

			myApp->entities->buildingsArray[i].healthRecovery *= 1.5;

			if (myApp->entities->buildingsArray[i].faction == entity_faction::COMMUNIST)
				myApp->entities->buildingsArray[i].health = myApp->entities->buildingsArray[i].maxHealth;

		}
	}
}


void Building::TakeReward() {

	rewardGiven = false;
	myApp->player->playerIncome -= income;

	if (buildingType == building_type::TANK_FACTORY)
		myApp->entities->heavyUnitsUnlocked = false; //TODO: Tocar UI con esto

	if (buildingType == building_type::HTPC)
		myApp->entities->unitBuff = false;

	else if (buildingType == building_type::EPC) {

		myApp->entities->buildingsBuff = false;

		for (int i = 0; i < myApp->entities->buildingsArray.size(); i++) {

			if (myApp->entities->buildingsArray[i].buildingType != building_type::COMMAND_CENTER)
				myApp->entities->buildingsArray[i].maxHealth -= StrategicPointsLifeBuff;
			else
				myApp->entities->mainBase->health -= MainBaseLifeBuff;

			myApp->entities->buildingsArray[i].healthRecovery /= 1.5;

			if (myApp->entities->buildingsArray[i].faction == entity_faction::COMMUNIST && myApp->entities->buildingsArray[i].health > myApp->entities->buildingsArray[i].maxHealth)
				myApp->entities->buildingsArray[i].health = myApp->entities->buildingsArray[i].maxHealth;
		}
	}
}

void Building::AddUnitsBuff() {

	myApp->entities->unitBuff = true;

	//Units
	for (std::vector<Unit>::iterator item = myApp->entities->unitPool.begin(); item != myApp->entities->unitPool.end(); item = next(item)) {

		if ((*item).active && (*item).faction == entity_faction::COMMUNIST && (*item).IsDead() == false) {

			(*item).stats.linSpeed *= UnitsSpeedBuff;
			(*item).stats.health += UnitsLifeBuff;
		}
	}

	//Launchers
	for (std::vector<Launcher>::iterator item = myApp->entities->launcherPool.begin(); item != myApp->entities->launcherPool.end(); item = next(item)) {

		if ((*item).active && (*item).faction == entity_faction::COMMUNIST && (*item).IsDead() == false) {

			(*item).stats.linSpeed *= UnitsSpeedBuff;
			(*item).stats.health += UnitsLifeBuff;
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

float Building::Repair(uint aroundUnits)
{
	health += aroundUnits * healthRecovery;

	if (health >= maxHealth) {

		repairable = false;
		health = maxHealth;
	}

	return health;
}

bool Building::IsDead()
{
	bool ret = false;

	if (destroyed == true)
		ret = true;
	/*if (health <= 0.0f) {
		ret = true;
	}*/

	return ret;
}

bool Building::IsVisible()
{
	return true;	//TODO: Make function with Fog of War
}
