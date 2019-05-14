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

Launcher::Launcher(){}

Launcher::~Launcher(){}

void Launcher::AttackCurrTarget(float dt) {
	fVec2 targetDirection = GetVector2(centerPos, currTarget->centerPos);
	unit_directions lastDirection = unitDirection;
	CheckDirection(targetDirection);

	if (lastDirection != unitDirection) {
		UpdateAnimation();
	}

	currTarget->Hurt((float)stats.damage * dt); //CHANGE FOR LAUNCH PROJECTILE
	currTarget.

	if (unitState != unit_state::ATTACKING) {
		attackTimer.Start();
		unitState = unit_state::ATTACKING;
	}
	else if (attackTimer.Read() > stats.cadency) {
		myApp->audio->PlayFx(stats.attackSfxId);
		attackTimer.Start();
	}
}