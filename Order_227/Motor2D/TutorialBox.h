#ifndef TUTORIAL_BOX_H
#define TUTORIAL_BOX_H

#include <cstring>
#include "Image.h"
#include "SDL\include\SDL_pixels.h"
#include "Unit.h"

struct _TTF_Font;
class Unit;
class Text;

class TutorialBox : public Image	//TODO: Make it so text can be managed inside a block, with several lines
{
public:
	//Constructor
	TutorialBox(fPoint position, SDL_Rect rect, SDL_Texture* tex = NULL, ui_type type = ui_type::NONE);

	virtual bool Update(float dt) override;
	virtual bool Draw() override;
	virtual bool DebugDraw() const override;

public:

	bool Back;
	bool Front;
	bool movingforeward;
	bool movingBackward;
	int amount_Moved;
	int currentPage = 0;
	SDL_Rect Rect;

protected:

	int velocitySlideshare = 17;
	SDL_Texture* graphics = nullptr;

};

#endif 