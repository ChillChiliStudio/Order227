#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "Render.h"
#include "Textures.h"
#include "Input.h"
#include "UserInterface.h"
#include "Fonts.h"
#include "Text.h"

Text::Text(const char* content, SDL_Color color, _TTF_Font* font, fPoint center, bool dynamic, UI_Element* parent, std::list<UI_Element*>* children,float size)
	: Image(ui_type::TEXT, center, GetTexSize(content, color, font), CreateTexture(content, color, font), dynamic, parent, children,size), content(content), color(color), font(font)
{}

void Text::operator+ (const char* addition)
{
	content += addition;
}

void Text::operator+ (Text& addition)
{
	content += addition.content.c_str();
}

void Text::operator- (int timesBack)
{
	/*for (int i = timesBack; i > 0; i--) {
		content.pop_back();
	}*/
}

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

void Text::ChangeSize(float size) {
	scale = size;
}

SDL_Rect Text::GetTexSize(const char* string, SDL_Color color, _TTF_Font* font)
{
	SDL_Texture* tmpTexture = myApp->fonts->Print(string, color, font);

	/*if (tmpTexture != nullptr) {	//TODO: With this commented it works, check why
		myApp->tex->UnLoad(tmpTexture);
	}*/

	return myApp->tex->GetSize(tmpTexture);
}

SDL_Texture* Text::CreateTexture(const char* string, SDL_Color color, _TTF_Font* font)
{
	return myApp->fonts->Print(string, color, font);
}

SDL_Texture* Text::LoadTexture(const char* string, SDL_Color color, _TTF_Font* font)
{
	if (graphics != nullptr) {
		myApp->tex->UnLoad(graphics);
		graphics = nullptr;
	}

	graphics = myApp->fonts->Print(string, color, font);
	return graphics;
}

SDL_Rect Text::ChangeText(const char* string, SDL_Color color, _TTF_Font* font)
{
	content += string;
	LoadTexture(string, color, font);
	myApp->tex->GetSize(graphics, (uint&)this->sprite->w, (uint&)this->sprite->h);
	return *sprite;
}

SDL_Rect Text::ChangeText(std::string string, SDL_Color color, _TTF_Font* font)
{
	return ChangeText(string.c_str(), color, font);
}

SDL_Rect Text::ChangeString(const char* string)
{
	content += string;
	LoadTexture(string, color, font);
	myApp->tex->GetSize(graphics, (uint&)this->sprite->w, (uint&)this->sprite->h);
	return *sprite;
}

SDL_Rect Text::ChangeString(std::string string)
{
	return ChangeString(string.c_str());
}

SDL_Rect Text::ChangeColor(SDL_Color color)
{
	this->color = color;
	LoadTexture(content.c_str(), color, font);
	return *sprite;
}

SDL_Rect Text::ChangeFont(_TTF_Font* font)
{
	this->font = font;
	LoadTexture(content.c_str(), color, font);
	myApp->tex->GetSize(graphics, (uint&)this->sprite->w, (uint&)this->sprite->h);
	return *sprite;
}