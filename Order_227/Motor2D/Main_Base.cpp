#include "Main_Base.h"
#include "Entity.h"
#include "Render.h"
#include "Textures.h"
#include "Scene.h"

Main_Base::Main_Base(fPoint pos) : Entity(entity_type::OBJECT_ENT, pos)
{
	position = pos;

	texture = myApp->tex->Load("textures/buildings/mainbase.png");
	life = 100;
	MainBaseRect = { 313, 2072, 225, 225 };
}



Main_Base::~Main_Base()
{
}


bool Main_Base::Update(float dt) {

	Draw();

	if (life <= 0)
		myApp->entities->DestroyEntity(this);

	return true;
}


bool Main_Base::Draw() {
	
	myApp->render->Blit(texture, 10, 10, &MainBaseRect);
	return true;
}