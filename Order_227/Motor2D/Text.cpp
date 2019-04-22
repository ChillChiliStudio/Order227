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
	: Image(ui_type::TEXT, center, myApp->tex->GetSize(graphics), LoadTexture(content, color, font), dynamic, parent, children), /*content(content), */color(color), font(font)
{
	strcpy_s(this->content, sizeof(this->content), content);
}

void Text::operator+ (const char* addition)
{
	strcat_s(content, sizeof(this->content), addition);
}

void Text::operator- (int timesBack)
{
	/*for (int i = timesBack; i > 0; i--) {
		content.pop_back();
	}*/
}

const char* Text::GetText() const
{
	return content;
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
	if (graphics != nullptr) {
		myApp->tex->UnLoad(graphics);
		graphics = nullptr;
	}

	graphics = myApp->fonts->Print(string, color, font);
	return graphics;
}

SDL_Rect Text::ChangeText(const char* string, SDL_Color color, _TTF_Font* font)
{
	strcpy_s(this->content, sizeof(this->content), content);
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
	strcpy_s(this->content, sizeof(this->content), content);
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
	LoadTexture(content, color, font);
	return *sprite;
}

SDL_Rect Text::ChangeFont(_TTF_Font* font)
{
	this->font = font;
	LoadTexture(content, color, font);
	myApp->tex->GetSize(graphics, (uint&)this->sprite->w, (uint&)this->sprite->h);
	return *sprite;
}