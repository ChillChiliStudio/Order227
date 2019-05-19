#include "Horde_Manager.h"
#include "Spawning_Point.h"
#include "Log.h"
#include "App.h"
#include "Input.h"
#include "UserInterface.h"
#include "Text.h"
#include "Render.h"

Horde_Manager::Horde_Manager()
{
	name.assign("horde_manager");
}

bool Horde_Manager::Awake(pugi::xml_node&config) {

	maxHordes = config.child("max_hordes").attribute("value").as_int(0);
	initialRoundThreat = roundThreat = config.child("initial_horde_threat").attribute("value").as_int(0);
	initialThreatIncremental = threatIncremental = config.child("initial_threat_incremental").attribute("value").as_int(0);
	return true;
}

bool Horde_Manager::Start()
{

	return true;
}

bool Horde_Manager::CleanUp()
{
	for (int i = SpawningPoints_Array.size() - 1; i >= 0; i--)
		RELEASE(SpawningPoints_Array[i]);

	SpawningPoints_Array.clear();

	return true;
}

bool Horde_Manager::Update(float dt)
{

	//Spawn Point Draw
	if (myApp->map->mapDebugDraw)
		for (int i = 0; i < SpawningPoints_Array.size(); i++) 
			myApp->render->Blit(myApp->map->debug_tex, SpawningPoints_Array[i]->position.x, SpawningPoints_Array[i]->position.y);

	if (CleanHordesTimer.ReadSec() > TIME_TO_CHECK_HORDES && hordeActive == true) {

		CleanHordes();

		if (HordesDead() == true && roundTimer.Read() > TIME_BETWEEN_ROUNDS) {

			ChooseSpawningPoints();
			roundTimer.Start();
		}

		else if (HordesDead() == false)
			roundTimer.Start();
	}

	for (int i = 0; i < SpawningPoints_Array.size(); i++) 
	{
		if (SpawningPoints_Array[i]->Enemies_to_Spawn.size() > 0)
		{
			fPoint SP_Pos(SpawningPoints_Array[i]->position.x, SpawningPoints_Array[i]->position.y);

			while (SpawningPoints_Array[i]->Enemies_to_Spawn.size() > 0)
			{

				SpawningPoints_Array[i]->SpawnTime.Start();

				int infType = (SpawningPoints_Array[i]->Enemies_to_Spawn.back());
				hordes[i]->AddUnit(myApp->entities->ActivateUnit(SP_Pos, infantry_type(infType), entity_faction::CAPITALIST));
				SpawningPoints_Array[i]->Enemies_to_Spawn.pop_back();

			}

			hordes[i]->SpreadDestinations(SpawningPoints_Array[i]->position);
			hordes[i]->TransmitOrders(unit_orders::MOVE);

		}
		if (!hordes[i]->groupUnits.empty() &&
			!SpawningPoints_Array[i]->enemiesAttacking &&
			SpawningPoints_Array[i]->SpawnTime.Read() > 2000)
		{

			hordes[i]->SpreadDestinations({int (myApp->entities->mainBase->position.x + myApp->entities->mainBase->entityRect.w/2), int(myApp->entities->mainBase->position.y + myApp->entities->mainBase->entityRect.h / 2)});
			hordes[i]->TransmitOrders(unit_orders::MOVE);
			SpawningPoints_Array[i]->enemiesAttacking = true;
		}
	}

	return true;
}

void Horde_Manager::ChooseSpawningPoints()
{

		if (roundNumber == maxHordes) {

			roundNumber = 0;
			roundThreat = initialRoundThreat;
		}

		//This is VERY HARDCODED - I'm not proud of this, but will work for now (fck this shit) - Is just the manager of round threat TODO
		if (roundNumber == 0 || roundNumber == 1)
			threatIncremental = initialThreatIncremental;
		else if (roundNumber % 3 == 0)
			threatIncremental += initialThreatIncremental;
		else
			threatIncremental = 0;

		LOG("Round num: %i   Threat: %i   ThreatInc: %i", roundNumber, roundThreat, threatIncremental);

		roundThreat += threatIncremental;
		roundNumber++;

		std::string a = std::to_string(roundNumber);
		myApp->gui->hordeNumber_Label->ChangeString(a.c_str());

		//Reseting spawning points (put them at false to choose between them)
		for (int i = 0; i < SpawningPoints_Array.size(); i++)
			SpawningPoints_Array[i]->active = false;


		//If we want more spawning points just put more of these
		int r1 = rand() % SpawningPoints_Array.size(); //For ranges not starting at 0: rand()%X + Y --> Range of rands between Y and X
		int r2 = rand() % SpawningPoints_Array.size(); //Rand num between 0 and array's size (if 4 SP, then 0-4)

		while (r2 == r1)
			r2 = rand() % SpawningPoints_Array.size(); //This can be pretty unpredictable & uncontrolling shit
		
		int r3 = rand() % SpawningPoints_Array.size();

		while (r3 == r2 || r3 == r1)
			r3 = rand() % SpawningPoints_Array.size();


		SpawningPoints_Array[r1]->active = true;
		SpawningPoints_Array[r2]->active = true;
		SpawningPoints_Array[r3]->active = true;

		SpawningPoints_Array[r1]->enemiesAttacking = false;
		SpawningPoints_Array[r2]->enemiesAttacking = false;
		SpawningPoints_Array[r3]->enemiesAttacking = false;

		for (int i = 0; i < SpawningPoints_Array.size(); i++)
			if (SpawningPoints_Array[i]->active == true)
				SpawningPoints_Array[i]->FillEnemies(roundThreat);

}

void Horde_Manager::ClearEnemies()
{
	for (int i = 0; i < hordes.size(); ++i)
	{
		for (std::list<Unit*>::iterator it = hordes[i]->groupUnits.begin();!hordes[i]->groupUnits.empty() ;it++ )	//TODO: DeActivate all then clear list, not necessary to erase one by one
		{
			myApp->entities->DeActivateUnit((*it));
			hordes[i]->groupUnits.erase(it);
		}
	}
}

bool Horde_Manager::HordesDead()
{

	for (int i = 0; i < hordes.size(); ++i) 
	{
		if (hordes[i]->groupUnits.size() > 0)
			return false;
		/*std::list<Unit*>::iterator iter = hordes[i]->groupUnits.begin();
		for ( int b=0; b< hordes[i]->groupUnits.size();++b)
		{
			if ((*iter)->active)
				return false;
			++iter;
		}*/
	}
	return true;
}


void Horde_Manager::CleanHordes() {

	for (int i = 0; i < hordes.size(); i++) {

		std::list<Unit*>::iterator iter = hordes[i]->groupUnits.begin();
		for (int j = 0; j < hordes[i]->groupUnits.size(); j++) {


			if ((*iter)->faction == entity_faction::COMMUNIST || (*iter)->infantryType == infantry_type::INFANTRY_NONE)
				hordes[i]->groupUnits.erase(iter);

			++iter;
		}
	}

	CleanHordesTimer.Start();
}