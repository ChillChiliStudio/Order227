#include "App.h"
#include "Audio.h"
#include "Textures.h"
#include "Render.h"
#include "Fonts.h"
#include "Input.h"
#include "UserInterface.h"
#include "Text.h"
#include "Window.h"
#include "Slider.h"
#include "Log.h"

#include "App.h"
#include "Audio.h"


Slider::Slider(ui_type type, fPoint center, float barLength, uint* value, SDL_Rect sprite, SDL_Texture* tex, bool dynamic, UI_Element* parent, std::list<UI_Element*>* children, float size)
	: Image(type, center, sprite, tex, dynamic, parent, children, size), barLength(barLength), value(value)
{
	leftLimit = center.x - barLength / 2;
	rightLimit = center.x + barLength / 2;
}

Slider::~Slider()
{
	RELEASE(sprite);
}

bool Slider::Update(float dt)
{
	bool ret = true;

	if (dynamic && MouseOnImage() && myApp->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
		iPoint mousePos;
		myApp->input->GetMousePosition(mousePos.x, mousePos.y);

		if (setMouseGrabPos == false) {
			grabOffset = { (float)mousePos.x - position.x, (float)mousePos.y - position.y };
			setMouseGrabPos = true;
		}
	}
	else if (setMouseGrabPos == true && myApp->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
		iPoint mousePos;
		myApp->input->GetMousePosition(mousePos.x, mousePos.y);

		fPoint newGrabOffset;
		newGrabOffset = { (float)mousePos.x - position.x, (float)mousePos.y - position.y };

		fPoint mouseMov = { newGrabOffset.x - grabOffset.x, newGrabOffset.y - grabOffset.y };
		if (mouseMov.x > 0 && center.x < rightLimit) {
			position.x += mouseMov.x;
			center.x += mouseMov.x;
			UpdateValue();
		}
		else if (mouseMov.x < 0 && center.x > leftLimit) {
			position.x += mouseMov.x;
			center.x += mouseMov.x;
			UpdateValue();
		}
	}
	else if (setMouseGrabPos == true && myApp->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP && value == &myApp->audio->sfxVolume) {
		myApp->audio->SetSfxChunkVolume(myApp->audio->sfxVolume);
	}
	else {
		setMouseGrabPos = false;
	}
	UpdateValue();
	return ret;
}

bool Slider::Draw()
{
	bool ret = true;

	if (lookingRight) {
		ret = myApp->render->Blit(graphics, (int)position.x, (int)position.y, sprite, SDL_FLIP_NONE, FollowCam, scale);
	}
	else {
		ret = myApp->render->Blit(graphics, (int)position.x, (int)position.y, sprite, SDL_FLIP_HORIZONTAL, FollowCam, scale);
	}

	for (std::list<UI_Element*>::iterator iter = children.begin(); iter != children.end(); iter = next(iter)) {
		if ((*iter)->active)
			(*iter)->Draw();
	}

	return ret;
}

bool Slider::DebugDraw() const
{
	bool ret = true;

	/* COLORS
		Red: 255, 0, 0
		Green: 0, 255, 0
		Blue: 0, 0, 255
		Yellow: 255, 255, 0
		Magenta: 255, 0, 255
		Cyan: 0, 255, 255
		White: 255, 255, 255
		Black: 0, 0, 0
		Brown: 160, 128, 96
	*/

	// Top-Left Corner
	myApp->render->DrawLine((int)position.x, (int)position.y, (int)(position.x + sprite->w), (int)position.y, 255, 0, 0, 255, false);	// Right
	myApp->render->DrawLine((int)position.x, (int)position.y, (int)position.x, (int)position.y + sprite->h, 255, 0, 0, 255, false);	// Down
	// Down-Right Corner
	myApp->render->DrawLine((int)(position.x + sprite->w), (int)(position.y + sprite->h), (int)position.x, (int)(position.y + sprite->h), 255, 0, 0, 255, false);	// Up
	myApp->render->DrawLine((int)(position.x + sprite->w), (int)(position.y + sprite->h), (int)(position.x + sprite->w), (int)position.y, 255, 0, 0, 255, false);	// Left

	return ret;
}

iPoint Slider::GetSize() const
{
	iPoint size = { sprite->w, sprite->h };
	return size;
}

SDL_Rect* Slider::GetSprite() const
{
	return sprite;
}

fPoint Slider::RelocateCenterByPos()
{
	center = { position.x + (sprite->w / 2), position.y + (sprite->h / 2) };
	return center;
}

fPoint Slider::RelocatePosByCenter()
{
	position = { center.x - (sprite->w / 2), center.y - (sprite->h / 2) };
	return position;
}

fPoint Slider::ChangePosition(fPoint reference)
{
	position = reference;
	RelocateCenterByPos();
	return position;
}

fPoint Slider::ChangeCenter(fPoint reference)
{
	center = reference;
	RelocatePosByCenter();
	return center;
}

bool Slider::MouseOnImage() {	// Note: SDL disposes of the following function -> SDL_PointInRect(&mousePos, sprite);
	SDL_Point mousePos;
	myApp->input->GetMousePosition(mousePos.x, mousePos.y);

	return !(position.y + sprite->h*scale < mousePos.y || position.y > mousePos.y || position.x + sprite->w*scale < mousePos.x || position.x > mousePos.x);
}

void Slider::UpdateValue() {
	if (value != nullptr)
	{
		int newValue = ((center.x - leftLimit) / float(barLength)) * 100;

		if (newValue < 0)
			newValue = 0;
		else if (newValue > 100)
			newValue = 100;

		*value = newValue;
		LOG(" %u ", myApp->audio->musicVolume);

		//Harcode
		if (value == &myApp->audio->musicVolume)
			myApp->audio->SetMusicVolume();
	}
}