#include "App.h"
#include "Audio.h"
#include "Textures.h"
#include "Render.h"
#include "Fonts.h"
#include "Input.h"
#include "UserInterface.h"
#include "Image.h"
#include "Text.h"
#include "Window.h"

Image::Image(ui_type type, fPoint center, SDL_Rect sprite, SDL_Texture* tex, bool dynamic, UI_Element* parent, std::list<UI_Element*>* children,float size)
	: UI_Element(type, center, dynamic, parent, children), graphics(tex)
{
	this->sprite = new SDL_Rect;
	*(this->sprite) = sprite;
	RelocatePosByCenter();
	scale = size;
	if (parent != NULL && center.x == 0.0f && center.y == 0.0f) {
		ChangeCenter(parent->GetCenter());
	}
}

Image::~Image()
{
	RELEASE(sprite);
}

bool Image::Update(float dt)
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
		position.x += mouseMov.x;
		position.y += mouseMov.y;
		center.x += mouseMov.x;
		center.y += mouseMov.y;

		for (std::list<UI_Element*>::iterator iter = children.begin(); iter != children.end(); iter = next(iter)) {
			(*iter)->position.x += mouseMov.x;
			(*iter)->position.y += mouseMov.y;
			(*iter)->center.x += mouseMov.x;
			(*iter)->center.y += mouseMov.y;
		}
	}
	else {
		setMouseGrabPos = false;
	}

	return ret;
}

bool Image::Draw()
{
	bool ret = true;

		if (lookingRight) {
			ret = myApp->render->Blit(graphics, (int)position.x, (int)position.y, sprite, SDL_FLIP_NONE, FollowCam, scale);
		}
		else {
			ret = myApp->render->Blit(graphics, (int)position.x, (int)position.y, sprite, SDL_FLIP_HORIZONTAL, FollowCam, scale);
		}

		for (std::list<UI_Element*>::iterator iter = children.begin(); iter != children.end(); iter = next(iter)) {
			if((*iter)->active)
				(*iter)->Draw();
		}
	
	return ret;
}

bool Image::DebugDraw() const
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

iPoint Image::GetSize() const
{
	iPoint size = { sprite->w, sprite->h };
	return size;
}

SDL_Rect* Image::GetSprite() const
{
	return sprite;
}

//int Image::SetSlider(int left, int right, int* value)	//TODO: Reallocate slider functionalities where necessary (this was so wacky lol)
//{
//	leftLimit = left;
//	rightLimit = right;
//	sliderValue = value;
//	center.x = (float)(leftLimit + *sliderValue);
//	RelocatePosByCenter();
//	return center.x;
//}
//
//void Image::LimitSlide()
//{
//	if (center.x > rightLimit) {
//		center.x = rightLimit;
//	}
//
//	if (center.x < leftLimit) {
//		center.x = leftLimit;
//	}
//	RelocatePosByCenter();
//}
//
//void Image::SliderToValue()
//{
//	*sliderValue = center.x - leftLimit;
//	
//	myApp->audio->SetMusicVolume();
//	myApp->audio->SetSfxVolume();
//}

fPoint Image::RelocateCenterByPos()
{
	center = { position.x + (sprite->w / 2), position.y + (sprite->h / 2) };
	return center;
}

fPoint Image::RelocatePosByCenter()
{
	position = { center.x - (sprite->w / 2), center.y - (sprite->h / 2) };
	return position;
}

fPoint Image::ChangePosition(fPoint reference)
{
	position = reference;
	RelocateCenterByPos();
	return position;
}

fPoint Image::ChangeCenter(fPoint reference)
{
	center = reference;
	RelocatePosByCenter();
	return center;
}

bool Image::MouseOnImage() {	// Note: SDL disposes of the following function -> SDL_PointInRect(&mousePos, sprite);
	SDL_Point mousePos;
	myApp->input->GetMousePosition(mousePos.x, mousePos.y);

	return !(position.y + sprite->h*scale < mousePos.y || position.y > mousePos.y || position.x + sprite->w*scale < mousePos.x || position.x > mousePos.x);
}