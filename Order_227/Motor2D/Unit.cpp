#include "Unit.h"
#include "Entity.h"


Unit::Unit(unit_type unitType, iPoint pos, faction_enum faction) : Entity(entity_type::UNIT_ENT, pos)  {

	UnitFaction = faction;
	UnitType = unitType;
}

Unit::~Unit() {

}