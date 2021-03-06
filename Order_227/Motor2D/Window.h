#ifndef WINDOW_H
#define WINDOW_H

#include "Module.h"

struct SDL_Window;
struct SDL_Surface;

class Window : public Module
{
public:

	Window();

	// Destructor
	virtual ~Window();

	// Called before render is available
	bool Awake(pugi::xml_node&) override;

	// Called before quitting
	bool CleanUp() override;

	// Changae title
	void SetTitle(const char* new_title);

	// Retrive window size
	void GetWindowSize(uint& width, uint& height) const;
	void GetWindowCenter(uint& mid_x, uint& mid_y) const;

	// Retrieve window scale
	uint GetScale() const;

public:
	//The window we'll be rendering to
	SDL_Window* window;

	//The surface contained by the window
	SDL_Surface* screen_surface;


	uint		width;
	uint		height;
	
	uint		middle_x;
	uint		middle_y;

private:
	std::string	title;
	uint		scale;
};

#endif // WINDOW_H