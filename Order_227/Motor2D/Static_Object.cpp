#include "Static_Object.h"
#include "Render.h"
#include "Textures.h"
#include "Scene.h"
#include "Render.h"

Static_Object::Static_Object(fPoint pos, object_type objectType, entity_faction faction) : Entity(pos, entity_type::OBJECT, faction)
{

}

bool Static_Object::Update(float dt)
{

	Draw();

	return true;
}

bool Static_Object::CleanUp()
{
	int i = 0;



	return true;
}

bool Static_Object::Draw()
{

	UpdateBlitOrder();
	myApp->render->Push(order, texture, position.x, position.y, &UnitRect);

	return true;
}

void Static_Object::UpdateBlitOrder() {

	//ARRAY
	for (int i = 0; i < UNITS_ARRAY_SIZE; ++i) {

		if (myApp->entities->entitiesArray[i] != this) {

			if (this->position.y > myApp->entities->entitiesArray[i]->position.y)
				order += 1;
			else
				order -= 1;

		}
	}
}
