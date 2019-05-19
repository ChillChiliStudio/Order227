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
	virtual float Repair();
	virtual bool IsDead();
	virtual bool IsVisible();

private:

	void GiveReward();
	void TakeReward();

public:

	//ADD ANIMATION
	//Current_animation*
	building_type buildingType;
	Animation StartAnim;
	Animation CurrentAnim;

	//Buffs
	bool unitBuff = false;
	bool destroyed = false;

	int UnitsSpeedBuff = 0;
	int UnitsLifeBuff = 0;
	int MainBaseLifeBuff = 0;
	int StrategicPointsLifeBuff = 0;

	float health = 0.0f;
	float maxHealth = 0.0f;
	float healthRecovery = 0.1f;
	int income = 0;

	bool repairable = false;

private:

	bool rewardGiven = false;

};
#endif 