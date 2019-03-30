#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "Input.h"
#include "Audio.h"
#include "Button.h"
#include "SDL/include/SDL.h"

//Constructor
Button::Button(ui_type type, fPoint center, SDL_Texture* tex, SDL_Rect spriteRect, bool dynamic, UI_Element* parent, std::list<UI_Element*>* children)
	: Image(type, center, tex, spriteRect, dynamic, parent, children), status(button_state::IDLE)
{};

Button::~Button()
{
	RELEASE(sprite);
}

bool Button::Update(float dt)
{
	bool ret = true;

	if (status != button_state::DISABLED) {
		CheckCurrentState();
		ButtonStateEffects();
	}

	return ret;
}

//Enable/Disable
void Button::Enable()
{
	status = button_state::IDLE;
}

void Button::Disable()
{
	status = button_state::DISABLED;
}

button_state Button::CheckCurrentState()
{
	switch (status) {
	case button_state::IDLE:
		if (MouseOnImage() == true) {
			OnHover();
			status = button_state::HOVERING;
		}
		break;
	case button_state::HOVERING:
		if (myApp->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
			OnPress();
			status = button_state::PRESSING;
		}
		else if (MouseOnImage() == false) {
			OnIdle();
			status = button_state::IDLE;
		}
		break;
	case button_state::PRESSING:
		if (myApp->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP || MouseOnImage() == false) {
			OnIdle();
			status = button_state::IDLE;
		}
		break;
	}

	return status;
}

button_state Button::ButtonStateEffects()
{
	switch (status) {
	case button_state::IDLE:
		WhileIdle();
		break;
	case button_state::HOVERING:
		WhileHover();
		break;
	case button_state::PRESSING:
		WhilePress();
		break;
	}

	return status;
}