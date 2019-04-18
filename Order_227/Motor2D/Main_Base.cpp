#include "Main_Base.h"
#include "Entity.h"
#include "Render.h"
#include "Textures.h"
#include "Scene.h"

Main_Base::Main_Base(fPoint pos) : Entity(entity_type::OBJECT, pos)
{
	position = pos;

	texture = myApp->tex->Load("textures/buildings/mainbase.png");
	life = 100;
	MainBaseRect = { 313, 2072, 225, 225 };
	
	CheckInCamera = {(int)pos.x,(int)pos.y,MainBaseRect.w,MainBaseRect.h};

}



Main_Base::~Main_Base()
{
}


bool Main_Base::Update(float dt) {

	if (life <= 0)
		myApp->entities->DestroyEntity(this);

	if (myApp->render->InsideCamera(CheckInCamera) == true) {

		UpdateBlitOrder();

		myApp->render->Push(order, texture, position.x, position.y, &MainBaseRect);
	}
	return true;
}


bool Main_Base::Draw() {
	
	myApp->render->Blit(texture, 10, 10, &MainBaseRect);
	return true;
}


void Main_Base::UpdateBlitOrder() {

	std::list<Entity*>::iterator item = myApp->entities->entities_list.begin();
	while (item != myApp->entities->entities_list.end()) {

		if ((*item) != this) {

			if (this->position.y > (*item)->position.y)
				order -= 1;
			else
				order += 1;


		}
		item = next(item);
	}

}