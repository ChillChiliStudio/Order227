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

	SDL_Rect rect = { position.x,position.y,50,50 };

	myApp->render->DrawQuad(rect, 255, 0, 0);

	return true;
}

bool Infantry::Move(float dt) {

	//position.x += (speed * dt);
	//position.y += (speed * dt);
	return true;
}

void Infantry::UpdateBlitOrder() {

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