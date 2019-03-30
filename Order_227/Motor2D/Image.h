#ifndef IMAGE_H_
#define IMAGE_H_	// @Carles

#include "UIElement.h"

class Image	: public UI_Element
{
public:
	//Constructor
	Image(ui_type type, fPoint center, SDL_Rect texRect, SDL_Texture* tex, bool dynamic = false, UI_Element* parent = NULL, std::list<UI_Element*>* children = NULL);

	//Destructor
	virtual ~Image();

	// Called before render is available
	virtual bool Awake(pugi::xml_node&) { return true; }

	// Called before the first frame
	virtual bool Start() { return true; }

	// Called each loop iteration
	virtual bool PreUpdate() { return true; }

	// Called each frame (framerate dependant)
	virtual bool Update(float dt);

	// Called each loop iteration (graphic)
	virtual bool Draw() override;
	virtual bool DebugDraw() const override;

	// Called each loop iteration
	virtual bool PostUpdate() { return true; }

	// Save and Load
	virtual bool Load(pugi::xml_node&) { return true; }
	virtual bool Save(pugi::xml_node&) const { return true; };

public:
	virtual iPoint GetSize() const;
	virtual SDL_Rect* GetSprite() const;

	virtual fPoint RelocateCenterByPos();
	virtual fPoint RelocatePosByCenter();
	virtual fPoint MatchPosition(fPoint reference);
	virtual fPoint MatchCenter(fPoint reference);

	virtual bool MouseOnImage();

	//int SetSlider(int left, int right, int* value);	//TODO: Bad slider implementation, improve
	//void LimitSlide();
	//void SliderToValue();

public:
	std::string folder;

	bool lookingRight = true;	// IMPROVE: Change to an enum "orientation" for more blitting parameters
	
	//int rightLimit = 0;	//TODO: Bad slider implementation, improve
	//int leftLimit = 0;
	//int* sliderValue = nullptr;

protected:
	SDL_Rect* sprite = nullptr;
	SDL_Texture* graphics = nullptr;

	fPoint grabOffset;
	bool setMouseGrabPos = false;
};

#endif //__IMAGE_H__