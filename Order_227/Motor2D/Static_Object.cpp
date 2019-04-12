#include "Static_Object.h"

Static_Object::Static_Object(fPoint position, object_type objectType, entity_faction faction): Entity(position,entity_type::OBJECT,faction)
{}

bool Static_Object::Update()
{
	int i = 0;

	return true;
}

bool Static_Object::CleanUp()
{
	int i = 0;

	return true;
}

bool Static_Object::Draw(float dt)
{

	return true;
}