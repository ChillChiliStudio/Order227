#include "Brofiler/Brofiler.h"
#include "App.h"
#include "Textures.h"
#include "Render.h"
#include "Fonts.h"
#include "UserInterface.h"
#include "UIElement.h"

UI_Element::UI_Element(ui_type type, fPoint position, bool dynamic, UI_Element* parent, std::list<UI_Element*>* children) : type(type), position(position), center(position), dynamic(dynamic), parent(parent)
{
	if (children != NULL) {
		this->children = *children;
	}

	if (parent != NULL) {
		parent->AttachChild(this);
	}

	Init();
}

UI_Element::~UI_Element()
{}

bool UI_Element::CleanUp()
{
	bool ret = true;

	for (std::list<UI_Element*>::iterator iter = children.begin(); iter != children.end(); ++iter) {
		(*iter)->mustDestroy = true;
	}
	children.clear();

	return ret;
}

ui_type UI_Element::GetType() const
{
	return type;
}

UI_Element* UI_Element::GetParent() const
{
	return parent;
}

fPoint UI_Element::GetPosition() const
{
	return position;
}

fPoint UI_Element::GetCenter() const
{
	return center;
}

fPoint UI_Element::MatchPosition(fPoint reference)
{
	position = reference;
	return position;
}

fPoint UI_Element::MatchCenter(fPoint reference)
{
	center = reference;
	return center;
}

void UI_Element::Activate()
{
	active = true;

	for (std::list<UI_Element*>::iterator iter = children.begin(); iter != children.end(); ++iter) {
		(*iter)->active = true;
	}
}

void UI_Element::Deactivate()
{
	active = false;

	for (std::list<UI_Element*>::iterator iter = children.begin(); iter != children.end(); ++iter) {
		(*iter)->active = false;
	}
}

void UI_Element::AttachParent(UI_Element* parent)	//WARNING: Revise if parent-child relations get fucky, std implemented
{
	this->parent = parent;
	parent->children.push_back(this);
}

void UI_Element::DetachParent()
{
	parent->children.remove(this);
	parent = NULL;
}

void UI_Element::AttachChild(UI_Element* child)
{
	child->parent = this;
	children.push_back(child);
}

void UI_Element::DetachChild(UI_Element* child)
{
	child->parent = NULL;
	children.remove(child);
}