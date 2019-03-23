#ifndef ACTION_BOX_H
#define ACTION_BOX_H

#include "Button.h"

template <class Ret, class... Args>
class ActionBox : public Button
{
public:
	//Constructor
	template<class Ret, class... Args> ActionBox(Ret(*action)(Args...), fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex, bool dynamic = false, UI_Element* parent = NULL, std::list<UI_Element*>* children = NULL)
		: Button(ui_type::BUTTON_ACTION, center, spriteList[(int)button_state::IDLE], tex, dynamic, parent, children), action(action)
	{
		stateSprites = new SDL_Rect[(int)button_state::MAX_TYPES];

		for (int i = 0; i < (int)button_state::MAX_TYPES; i++) {
			stateSprites[i] = spriteList[i];
		}
	};

	virtual ~ActionBox()
	{
		RELEASE(sprite);
		RELEASE_ARRAY(stateSprites);
	}

	//Button action calling
	virtual Ret operator() (Args&... args) const
	{
		return (action)(args...);
	}

	virtual Ret DoAction(Args&... args) const {
		return (action)(args...);
	}

public:
	//Enable/Disable
	virtual void Enable()
	{
		status = button_state::IDLE;
		*sprite = stateSprites[(int)button_state::IDLE];
	}

	virtual void Disable()
	{
		status = button_state::DISABLED;
		*sprite = stateSprites[(int)button_state::DISABLED];
	}

protected:
	virtual void OnIdle()
	{
		*sprite = stateSprites[(int)button_state::IDLE];
	}

	virtual void OnHover()
	{
		*sprite = stateSprites[(int)button_state::HOVERING];
	}

	virtual void OnPress()
	{
		//myApp->audio->PlayFx(myApp->audio->buttonSfx.id, 0);
		*sprite = stateSprites[(int)button_state::PRESSING];
		DoAction(Args...);
	}

	virtual void WhileIdle() {}

	virtual void WhileHover() {}

	virtual void WhilePress() {}

private:
	Ret(*action)(Args...);
	SDL_Rect* stateSprites = nullptr;	//Disabled, Idle, Hover, Pressed
};

#endif //ACTION_BOX_H