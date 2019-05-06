#ifndef TEXT_H
#define TEXT_H	//@Carles

#include "Image.h"
#include "SDL\include\SDL_pixels.h"

struct _TTF_Font;

class Text : public Image	//TODO: Make it so text can be managed inside a block, with several lines
{
public:
	//Constructor
	Text(const char* content, SDL_Color color = { 255, 255, 255, 255 }, _TTF_Font* font = NULL, fPoint center = { 0, 0 }, bool dynamic = false, UI_Element* parent = NULL, std::list<UI_Element*>* children = NULL,float size=1.0f);

public:
	void operator+ (const char* addition);
	void operator+ (Text& addition);
	void operator- (int timesBack);

	const char* GetText() const;
	SDL_Color GetColor() const;
	_TTF_Font* GetFont() const;
	SDL_Rect GetTexSize(const char* string, SDL_Color color, _TTF_Font* font);

	SDL_Texture* CreateTexture(const char* string, SDL_Color color, _TTF_Font* font);
	SDL_Texture* LoadTexture(const char* string, SDL_Color color, _TTF_Font* font);

	SDL_Rect ChangeText(const char* string, SDL_Color color, _TTF_Font* font);
	SDL_Rect ChangeText(std::string string, SDL_Color color, _TTF_Font* font);
	SDL_Rect ChangeString(const char* string);
	SDL_Rect ChangeString(std::string string);
	SDL_Rect ChangeColor(SDL_Color color);
	SDL_Rect ChangeFont(_TTF_Font* font);
	void ChangeSize(float size);


protected:
	std::string content;	//TODO: Temporal fix, std::string didn't work no matter how used, we should instead use owr own string class and remake Text when using it
	SDL_Color color;
	_TTF_Font* font = nullptr;
	
	//uint lineChars;
	//uint maxChars;
};

#endif //__TEXT_H_