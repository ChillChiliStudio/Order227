#ifndef UNIT_H
#define UNIT_H



#include "Entity.h"
#include "EntityManager.h"
#include "App.h"
//static_assert(unit_type::UNKNOWN == entity_type(2), "UPDATE ENTITY TYPES");

enum class unit_type {
	INFANTRY_DIVISION,
	UNKNOWN = 1
};
enum class unit_state {

};


class Class_Unit : public Entity{
public:
	Class_Unit(iPoint pos);
	~Class_Unit();
public:
	void Move();
	void Attack();
public:
	void Kill();
	void Hurt();
	bool IsDead();

public:
	unsigned int life;
	unsigned int maxLife;
	unit_state status;
	iPoint detectionRadius;
	iPoint attackRange;
	bool enemyDetected;
	bool enemyInRange;


};






#endif
