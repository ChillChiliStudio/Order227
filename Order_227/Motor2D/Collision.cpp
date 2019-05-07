#include "Collision.h"
#include "Input.h"
#include "Log.h"
#include "Entity.h"
#include "Render.h"
#include "App.h"

Collision::Collision() {
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
		colliders[i] = nullptr;
	matrix
}