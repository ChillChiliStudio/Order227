#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "Input.h"
#include "Audio.h"
#include "Button.h"
#include "SDL/include/SDL.h"

//Constructor
Button::Button(ui_type type, fPoint center, SDL_Rect spriteRect, SDL_Texture* tex, bool dynamic, UI_Element* parent, std::list<UI_Element*>* children)
	: Image(type, center, spriteRect, tex, dynamic, parent, children), buttonStatus(button_state::IDLE)
{};

Button::~Button()
{
	RELEASE(sprite);
}

bool Button::Update(float dt)
{
	bool ret = true;

	if (buttonStatus != button_state::DISABLED) {
		CheckCurrentState();
		ButtonStateEffects();
	}

	return ret;
}

//Enable/Disable
void Button::Enable()
{
	buttonStatus = button_state::IDLE;
}

void Button::Disable()
{
	buttonStatus = button_state::DISABLED;
}

button_state Button::GetState()
{
	return buttonStatus;
}

button_state Button::CheckCurrentState()
{
	switch (buttonStatus) {
	case button_state::IDLE:
		if (MouseOnImage() == true) {
			OnHover();
		}
		break;
	case button_state::HOVERING:
		if (myApp->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
			OnPress();
		}
		else if (myApp->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN) {
			OnPressRight();
		}
		else if (MouseOnImage() == false) {
			OnIdle();
		}
		break;
	case button_state::PRESSING:
		if (myApp->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP || MouseOnImage() == false) {
			OnIdle();
		}
		break;
	case button_state::PRESSING_RIGHT:
		if (myApp->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_UP || MouseOnImage() == false) {
			OnIdle();
		}
		break;
	}

	return buttonStatus;
}

button_state Button::ButtonStateEffects()
{
	switch (buttonStatus) {
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

	return buttonStatus;
}

void Button::OnIdle()
{
	buttonStatus = button_state::IDLE;
}

void Button::OnHover()
{
	buttonStatus = button_state::HOVERING;
}

void Button::OnPress()
{
	buttonStatus = button_state::PRESSING;
}
void Button::OnPressRight()
{
	buttonStatus = button_state::PRESSING_RIGHT;
}