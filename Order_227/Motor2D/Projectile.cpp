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
	CheckDirection(EntityVecSpeed);
	SetupVecSpeed();
	CheckExplosion();
}

bool Projectile::Update(float dt) {

	position.x += (EntityVecSpeed.x * dt);
	position.y += (EntityVecSpeed.y * dt);
	/*centerPos.x += (EntityVecSpeed.x * dt);
	centerPos.y += (EntityVecSpeed.y * dt);
	groundPos.x += (EntityVecSpeed.x * dt);
	groundPos.y += (EntityVecSpeed.y * dt);*/

	myApp->render->DrawQuad({ (int)this->position.x,(int)this->position.y,10,10 }, 255, 0, 255, 255, true);
	return true;
}

entity_directions Projectile::CheckDirection(fVec2 direction)
{
	EntityVecAngle = direction.GetAngle({ 0.0f, -1.0f });
	EntityVecAngle = RadsToDeg(EntityVecAngle);

	if (EntityVecAngle > 337.5f || EntityVecAngle <= 22.5f) {
		EntityDirection = entity_directions::NORTH;
	}
	else if (EntityVecAngle > 292.5f) {
		EntityDirection = entity_directions::NORTH_EAST;
	}
	else if (EntityVecAngle > 247.5f) {
		EntityDirection = entity_directions::EAST;
	}
	else if (EntityVecAngle > 202.5f) {
		EntityDirection = entity_directions::SOUTH_EAST;
	}
	else if (EntityVecAngle > 157.5f) {
		EntityDirection = entity_directions::SOUTH;
	}
	else if (EntityVecAngle > 112.5f) {
		EntityDirection = entity_directions::SOUTH_WEST;
	}
	else if (EntityVecAngle > 67.5f) {
		EntityDirection = entity_directions::WEST;
	}
	else if (EntityVecAngle > 22.5f) {
		EntityDirection = entity_directions::NORTH_WEST;
	}

	return EntityDirection;
}

fVec2 Projectile::SetupVecSpeed()
{
	fPoint nodePos = { (Destination.x), (Destination.y) };

	EntityVecSpeed = GetVector2(position, nodePos);
	EntityVecSpeed = EntityVecSpeed.GetUnitVector();
	EntityVecSpeed *= 120;
	return EntityVecSpeed;
}

void Projectile::CheckExplosion() {
	if (this->position.x - Destination.x < 10 && this->position.x - Destination.x > -10 && this->position.y - Destination.y < 10 && this->position.y - Destination.y > -10) {
		Explode();
	}
}

void Projectile::Explode() {
	LOG("EXPLOSION");
	
	this->active = false;
	
}