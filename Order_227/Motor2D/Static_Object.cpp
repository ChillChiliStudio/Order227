#include "Static_Object.h"
#include "Render.h"
#include "Textures.h"
#include "Scene.h"
#include "Render.h"

Static_Object::Static_Object()
{}

Static_Object::Static_Object(fPoint pos, object_type objectType, entity_faction faction) : Entity(pos, entity_type::OBJECT, faction)
{
	this->objectType = objectType;
}

bool Static_Object::Update(float dt)
{
	Draw();

	if (myApp->map->mapDebugDraw) {
		DebugDraw();
	}

	return true;
}

bool Static_Object::CleanUp()
{
	int i = 0;



	return true;
}

bool Static_Object::Draw()
{
	myApp->render->Push(order, texture, position.x, position.y, &spriteRect);

	return true;
}

bool Static_Object::DebugDraw()
{
	return myApp->render->DrawQuad(entityRect, 0, 255, 0, 255, false);
}
