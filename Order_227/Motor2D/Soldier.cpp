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

	*/
	return true;
}

bool Soldier::Draw() {

	SDL_Rect rect = { position.x,position.y,50,50 };


	UpdateBlitOrder(); 
    myApp->render->Push(order, texture, position.x, position.y,&TestingRect);


	return true;
}

bool Soldier::Move(float dt) {

	//position.x += (speed * dt);
	//position.y += (speed * dt);
	return true;
}

void Soldier::UpdateBlitOrder() {

	//ARRAY 
	
	for (int i = 0; i < SOLDIERS_LIST_SIZE; ++i) {
		
		if (myApp->entities->eeuuUnitsArray[i] != this) {

			if (this->position.y > myApp->entities->eeuuUnitsArray[i]->position.y)
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