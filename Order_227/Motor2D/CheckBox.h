//#ifndef CHECK_BOX_H
//#define CHECK_BOX_H
//
//#include "ActionBox.h"
//
//enum class bool_state
//{
//	DISABLED = 0,
//	OFF,
//	HOVERING,
//	ON,
//
//	MAX_TYPES
//};
//
//class Check_Box : public Action_Box<bool, bool*>
//{
//public:
//	//Constructor
//	Check_Box(bool* value, fPoint center, SDL_Texture* tex, SDL_Rect spriteList[4], UI_Element* parent = NULL, std::list<UI_Element*>* children = NULL);
//
//	virtual ~Check_Box();
//
//public:
//	//Enable/Disable
//	virtual void Enable();
//	virtual void Disable();
//
//protected:
//	virtual button_state CheckCurrentState();
//	virtual button_state ButtonStateEffects();
//
//	virtual void OnIdle();
//	virtual void OnHover();
//	virtual void OnPress();
//
//	bool SwitchValue(bool* value);
//
//private:
//	bool* value;
//};
//
//#endif // CHECK_BOX_H