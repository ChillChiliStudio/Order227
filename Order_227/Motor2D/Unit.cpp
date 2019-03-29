#include "Unit.h"
#include "Entity.h"
#include "Render.h"
#include "Textures.h"

Unit::Unit(unit_type unitType, fPoint pos, faction_enum faction) : Entity(entity_type::UNIT_ENT, pos)  {

	UnitFaction = faction;
	UnitType = unitType;

	//if (faction == faction_enum::FACTION_CAPITALIST) {

	//	switch (unitType) {

	//	case (unit_type::INFANTRY_DIVISION):
	//		texture = myApp->tex->Load("textures\troops\allied\gi.png");
	//		break;
	//	case(unit_type::UNKNOWN):
	//		break;
	//	default:
	//		break;
	//	}

	//}
	//else if (faction == faction_enum::FACTION_COMMUNIST) {

	//	switch (unitType) {

	//	case (unit_type::INFANTRY_DIVISION):
	//		texture = myApp->tex->Load("textures\troops\soviet\InfanteriaSov.png");
	//		break;
	//	case(unit_type::UNKNOWN):
	//		break;
	//	default:
	//		break;
	//	}
	//}
}

Unit::~Unit() {

}


bool Unit::Update(float dt) {

	Move(dt);
	UnitRect = { (int)position.x, (int)position.y, 20, 20 };
	Draw();

	return true;
}

bool Unit::Draw() {

	myApp->render->DrawQuad(UnitRect, 0, 255, 0);
//	myApp->render->Blit(texture, position.x, position.y);
	return true;
}

bool Unit::Move(float dt) {

	position.x += (speed * dt);
	position.y += (speed * dt);
	return true;
}