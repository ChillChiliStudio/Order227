#ifndef _BUTTON_ACTIONS_H_
#define _BUTTON_ACTIONS_H_//@Carles

//List of all button actions
//void ShutDown() {
//	myApp->mustShutDown = true;
//}

enum class Screen_Type {

	SCREEN_NONE = -1,
	SCREEN_MAINMENU,
	SCREEN_INGAME,
	SCREEN_WIN,
	SCREEN_LOSE,
	SCREEN_MAX
};

void CreateConscript();
void CreateFlak();
void CreateChrono();
void CreateDesolator();
void CreateSniper();
void StartGame();
void QuitGame();
void CloseGame();
Screen_Type getCurrentScreen();


#endif //_BUTTON_ACTIONS_H_