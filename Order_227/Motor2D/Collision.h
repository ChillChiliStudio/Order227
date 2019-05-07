#ifndef COLLISION_H_
#define COLLISION_H_

#define MAX_COLLIDERS = 500;

#include "Module.h"
#include "SDL/include/SDL.h"
#include "Defs.h"
#include "Log.h"
#include "App.h"



class Entity;

enum class COLLIDER_TYPE {
	COLLIDER_NONE = -1,
	COLLIDER_CAPITALIST_TROOP,
	COLLIDER_MAX
};
struct Collider {
	SDL_Rect rect;
	bool to_delete = false;
	COLLIDER_TYPE type;
	Module* callback = nullptr;
	Entity* entity_callback = nullptr;

	Collider(SDL_Rect rect, COLLIDER_TYPE type, Module *callback = nullptr) :
		rect(rect), type(type), callback(callback) {}


	Collider(SDL_Rect rect, COLLIDER_TYPE type, Entity *entcallback = nullptr) :
		rect(rect), type(type), entity_callback(entcallback) {}


	void SetPos(int x, int y)
	{
		rect.x = x;
		rect.y = y;
	}

	bool CheckCollision(const SDL_Rect& r) const;
};

class Collision : public Module {
public:
	Collision();
	~Collision();

	bool PreUpdate() override;
	bool Update(float dt) override;
	bool CleanUp() override;

	Collider* AddCollider(SDL_Rect rect, COLLIDER_TYPE type, Module* callback = nullptr);
	Collider* AddEntCollider(SDL_Rect rect, COLLIDER_TYPE type, Entity* entity_callback = nullptr);
	void DebugDraw();

private:

	Collider * colliders[MAX_COLLIDERS];
	bool matrix[COLLIDER_MAX][COLLIDER_MAX];
	bool debug = false;
};

#endif // !COLLISION_H_
