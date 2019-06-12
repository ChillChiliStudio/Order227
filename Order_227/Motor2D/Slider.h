#ifndef SLIDER_H
#define SLIDER_H	//@Joan

#include "UIElement.h"

class Unit_Panel;

class Slider : public Image
{
public:
	friend Unit_Panel;
	//Constructor
	Slider(ui_type type, fPoint center, float barLength, uint* value, SDL_Rect texRect, SDL_Texture* tex, bool dynamic = false, UI_Element* parent = NULL, std::list<UI_Element*>* children = NULL, float size = 1.0f);

	//Destructor
	virtual ~Slider();

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

	//Update the value to which the slider is attached to
	
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

	void UpdateValue();


public:

	//Length of the bar
	uint barLength=0;

	//Movement limits
	uint leftLimit = 0;
	uint rightLimit = 0;

	//Value to change
	uint* value = nullptr;
};

#endif //IMAGE_H