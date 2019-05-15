#ifndef LAUNCHER_H_
#define LAUNCHER_H_
#include "Projectile.h"



class Launcher :public Unit {
public:
	Launcher();
	~Launcher();

	//bool Start() ;
	bool Update(float dt);
	

public:

	void AttackCurrTarget(float dt);
	void LaunchProjectile(fPoint destination);
	
	//std::vector<Projectile>			ProjectilePool;
	
};





#endif 
