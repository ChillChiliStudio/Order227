#include "Unit.h"
#include "Entity.h"
#include "Render.h"
#include "Textures.h"
#include "Scene.h"

Unit::Unit(unit_type unitType, fPoint pos, faction_enum faction) : Entity(entity_type::UNIT_ENT, pos)  {

	UnitFaction = faction;
	UnitType = unitType;

	if (faction == faction_enum::FACTION_CAPITALIST) {

		switch (unitType) {

		case (unit_type::INFANTRY_DIVISION):
			texture = myApp->scene->TestTexture;
			break;
		case(unit_type::UNKNOWN):
			break;
		default:
			break;
		}

	}
	else if (faction == faction_enum::FACTION_COMMUNIST) {

		switch (unitType) {

		case (unit_type::INFANTRY_DIVISION):
			texture = myApp->scene->TestTexture;
			break;
		case(unit_type::UNKNOWN):
			break;
		default:
			break;
		}
	}
}


Unit::~Unit() {

}


bool Unit::Update(float dt) {

	Move(dt);
	UnitRect = {12, 0, 55,47};
	Draw();

	return true;
}


bool Unit::Draw() {

	//myApp->render->DrawQuad(UnitRect, 0, 255, 0);

	myApp->render->Blit(texture, position.x, position.y,&UnitRect);
	return true;
}


bool Unit::Move(float dt) {

	position.x += (speed * dt);
	position.y += (speed * dt);
	return true;
}