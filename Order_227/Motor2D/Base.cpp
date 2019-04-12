#include "Base.h"

Base::Base(fPoint position, base_type baseType, entity_faction faction) : Entity(position, entity_type::BASE, faction)
{}

bool Base::Update()
{
	int i = 0;

	return true;
}

bool Base::CleanUp()
{
	int i = 0;

	return true;
}

bool Base::Draw(float dt)
{

	return true;
}