#include "Entity.h"


Entity::Entity(entity_type entityType, fPoint position) : type(entityType), position() {

}

bool Entity::CleanUp() {

	return true;
}
