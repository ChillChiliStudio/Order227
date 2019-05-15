#ifndef PROJECTILE_H_
#define PROJECTILE_H_


class Projectile :public Entity {
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

	entity_directions CheckDirection(fVec2 direction);
	fVec2 SetupVecSpeed();



	fPoint InitialPos;
	fPoint Destination;

};

#endif