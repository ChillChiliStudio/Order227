#ifndef PROJECTILE_H_
#define PROJECTILE_H_

#include "Entity.h"

enum class projectile_directions {
	NONE = -1,

	NORTH,
	NORTH_WEST,
	WEST,
	SOUTH_WEST,
	SOUTH,
	SOUTH_EAST,
	EAST,
	NORTH_EAST,

	MAX_DIRECTIONS
};

class Projectile : public Entity {
public:
	Projectile();
	~Projectile();

public:
	
	bool Update(float dt);

	void Fly();
	void Explode();
	void CheckExplosion();

	virtual float Hurt(float damage) { return 0.0f; };
	virtual bool IsDead() { return false; };
	virtual bool IsVisible() { return true; };

	projectile_directions CheckDirection(fVec2 direction);
	fVec2 SetupVecSpeed();

	// Speed
	fVec2 ProjectileVecSpeed;	// Vectorial speed
	float ProjectileVecAngle;	// Vector angle in reference with North-directed reference vector

	projectile_directions ProjectileDirection = projectile_directions::SOUTH_EAST;

	fPoint InitialPos;
	fPoint Destination;

	float damage = 0.0f; 

};

#endif