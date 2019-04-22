#include "Infantry.h"
#include "Entity.h"
#include "Render.h"
#include "Textures.h"
#include "Scene.h"
#include "Render.h"

Infantry::Infantry(fPoint pos, infantry_type type, entity_faction faction) : Unit(pos, entity_type::INFANTRY, faction)
{
}

bool Infantry::Draw() {

	SDL_Rect rect = { (int)position.x, (int)position.y, 50, 50 };


	UpdateBlitOrder();
	myApp->render->Push(order, texture, (int)position.x, (int)position.y,&TestingRect);


	return true;
}

//bool Infantry::Move(float dt)
//{
//
//	//position.x += (speed * dt);
//	//position.y += (speed * dt);
//	return true;
//}

void Infantry::UpdateBlitOrder() {

	//ARRAY
	for (int i = 0; i < UNITS_ARRAY_SIZE; ++i) {

		if (myApp->entities->entitiesArray[i] != this) {

			if (this->position.y > myApp->entities->entitiesArray[i]->position.y)
				order += 1;
			else
				order -= 1;

		}
	}


//LIST METHOD

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
