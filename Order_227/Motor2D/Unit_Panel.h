#ifndef UNIT_PANEL_H
#define UNIT_PANEL_H	//@Carles

#include <cstring>
#include "Image.h"
#include "SDL\include\SDL_pixels.h"
#include "Unit.h"

struct _TTF_Font;
class Unit;

class Unit_Panel : public UI_Element	//TODO: Make it so text can be managed inside a block, with several lines
{
public:
	//Constructor
	Unit_Panel( SDL_Rect rect, Image* button = NULL, SDL_Texture* tex = NULL ,ui_type type = ui_type::NONE);

	virtual bool Update(float dt) override;
	virtual bool Draw() override;
	virtual bool DebugDraw() const override;

	bool ActiveElement();
	bool onTop();
	
public:


protected:

	bool enableButton=false;
	Image* Currentbutton = nullptr;
	SDL_Rect Panel_Rect;
	SDL_Texture* graphics = nullptr;

	//uint lineChars;
	//uint maxChars;
};

#endif //__UNIT_PANEL_H_