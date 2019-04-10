#include "Unit.h"
#include "Entity.h"
#include "Render.h"
#include "Textures.h"
#include "Scene.h"

Unit::Unit(unit_type unitType, fPoint pos, faction_enum faction) : Entity(entity_type::UNIT_ENT, pos)  {

	
	UnitFaction = faction;
	UnitType = unitType;
	
	life = 1;

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

	if (life <= 0)
		myApp->entities->DestroyEntity(this);
	
	Draw();
	return true;
}


bool Unit::Draw() {

	UpdateBlitOrder();

	myApp->render->Push(order, texture, position.x, position.y, &UnitBlitRect);

	if (selected) {
		myApp->render->DrawQuad(UnitRect, 255, 0, 0, 255, false);
	}
	
	return true;
}


bool Unit::Move(float dt) {

	position.x += (speed * dt);
	position.y += (speed * dt);
	return true;
}


void Unit::UpdateBlitOrder() {

	std::list<Entity*>::iterator item = myApp->entities->entities_list.begin();
	while (item != myApp->entities->entities_list.end()) {

		if ((*item) != this) {

			if (this->position.y > (*item)->position.y)
				order += 1;
			else
				order -= 1;


		}
		item = next(item);
	}

}