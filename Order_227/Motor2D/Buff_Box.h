#ifndef BUFF_BOX_H
#define BUFF_BOX_H	//@Carles

#include <cstring>
#include "Image.h"
#include "SDL\include\SDL_pixels.h"
#include "Unit.h"

struct _TTF_Font;
class Unit;
class Text;

class Buff_Box : public Image	//TODO: Make it so text can be managed inside a block, with several lines
{
public:
	//Constructor
	Buff_Box(fPoint position,SDL_Rect rect, SDL_Texture* tex = NULL, bool* able=nullptr, ui_type type = ui_type::NONE);

	virtual bool Update(float dt) override;
	virtual bool Draw() override;
	virtual bool DebugDraw() const override;

public:


protected:

	
	bool* enableButton = false;
	SDL_Rect Rect;
	SDL_Texture* graphics = nullptr;

	//uint lineChars;
	//uint maxChars;
};

#endif //__UNIT_PANEL_H_