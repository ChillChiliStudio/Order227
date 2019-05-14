#ifndef LAUNCHER_H_
#define LAUNCHER_H_
#include "Projectile.h"



class Launcher :public Unit {
public:
	Launcher();
	~Launcher();

public:

	void AttackCurrTarget(float dt);
	void LaunchProjectile(fPoint destination);
	
	Projectile

};





#endif 
