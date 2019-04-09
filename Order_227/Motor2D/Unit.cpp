#include "Unit.h"
#include "Entity.h"
#include "Render.h"
#include "Textures.h"
#include "Scene.h"

Unit::Unit(fPoint pos, entity_type type,  entity_faction faction) : Entity(pos, type, faction) {
	
	


}


Unit::~Unit() {

}


bool Unit::Update(float dt) {

	Move(dt);
	

//	Draw();

	/*if (life <= 0)
		myApp->entities->DestroyEntity(this);

	UpdateBlitOrder();*/

	//myApp->render->Push(order, texture, position.x, position.y, &UnitRect);

	return true;
}


bool Unit::Draw() {

	//myApp->render->Blit(texture, position.x, position.y,&UnitRect);
	return true;
}


bool Unit::Move(float dt) {

	//position.x += (speed * dt);
	//position.y += (speed * dt);
	return true;
}


void Unit::UpdateBlitOrder() {

	/*std::list<Entity*>::iterator item = myApp->entities->entities_list.begin();
	while (item != myApp->entities->entities_list.end()) {

		if ((*item) != this) {

			if (this->position.y > (*item)->position.y)
				order += 1;
			else
				order -= 1;


		}
		item = next(item);
	}*/

}