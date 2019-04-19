#include "Building.h"

Building::Building(fPoint position, building_type baseType, entity_faction faction) : Entity(position, entity_type::BUILDING, faction)
{}


bool Building::Update()
{

	return true;
}


bool Building::CleanUp()
{

	return true;
}


bool Building::Draw(float dt)
{

	return true;
}