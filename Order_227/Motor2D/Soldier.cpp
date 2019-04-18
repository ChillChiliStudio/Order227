#include "Soldier.h"
#include "Entity.h"
#include "Render.h"
#include "Textures.h"
#include "Scene.h"

#include "Render.h"

Soldier::Soldier(fPoint pos, soldier_type type, entity_faction faction) :Unit(pos, entity_type::SOLDIER, faction)
{


}

bool Soldier::Update(float dt) {

	if (!active) {
		return false;
	}

	Move(dt);


	Draw();

	/*if (life <= 0)
		myApp->entities->DestroyEntity(this);

	UpdateBlitOrder();*/

	//myApp->render->Push(order, texture, position.x, position.y, &UnitRect);
	UnitRect.x = position.x;
	UnitRect.y = position.y;
	if (selected)
		myApp->render->DrawQuad(UnitRect, 0, 0, 255, 255, false);
	return true;
}

bool Soldier::Draw() {

	SDL_Rect rect = { position.x,position.y,50,50 };

	myApp->render->DrawQuad(rect, 255, 0, 0);

	return true;
}

bool Soldier::Move(float dt) {

	//position.x += (speed * dt);
	//position.y += (speed * dt);
	return true;
}

void Soldier::UpdateBlitOrder() {

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