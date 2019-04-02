#ifndef PARAM_BOX_H
#define PARAM_BOX_H	//@Carles

#include "ActionBox.h"
#include "SDL/include/SDL_rect.h"

template <class T_param>
UI_Element* GenerateParamBox(void(*action)(T_param), T_param parameter, fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex = NULL, UI_Element* parent = NULL)
{
	UI_Element* ret = nullptr;

	ret = new Param_Box<T_param>(action, parameter, center, spriteList, tex, parent);

	return ret;
}

template <class T_param>
class Param_Box : public Action_Box
{
public:
	typedef void(*event_function)(T_param);

	//Constructor
	Param_Box(event_function action, T_param parameter, fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex, UI_Element* parent = NULL)
		: Action_Box(ui_type::ACTION_BOX, center, spriteList, tex, parent), action(action), parameter(parameter)
	{};

protected:
	//State Entry
	virtual void OnPress()
	{
		//myApp->audio->PlayFx(myApp->audio->buttonSfx.id, 0);
		buttonStatus = button_state::PRESSING;
		*sprite = stateSprites[(int)buttonStatus];
		action(parameter);
	}

protected:
	event_function action;
	T_param parameter;
};

#endif //PARAM_BOX_H