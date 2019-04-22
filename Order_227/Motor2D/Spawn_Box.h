#ifndef SPAWN_BOX_H
#define SPAWN_BOX_H	//@Carles

#include "VoidBox.h"
#include "CheckBox.h"

class Spawn_Box : public Void_Box
{
public:
	//Constructor
	Spawn_Box(bool value, event_function action, fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex, UI_Element* parent = NULL);

public:
	//Enable/Disable
	void Enable() override;
	void Disable() override;

	bool GetValue();
	value_state GetValueState();
	bool SwitchValue();

protected:
	//State Entry
	void OnIdle() override;
	void OnHover() override;
	void OnPress() override;

private:
	value_state valueStatus;
	bool value;
};

#endif //CHECK_BOX_H