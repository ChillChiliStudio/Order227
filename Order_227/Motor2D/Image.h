#ifndef IMAGE_H
#define IMAGE_H	//@Carles

#include "UIElement.h"

class Unit_Panel;

class Image	: public UI_Element
{
public:
	friend Unit_Panel;
	//Constructor
	Image(ui_type type, fPoint center, SDL_Rect texRect, SDL_Texture* tex, bool dynamic = false, UI_Element* parent = NULL, std::list<UI_Element*>* children = NULL,float size=1.0f);

	//Destructor
	virtual ~Image();

	// Called before render is available
	virtual bool Awake(pugi::xml_node&) { return true; }

	// Called before the first frame
	virtual bool Start() override { return true; }

	// Called each loop iteration
	virtual bool PreUpdate() override { return true; }

	// Called each frame (framerate dependant)
	virtual bool Update(float dt) override;

	// Called each loop iteration (graphic)
	virtual bool Draw() override;
	virtual bool DebugDraw() const override;

	// Called each loop iteration
	virtual bool PostUpdate() override { return true; }

	// Save and Load
	virtual bool Load(pugi::xml_node&) override { return true; }
	virtual bool Save(pugi::xml_node&) const override { return true; };

public:
	virtual iPoint GetSize() const;
	virtual SDL_Rect* GetSprite() const;

	virtual fPoint RelocateCenterByPos();
	virtual fPoint RelocatePosByCenter();
	virtual fPoint ChangePosition(fPoint reference) override;
	virtual fPoint ChangeCenter(fPoint reference) override;

	virtual bool MouseOnImage();

	//int SetSlider(int left, int right, int* value);	//TODO: Bad slider implementation, improve
	//void LimitSlide();
	//void SliderToValue();

public:
	std::string folder;
	bool FollowCam = false;
	bool lookingRight = true;	// IMPROVE: Change to an enum "orientation" for more blitting parameters
	bool Unlocked = true;
	//int rightLimit = 0;	//TODO: Bad slider implementation, improve
	//int leftLimit = 0;s
	//int* sliderValue = nullptr;

protected:


	float scale = 1.0f;
	SDL_Rect* sprite = nullptr;
	SDL_Texture* graphics = nullptr;

	fPoint grabOffset;
	bool setMouseGrabPos = false;
};

#endif //IMAGE_H