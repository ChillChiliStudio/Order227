#include "Horde_Manager.h"
#include "Spawning_Point.h"
#include "Log.h"
#include "App.h"
#include "Input.h"
#include "Render.h"

Horde_Manager::Horde_Manager()
{}

bool Horde_Manager::Start()
{
	roundTimer.Start();

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
	if (myApp->debugMode)
	{
		if (myApp->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
			ChooseSpawningPoints();

		if (myApp->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
			ClearEnemies();
	}

	//Spawn Point Draw
	if (myApp->map->mapDebugDraw)
		for (int i = 0; i < SpawningPoints_Array.size(); i++) 
			myApp->render->Blit(myApp->map->debug_tex, SpawningPoints_Array[i]->position.x, SpawningPoints_Array[i]->position.y);
		
	if (HordesDead() && roundTimer.Read() > TIME_BETWEEN_ROUNDS){
		ChooseSpawningPoints();
		roundTimer.Start();
		}

	else if (!HordesDead())
		roundTimer.Start();


	for (int i = 0; i < SpawningPoints_Array.size(); i++) 
	{
		if (SpawningPoints_Array[i]->Enemies_to_Spawn.size() > 0)
		{
			fPoint SP_Pos(SpawningPoints_Array[i]->position.x, SpawningPoints_Array[i]->position.y);

			while (SpawningPoints_Array[i]->Enemies_to_Spawn.size() > 0)
			{
				SpawningPoints_Array[i]->SpawnTime.Start();

				hordes[i]->AddUnit(myApp->entities->ActivateInfantry(SP_Pos, infantry_type::BASIC, entity_faction::CAPITALIST));

				SpawningPoints_Array[i]->Enemies_to_Spawn.pop_back();

				LOG("%d", SpawningPoints_Array[i]->Enemies_to_Spawn.size());
			}

			hordes[i]->SpreadDestinations(SpawningPoints_Array[i]->position);
			hordes[i]->TransmitOrders(unit_orders::MOVE_AND_ATTACK);

		}
		if (!hordes[i]->groupUnits.empty() &&
			!SpawningPoints_Array[i]->enemiesAttacking && 
			SpawningPoints_Array[i]->SpawnTime.Read() > 2000)
		{
			hordes[i]->SpreadDestinations({0, 1400 });
			hordes[i]->TransmitOrders(unit_orders::MOVE_AND_ATTACK);
			SpawningPoints_Array[i]->enemiesAttacking = true;
		}
	}

	return true;
}

void Horde_Manager::ChooseSpawningPoints()
{

		//Restarting round if reached 20 - for MVP this should be 5
		if (roundNumber == 20) {

			roundNumber = 0;
			roundThreat = 0;
		}

		//This is VERY HARDCODED - I'm not proud of this, but will work for now (fck this shit) - Is just the manager of round threat TODO
		if (roundNumber == 0 || roundNumber == 1)
			threatIncremental = 10;
		else if (roundNumber % 3 == 0)
			threatIncremental += 10;
		else
			threatIncremental = 0;

		LOG("Round num: %i   Threat: %i   ThreatInc: %i", roundNumber, roundThreat, threatIncremental);

		roundThreat += threatIncremental;
		roundNumber++;

		//Reseting spawning points (put them at false to choose between them)
		for (int i = 0; i < SpawningPoints_Array.size(); i++)
			SpawningPoints_Array[i]->active = false;


		//If we want more spawning points just put more of these
		int r1 = rand() % SpawningPoints_Array.size(); //For ranges not starting at 0: rand()%X + Y --> Range of rands between Y and X
		int r2 = rand() % SpawningPoints_Array.size(); //Rand num between 0 and array's size (if 4 SP, then 0-4)

		if (SpawningPoints_Array.size() > 1) {

			while (r2 == r1)
				r2 = rand() % SpawningPoints_Array.size(); //This can be pretty unpredictable & uncontrolling shit
		}

		SpawningPoints_Array[r1]->active = true;
		SpawningPoints_Array[r2]->active = true;

		SpawningPoints_Array[r1]->enemiesAttacking = false;
		SpawningPoints_Array[r2]->enemiesAttacking = false;

		for (int i = 0; i < SpawningPoints_Array.size(); i++)
			if (SpawningPoints_Array[i]->active == true)
				SpawningPoints_Array[i]->FillEnemies(roundThreat);

}

void Horde_Manager::ClearEnemies()
{
	for (int i = 0; i < hordes.size(); ++i)
	{
		for (std::list<Unit*>::iterator it = hordes[i]->groupUnits.begin();!hordes[i]->groupUnits.empty() ;it++ )
		{
			myApp->entities->DeActivateInfantry((*it));
			hordes[i]->groupUnits.erase(it);
		}
	}
}

bool Horde_Manager::HordesDead()
{
	for (int i = 0; i < hordes.size(); ++i) {
		if (!hordes[i]->groupUnits.empty()) {
			return false;
		}
	}

	return true;
}