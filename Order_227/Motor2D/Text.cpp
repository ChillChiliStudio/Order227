#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "Render.h"
#include "Textures.h"
#include "Input.h"
#include "UserInterface.h"
#include "Fonts.h"
#include "Text.h"

Text::Text(const char* content, SDL_Color color, _TTF_Font* font, fPoint center, bool dynamic, UI_Element* parent, std::list<UI_Element*>* children)
	: Image(ui_type::TEXT, center, LoadTexture(content, color, font), myApp->tex->GetSize(graphics), dynamic, parent, children), content(content), color(color), font(font)
{}

const char* Text::GetText() const
{
	return content.c_str();
}

SDL_Color Text::GetColor() const
{
	return color;
}

_TTF_Font* Text::GetFont() const
{
	return font;
}

SDL_Texture* Text::LoadTexture(const char* string, SDL_Color color, _TTF_Font* font)
{
	SDL_Rect tmp = { 0, 0, 0, 0 };
	content.append(string);
	graphics = myApp->fonts->Print(string, color, font);
	return graphics;
}

SDL_Rect Text::ChangeText(const char* string, SDL_Color color, _TTF_Font* font)
{
	if (graphics != nullptr) {
		myApp->tex->UnLoad(graphics);
		graphics = nullptr;
	}

	content.append(string);
	graphics = myApp->fonts->Print(string, color, font);
	myApp->tex->GetSize(graphics, (uint&)this->sprite->w, (uint&)this->sprite->h);
	return *sprite;
}

SDL_Rect Text::ChangeText(std::string string, SDL_Color color, _TTF_Font* font)
{
	if (graphics != nullptr) {
		myApp->tex->UnLoad(graphics);
		graphics = nullptr;
	}

	content = string;
	graphics = myApp->fonts->Print(string.c_str(), color, font);
	myApp->tex->GetSize(graphics, (uint&)this->sprite->w, (uint&)this->sprite->h);
	return *sprite;
}

SDL_Rect Text::ChangeContent(const char* string)
{
	if (graphics != nullptr) {
		myApp->tex->UnLoad(graphics);
		graphics = nullptr;
	}

	content.append(string);
	graphics = myApp->fonts->Print(string, color, font);
	myApp->tex->GetSize(graphics, (uint&)this->sprite->w, (uint&)this->sprite->h);
	return *sprite;
}

SDL_Rect Text::ChangeColor(SDL_Color color)
{
	if (graphics != nullptr) {
		myApp->tex->UnLoad(graphics);
		graphics = nullptr;
	}

	this->color = color;
	graphics = myApp->fonts->Print(content.c_str(), color, font);
	return *sprite;
}

SDL_Rect Text::ChangeFont(_TTF_Font* font)
{
	if (graphics != nullptr) {
		myApp->tex->UnLoad(graphics);
		graphics = nullptr;
	}

	this->font = font;
	graphics = myApp->fonts->Print(content.c_str(), color, font);
	myApp->tex->GetSize(graphics, (uint&)this->sprite->w, (uint&)this->sprite->h);
	return *sprite;
}