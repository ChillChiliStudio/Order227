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

UnitRect = {12, 0, 55,47};

Unit::~Unit() {

}


bool Unit::Update(float dt) {

	Move(dt);

	CheckInCamera = {(int)position.x,(int)position.y,UnitRect.w,UnitRect.h };
//	Draw();

	if (life <= 0)
		myApp->entities->DestroyEntity(this);

	if (myApp->render->InsideCamera(CheckInCamera) == true) {
	   Draw();
		
	}

	if (life <= 0)
		myApp->entities->DestroyEntity(this);
	
	return true;
}


bool Unit::Draw() {

		UpdateBlitOrder();

		myApp->render->Push(order, texture, position.x, position.y, &UnitRect);

	}

	return true;
}


bool Unit::Draw() {

	myApp->render->Blit(texture, (int)position.x, (int)position.y,&UnitRect);

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