#ifndef BUILDING_H
#define BUILDING_H

#include "Entity.h"
#include "App.h"
#include "Animation.h"

#include "SDL/include/SDL.h"


struct SDL_Rect;

enum class building_type {

	BUILDING_NONE = -1,
	COMMAND_CENTER,
	TANK_FACTORY,
	RADAR,
	GOLDYARD,
	HTPC,
	PEOPLE_HEART,
	EPC,
	BUILDING_MAX
};

enum class Building_State {

	SPAWN,
	IDLE,
	DESTROYED,
	MAX

};


class Building :public Entity
{
public:

	Building();
	Building(fPoint position, building_type baseType = building_type::BUILDING_NONE, entity_faction faction = entity_faction::NEUTRAL);
	~Building() {}

	bool Update(float dt);
	bool CleanUp();
	bool Start();

	bool Draw();
	bool DebugDraw();

	virtual float Hurt(float damage);
	virtual float Repair(uint aroundUnits);
	virtual bool IsDead();
	virtual bool IsVisible();

	void TakeReward();

private:

	void GiveReward();
	void AddUnitsBuff();

public:

	//ADD ANIMATION
	//Current_animation*
	building_type buildingType;
	Animation StartAnim;
	Animation CurrentAnim;

	//Buffs
	bool destroyed = true;

	float UnitsSpeedBuff = 1.5f;
	int UnitsLifeBuff = 2;
	int MainBaseLifeBuff = 400;
	int StrategicPointsLifeBuff = 200;

	float health = 0.0f;
	float maxHealth = 0.0f;
	float healthRecovery = 0.2f;	//TODO: Unharcode
	int income = 0;

	float repairRadius = 170.0f;
	bool repairable = false;

	bool rewardGiven = false;

};
#endif
