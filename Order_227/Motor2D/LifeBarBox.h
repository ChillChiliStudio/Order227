#ifndef LIFEBAR_H
#define LIFEBAR_H	//@Carles

#include <cstring>
#include "Image.h"
#include "SDL\include\SDL_pixels.h"
#include "Unit.h"

struct _TTF_Font;
class Unit;

class LifeBar : public UI_Element	//TODO: Make it so text can be managed inside a block, with several lines
{
public:
	//Constructor
	LifeBar(fPoint center, Unit* parent=NULL,SDL_Texture* tex=NULL,ui_type type=ui_type::NONE,float* auxHealth=NULL);

	virtual bool Update(float dt) override;
	virtual bool Draw() override;
	virtual bool DebugDraw() const override;

public:

	float totalLife;
	float* life=nullptr;
	unit_stats state;

protected:

	Unit* Currentparent = nullptr;
	SDL_Rect LowHealth;
	SDL_Rect HighHealth;
	SDL_Texture* graphics = nullptr;

	//uint lineChars;
	//uint maxChars;
};

#endif //__LIFEBAR_H_