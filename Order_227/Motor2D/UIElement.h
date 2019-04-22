#ifndef UI_ELEMENT_H
#define UI_ELEMENT_H	//@Carles

#include <list>
#include "Point.h"
#include "PugiXml/src/pugixml.hpp"

struct SDL_Rect;
struct SDL_Texture;

enum class ui_type
{
	NONE = -1,

	IMAGE,
	TEXT,

	BUTTON,
	ACTION_BOX,
	CHECK_BOX,
	SLIDER,

	MAX_TYPES
};

class UI_Element
{
public:
	//Constructor
	UI_Element(ui_type type, fPoint position, bool dynamic = false, UI_Element* parent = NULL, std::list<UI_Element*>* children = NULL);

	//Destructor
	virtual ~UI_Element();

	// Called on entity creation
	virtual void Init() {
		active = true;
	}

	// Called before the first frame
	virtual bool Start() { return true; }

	// Called each loop iteration
	virtual bool PreUpdate() { return true; }

	// Called each frame (framerate dependant)
	virtual bool Update(float dt) { return true; };

	// Called each loop iteration
	virtual bool PostUpdate() { return true; }

	// Called before quitting
	virtual bool CleanUp();

	// Save and Load
	virtual bool Load(pugi::xml_node&) { return true; }
	virtual bool Save(pugi::xml_node&) const { return true; };

public:
	virtual ui_type GetType() const;
	virtual UI_Element* GetParent() const;
	virtual fPoint GetPosition() const;
	virtual fPoint GetCenter() const;

	virtual fPoint ChangePosition(fPoint reference);
	virtual fPoint ChangeCenter(fPoint reference);

	virtual void Activate();
	virtual void Deactivate();

	virtual void AttachParent(UI_Element* parent);
	virtual void DetachParent();

	virtual void AttachChild(UI_Element* child);
	virtual void DetachChild(UI_Element* child);

	virtual bool Draw() = 0;
	virtual bool DebugDraw() const = 0;

public:
	std::string name;

	bool active;
	bool mustDestroy = false;

	fPoint position;
	fPoint center;
	bool dynamic = false;

protected:
	UI_Element* parent = nullptr;
	std::list<UI_Element*> children;

private:
	ui_type type;
};

#endif //__UI_ELEMENT_H__