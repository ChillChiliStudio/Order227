#ifndef LAUNCHER_H_
#define LAUNCHER_H_


class Projectile :public Entity {
public:
	Projectile();
	~Projectile();

public:

	void Fly(fPoint destination);
	void Explode();

};

#endif