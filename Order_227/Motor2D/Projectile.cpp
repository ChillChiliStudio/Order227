#include "Geometry.h"
#include "Textures.h"
#include "Entity_Manager.h"
#include "Unit.h"
#include "Projectile.h"
#include "Log.h"
#include "Render.h"
Projectile::Projectile() {

}

Projectile::~Projectile() {

}

void Projectile::Fly() {
	LOG("I BELIEVE I CAN FLAY");
	/*vector direction --- move*/
}

bool Projectile::Update(float dt) {

	LOG("working xd");

	myApp->render->DrawQuad(this->entityRect,0,0,255,255,true);
	return true;
}