#ifndef BUTTON_H
#define BUTTON_H	//@Carles

#include "Image.h"

enum class button_state
{
	DISABLED = 0,
	IDLE,
	HOVERING,
	PRESSING,
	PRESSING_RIGHT,

	MAX_TYPES
};

class Button : public Image
{
public:
	//Constructor
	Button(ui_type type, fPoint center, SDL_Rect spriteRect, SDL_Texture* tex, bool dynamic = false, UI_Element* parent = NULL, std::list<UI_Element*>* children = NULL);

	virtual ~Button();

	// Called each frame (framerate dependant)
	virtual bool Update(float dt) override;

public:
	virtual void Enable();
	virtual void Disable();
	
	button_state GetState();

protected:
	virtual button_state CheckCurrentState();
	virtual button_state ButtonStateEffects();
	
	//State Entry
	virtual void OnIdle();
	virtual void OnHover();
	virtual void OnPress();
	virtual void OnPressRight();

	//State Effects
	virtual void WhileIdle() {};
	virtual void WhileHover() {};
	virtual void WhilePress() {};

protected:
	button_state buttonStatus;
};

#endif //__BUTTON_H__