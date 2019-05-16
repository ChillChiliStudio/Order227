#ifndef MOUSE_H
#define MOUSE_H	//@Carles

#include <cstring>
#include "Image.h"
#include "SDL\include\SDL_pixels.h"
#include "Unit.h"

struct _TTF_Font;
class Unit;

enum class Mouse_States {

	NONE = -1,
	IDLE,
	PRESSING,
	ONTOPENEMY,
	SELECTUNITS,
	PRESSONENEMY,

	MAX_STATE
};

class Mouse : public UI_Element	//TODO: Make it so text can be managed inside a block, with several lines
{
public:
	//Constructor
	Mouse( SDL_Texture* tex = NULL, ui_type type = ui_type::NONE );

	virtual bool Update(float dt) override;
	virtual bool Draw() override;
	virtual bool DebugDraw() const override;
	Mouse_States CheckCurrentState();
	void loadMouseAnims(Animation& Anim,int initial,int end);
	bool onTopEnemy();
public:



protected:

	fPoint offset;
	Animation Animation_Arr[5];
	Animation*CurrentAnimation;
	Mouse_States currentState;
	SDL_Rect* sprite = nullptr;
	SDL_Texture* graphics = nullptr;

	//uint lineChars;
	//uint maxChars;
};

#endif //__MOUSE_H_