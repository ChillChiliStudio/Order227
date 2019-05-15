#include "Geometry.h"
#include "Render.h"
#include "Textures.h"
#include "Scene.h"
#include "Map.h"
#include "Pathfinding.h"
#include "Entity_Manager.h"
#include "Entity.h"
#include "Unit.h"
#include "Audio.h"
#include "UserInterface.h"
#include "Image.h"
#include "Brofiler/Brofiler.h"
#include "Launcher.h"
#include "Projectile.h"
#include "Log.h"
Launcher::Launcher(){
	//ProjectilePool.resize(10);
}

Launcher::~Launcher(){}



bool Launcher::Update(float dt) {

	if (ProjectilePool.size() < 10) {
		ProjectilePool.resize(10);
	}

	for (int i = 0; i < ProjectilePool.size(); i++) {
		if (ProjectilePool[i].active == true) {
			ProjectilePool[i].Fly();
			ProjectilePool[i].Update(dt);
		}
	}

	LOG("UPDATE WORKING XD");
	onCamera = InsideCamera();

	entityRect.x = position.x;
	entityRect.y = position.y;

	UnitWorkflow(dt);

	if (mustDespawn) {
		mustDespawn = false;
		myApp->entities->DeActivateUnit(this);
	}
	else {
		if (myApp->entities->entitiesDebugDraw && currNode != unitPath.end()) {
			DrawPath();
		}

		currentAnimation.AdvanceAnimation(dt);	// Animation must continue even if outside camera

		if (onCamera) {	// If inside camera

			if (unitState == unit_state::MOVING) {	// State changes are always updated on animation,
				unit_directions lastDirection = unitDirection;

				if (lastDirection != CheckDirection(vecSpeed)) {
					UpdateAnimation();
				}
			}

			Draw();

			if (selected) {	//TODO: This should be as a debug functionality, but for now it'll do
				myApp->render->DrawQuad(entityRect, 0, 255, 0, 255, false);
			}

			if (myApp->entities->entitiesDebugDraw) {
				DebugDraw();
			}
		}
	}

	return true;
}

void Launcher::AttackCurrTarget(float dt) {

	fVec2 targetDirection = GetVector2(centerPos, currTarget->centerPos);
	unit_directions lastDirection = unitDirection;
	CheckDirection(targetDirection);

	if (lastDirection != unitDirection) {
		UpdateAnimation();
	}

	//currTarget->Hurt((float)stats.damage * dt); //CHANGE FOR LAUNCH PROJECTILE
	
	if (attackTimer.Read() > stats.cadency) {
		myApp->audio->PlayFx(stats.attackSfxId);  //SOUND
		LaunchProjectile(currTarget->centerPos);  //ATTACK

		if (unitState != unit_state::ATTACKING) { //ANIM
			attackTimer.Start();                  //TIMER RE-START

			unitState = unit_state::ATTACKING;
		}
	}
	else if (attackTimer.Read() < stats.cadency && this->currentAnimation.Finished()) {
		if (unitState == unit_state::ATTACKING) {
			unitState = unit_state::IDLE;
		}
	}

}

void Launcher::LaunchProjectile(fPoint destination) {
	
	for (int i = 0; i < ProjectilePool.size(); i++) {
		if (ProjectilePool[i].active == false) {
			ProjectilePool[i].active = true;
			ProjectilePool[i].position = this->position;
			ProjectilePool[i].Destination = destination;
			break;
		}
	}
}