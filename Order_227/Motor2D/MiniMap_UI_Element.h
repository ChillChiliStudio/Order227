#ifndef MINI_MAP_H
#define MINI_MAP_H	//@Carles

#include <cstring>
#include "Image.h"
#include "SDL\include\SDL_pixels.h"
#include "Unit.h"


class MiniMap_UI : public UI_Element	//TODO: Make it so text can be managed inside a block, with several lines
{
public:

	//Constructor
	MiniMap_UI(ui_type type=ui_type::NONE);

	//bool Start();
	virtual bool Update(float dt) override;
	virtual bool Draw() override;
	virtual bool DebugDraw() const override;
	virtual bool PostUpdate() override;




public:


protected:

	//uint lineChars;
	//uint maxChars;
};

#endif //__UNIT_PANEL_H_