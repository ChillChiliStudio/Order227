//#include "Defs.h"
//#include "Log.h"
//#include "App.h"
//#include "Input.h"
//#include "Audio.h"
//#include "ActionBox.h"
//#include "CheckBox.h"
//#include "SDL/include/SDL.h"
//
////Constructor
//Check_Box::Check_Box(bool* value, fPoint center, SDL_Texture* tex, SDL_Rect spriteList[4], UI_Element* parent = NULL, std::list<UI_Element*>* children = NULL)
//	: Action_Box<bool, bool*>(action, center, tex, spriteList, parent, children)
//{
//	action = SwitchValue;
//}
//
//Check_Box::~Check_Box()
//{}
//
////Enable/Disable
//void Check_Box::Enable()
//{
//	status = button_state::IDLE;
//	*sprite = stateSprites[(int)bool_state::OFF];
//}
//
//void Check_Box::Disable()
//{
//	status = button_state::DISABLED;
//	*sprite = stateSprites[(int)button_state::DISABLED];
//}
//
//void Check_Box::OnIdle()
//{}
//
//void Check_Box::OnHover()
//{}
//
//void Check_Box::OnPress()
//{}
//
//bool Check_Box::SwitchValue(bool* value)
//{
//	*value = !*value;
//	return *value;
//}