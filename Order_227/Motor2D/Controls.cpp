#include "Controls.h"
#include "App.h"
#include "Input.h"

#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "Input.h"
#include "Window.h"
#include "Audio.h"
#include "UserInterface.h"
#include "KeyConfig.h"
#include "SDL/include/SDL.h"

#define stringify( name ) # name

Controls::Controls()
{
	name.assign("controls");
}

Controls::~Controls()
{}

bool Controls::Awake(pugi::xml_node& config)
{
	if (myApp->saveFileExists) {	//If save file exists, load control configuration from that file, otherwise use default config
		pugi::xml_document	save_file;
		Load(myApp->LoadSaveFile(save_file).child(name.c_str()));
	}
	else {
		Load(config);
	}

	return true;
}

bool Controls::Start()
{
	for (int i = 0; i < 6; i++)
		spawnHotKeys[i] = SDL_SCANCODE_1 + i;

	AllocateLists();

	CreateKeyboardStrings();

	return true;
}

bool Controls::PreUpdate()
{
	return true;
}

bool Controls::Update(float dt)
{
	//if (myApp->input->GetKey(SDL_SCANCODE_O) == KEY_DOWN) {	//Debug Testing
	//	PrepareInputChange(orders.patrol);
	//}

	if (awaitingInput/* && myApp->input->GetKey(SDL_SCANCODE_O) != KEY_DOWN && myApp->input->GetKey(SDL_SCANCODE_O) != KEY_REPEAT*/) {

		const Uint8* keys = SDL_GetKeyboardState(NULL);

		for (int i = 0; i < MAX_KEYS; ++i) {

			if (keys[i] == 1) {
				if (KeyInUse(i)) {
					myApp->audio->PlayFx(myApp->audio->SoundUI_Player[(int)UI_playerSound::UI_ERROR], 0, CHANNEL_UI);
				}
				else {
					UpdateKey(i);
				}
				
				awaitingInput = false;

				buttonUsed->OnIdle();
				buttonUsed->UpdateText();
				buttonUsed = nullptr;

				break;
			}
		}

		//IMPROVEMENT: Allow mouse to be updated
		//if (awaitingInput) {
		//	for (int i = 0; i < NUM_MOUSE_BUTTONS; ++i) {
		//		if (myApp->input->GetMouseButtonDown(i) == KEY_DOWN) {
		//			UpdateMouseButton(i);
		//			awaitingInput = false;
		//		}
		//	}
		//}
	}

	return true;
}

bool Controls::PostUpdate()
{
	return true;
}

bool Controls::CleanUp()
{
	mouseButtonsInUse.clear();
	keysInUse.clear();

	return true;
}

// Save and Load
bool Controls::Load(pugi::xml_node& node)
{
	//Debug
	debug.debugMap = node.child("debug").attribute("map").as_int(SDL_SCANCODE_F11);
	debug.debugUI = node.child("debug").attribute("gui").as_int(SDL_SCANCODE_F10);
	debug.debugEntities = node.child("debug").attribute("entities").as_int(SDL_SCANCODE_F9);
	debug.nextRound = node.child("debug").attribute("next_round").as_int(SDL_SCANCODE_F8);
	//debug.clearEnemies = node.child("debug").attribute("???").as_int(???);

	debug.spawnBlueDog = node.child("debug").attribute("spawn_blue_dog").as_int(SDL_SCANCODE_F7);
	debug.spawnBlueChrono = node.child("debug").attribute("spawn_blue_chrono").as_int(SDL_SCANCODE_F6);
	debug.spawnBlueSniper = node.child("debug").attribute("spawn_blue_sniper").as_int(SDL_SCANCODE_F5);

	debug.spawnRedChrono = node.child("debug").attribute("spawn_red_chrono").as_int(SDL_SCANCODE_F4);
	debug.spawnRedDesolator = node.child("debug").attribute("spawn_red_desolator").as_int(SDL_SCANCODE_F3);
	debug.spawnRedBazooka = node.child("debug").attribute("spawn_red_bazooka").as_int(SDL_SCANCODE_F2);
	debug.spawnRedConscript = node.child("debug").attribute("spawn_red_conscript").as_int(SDL_SCANCODE_F1);

	//Mouse
	mouse.select = node.child("mouse").attribute("select").as_int(SDL_BUTTON_LEFT);
	mouse.issueOrders = node.child("mouse").attribute("issue_orders").as_int(SDL_BUTTON_RIGHT);

	//Camera
	camera.up = node.child("camera").attribute("up").as_int(SDL_SCANCODE_W);
	camera.down = node.child("camera").attribute("down").as_int(SDL_SCANCODE_S);
	camera.left = node.child("camera").attribute("left").as_int(SDL_SCANCODE_A);
	camera.right = node.child("camera").attribute("right").as_int(SDL_SCANCODE_D);

	//Orders
	orders.hold = node.child("orders").attribute("hold").as_int(SDL_SCANCODE_SPACE);
	orders.hunt = node.child("orders").attribute("hunt").as_int(SDL_SCANCODE_Q);
	orders.patrol = node.child("orders").attribute("patrol").as_int(SDL_SCANCODE_E);

	orders.defensive = node.child("orders").attribute("defensive").as_int(SDL_SCANCODE_LSHIFT);
	orders.aggressive = node.child("orders").attribute("aggressive").as_int(SDL_SCANCODE_LCTRL);

	return true;
}

bool Controls::Save(pugi::xml_node& node)
{
	pugi::xml_node tmpNode;

	//Debug
	tmpNode = node.append_child("debug");
	tmpNode.append_attribute("map") = debug.debugMap;
	tmpNode.append_attribute("gui") = debug.debugUI;
	tmpNode.append_attribute("entities") = debug.debugEntities;
	tmpNode.append_attribute("next_round") = debug.nextRound;
	//tmpNode.append_attribute("???") = debug.clearEnemies;

	tmpNode.append_attribute("spawn_blue_dog") = debug.spawnBlueDog;
	tmpNode.append_attribute("spawn_blue_chrono") = debug.spawnBlueChrono;
	tmpNode.append_attribute("spawn_blue_sniper") = debug.spawnBlueSniper;

	tmpNode.append_attribute("spawn_red_chrono") = debug.spawnRedChrono;
	tmpNode.append_attribute("spawn_red_desolator") = debug.spawnRedDesolator;
	tmpNode.append_attribute("spawn_red_bazooka") = debug.spawnRedBazooka;
	tmpNode.append_attribute("spawn_red_conscript") = debug.spawnRedConscript;

	//Mouse
	tmpNode = node.append_child("mouse");
	tmpNode.append_attribute("select") = mouse.select;
	tmpNode.append_attribute("issue_orders") = mouse.issueOrders;

	//Camera
	tmpNode = node.append_child("camera");
	tmpNode.append_attribute("up") = camera.up;
	tmpNode.append_attribute("down") = camera.down;
	tmpNode.append_attribute("left") = camera.left;
	tmpNode.append_attribute("right") = camera.right;

	//Orders
	tmpNode = node.append_child("orders");
	tmpNode.append_attribute("hold") = orders.hold;
	tmpNode.append_attribute("hunt") = orders.hunt;
	tmpNode.append_attribute("patrol") = orders.patrol;

	tmpNode.append_attribute("defensive") = orders.defensive;
	tmpNode.append_attribute("aggressive") = orders.aggressive;

	return true;
}

void Controls::AllocateLists()
{
	//Mouse
	mouseButtonsInUse.push_back(mouse.select);
	mouseButtonsInUse.push_back(mouse.issueOrders);

	//Keys - Debug (if debugMode activated, mark F keys as used)
	//if (myApp->debugMode) {
		for (int i = (int)SDL_SCANCODE_F1; i < (int)SDL_SCANCODE_F11; i++)
			keysInUse.push_back(i);
	//}

	//Keys - Camera
	keysInUse.push_back(camera.up);
	keysInUse.push_back(camera.down);
	keysInUse.push_back(camera.left);
	keysInUse.push_back(camera.right);

	//Keys - Orders
	keysInUse.push_back(orders.hold);
	keysInUse.push_back(orders.hunt);
	keysInUse.push_back(orders.patrol);

	keysInUse.push_back(orders.defensive);
	keysInUse.push_back(orders.aggressive);

	//Keys - Spawn HotKeys
	for (int i = 0; i < 6; i++)
		keysInUse.push_back(spawnHotKeys[i]);

	//Keys - Others
	keysInUse.push_back(pause);
}

void Controls::PrepareInputChange(int* selectedInput)
{
	inputToChange = selectedInput;
	awaitingInput = true;
}

bool Controls::MouseButtonInUse(int button)
{
	bool ret = false;

	for (int i = 0; i < mouseButtonsInUse.size(); i++) {
		if (button == mouseButtonsInUse[i]) {
			ret = true;
			break;
		}
	}

	return ret;
}

bool Controls::KeyInUse(int key)
{
	bool ret = false;

	for (int i = 0; i < keysInUse.size(); i++) {
		if (key == keysInUse[i]) {
			ret = true;
			break;
		}
	}

	return ret;
}

void Controls::UpdateMouseButton(int newVal)
{
	for (int i = 0; i < mouseButtonsInUse.size(); i++) {
		if (mouseButtonsInUse[i] == *inputToChange) {
			mouseButtonsInUse[i] = newVal;
			break;
		}
	}

	*inputToChange = newVal;
	inputToChange = nullptr;
}

void Controls::UpdateKey(int newVal)
{
	for (int i = 0; i < keysInUse.size(); i++) {
		if (keysInUse[i] == *inputToChange) {
			keysInUse[i] = newVal;
			break;
		}
	}
	
	*inputToChange = newVal;
	inputToChange = nullptr;
}

std::string Controls::TranslateKeycode(int i)
{
	return keyNames[i];
}

void Controls::CreateKeyboardStrings()
{
	keyNames[SDL_SCANCODE_UNKNOWN].assign(stringify(NONE));
	keyNames[SDL_SCANCODE_A].assign(stringify(A));
	keyNames[SDL_SCANCODE_B].assign(stringify(B));
	keyNames[SDL_SCANCODE_C].assign(stringify(C));
	keyNames[SDL_SCANCODE_D].assign(stringify(D));
	keyNames[SDL_SCANCODE_E].assign(stringify(E));
	keyNames[SDL_SCANCODE_F].assign(stringify(F));
	keyNames[SDL_SCANCODE_G].assign(stringify(G));
	keyNames[SDL_SCANCODE_H].assign(stringify(H));
	keyNames[SDL_SCANCODE_I].assign(stringify(I));
	keyNames[SDL_SCANCODE_J].assign(stringify(J));
	keyNames[SDL_SCANCODE_K].assign(stringify(K));
	keyNames[SDL_SCANCODE_L].assign(stringify(L));
	keyNames[SDL_SCANCODE_M].assign(stringify(M));
	keyNames[SDL_SCANCODE_N].assign(stringify(N));
	keyNames[SDL_SCANCODE_O].assign(stringify(O));
	keyNames[SDL_SCANCODE_P].assign(stringify(P));
	keyNames[SDL_SCANCODE_Q].assign(stringify(Q));
	keyNames[SDL_SCANCODE_R].assign(stringify(R));
	keyNames[SDL_SCANCODE_S].assign(stringify(S));
	keyNames[SDL_SCANCODE_T].assign(stringify(T));
	keyNames[SDL_SCANCODE_U].assign(stringify(U));
	keyNames[SDL_SCANCODE_V].assign(stringify(V));
	keyNames[SDL_SCANCODE_W].assign(stringify(W));
	keyNames[SDL_SCANCODE_X].assign(stringify(X));
	keyNames[SDL_SCANCODE_Y].assign(stringify(Y));
	keyNames[SDL_SCANCODE_Z].assign(stringify(Z));

	keyNames[SDL_SCANCODE_1].assign(stringify(1));
	keyNames[SDL_SCANCODE_2].assign(stringify(2));
	keyNames[SDL_SCANCODE_3].assign(stringify(3));
	keyNames[SDL_SCANCODE_4].assign(stringify(4));
	keyNames[SDL_SCANCODE_5].assign(stringify(5));
	keyNames[SDL_SCANCODE_6].assign(stringify(6));
	keyNames[SDL_SCANCODE_7].assign(stringify(7));
	keyNames[SDL_SCANCODE_8].assign(stringify(8));
	keyNames[SDL_SCANCODE_9].assign(stringify(9));
	keyNames[SDL_SCANCODE_0].assign(stringify(0));

	keyNames[SDL_SCANCODE_RETURN].assign(stringify(RET));
	keyNames[SDL_SCANCODE_ESCAPE].assign(stringify(ESC));
	keyNames[SDL_SCANCODE_BACKSPACE].assign(stringify(BACKSPACE));
	keyNames[SDL_SCANCODE_TAB].assign(stringify(TAB));
	keyNames[SDL_SCANCODE_SPACE].assign(stringify(SPACE));

	keyNames[SDL_SCANCODE_MINUS].assign(stringify(MINUS));
	keyNames[SDL_SCANCODE_EQUALS].assign(stringify(EQUALS));
	keyNames[SDL_SCANCODE_LEFTBRACKET].assign(stringify(LEFTBRACKET));
	keyNames[SDL_SCANCODE_RIGHTBRACKET].assign(stringify(RIGHTBRACKET));
	keyNames[SDL_SCANCODE_BACKSLASH].assign(stringify(BACKSLASH)); /**< Located at the lower left of the return
								  *   key on ISO keyboards and at the right end
								  *   of the QWERTY row on ANSI keyboards.
								  *   Produces REVERSE SOLIDUS (backslash) and
								  *   VERTICAL LINE in a US layout, REVERSE
								  *   SOLIDUS and VERTICAL LINE in a UK Mac
								  *   layout, NUMBER SIGN and TILDE in a UK
								  *   Windows layout, DOLLAR SIGN and POUND SIGN
								  *   in a Swiss German layout, NUMBER SIGN and
								  *   APOSTROPHE in a German layout, GRAVE
								  *   ACCENT and POUND SIGN in a French Mac
								  *   layout, and ASTERISK and MICRO SIGN in a
								  *   French Windows layout.
								  */
	keyNames[SDL_SCANCODE_NONUSHASH].assign(stringify(NONUSHASH)); /**< ISO USB keyboards actually use this code
								  *   instead of 49 for the same key, but all
								  *   OSes I've seen treat the two codes
								  *   identically. So, as an implementor, unless
								  *   your keyboard generates both of those
								  *   codes and your OS treats them differently,
								  *   you should generate keyNames[SDL_SCANCODE_BACKSLASH
								  *   instead of this code. As a user, you
								  *   should not rely on this code because SDL
								  *   will never generate it with most (all?)
								  *   keyboards.
								  */
	keyNames[SDL_SCANCODE_SEMICOLON].assign(stringify(SEMICOLON));
	keyNames[SDL_SCANCODE_APOSTROPHE].assign(stringify(APOSTROPHE));
	keyNames[SDL_SCANCODE_GRAVE].assign(stringify(GRAVE)); /**< Located in the top left corner (on both ANSI
							  *   and ISO keyboards). Produces GRAVE ACCENT and
							  *   TILDE in a US Windows layout and in US and UK
							  *   Mac layouts on ANSI keyboards, GRAVE ACCENT
							  *   and NOT SIGN in a UK Windows layout, SECTION
							  *   SIGN and PLUS-MINUS SIGN in US and UK Mac
							  *   layouts on ISO keyboards, SECTION SIGN and
							  *   DEGREE SIGN in a Swiss German layout (Mac:
							  *   only on ISO keyboards), CIRCUMFLEX ACCENT and
							  *   DEGREE SIGN in a German layout (Mac: only on
							  *   ISO keyboards), SUPERSCRIPT TWO and TILDE in a
							  *   French Windows layout, COMMERCIAL AT and
							  *   NUMBER SIGN in a French Mac layout on ISO
							  *   keyboards, and LESS-THAN SIGN and GREATER-THAN
							  *   SIGN in a Swiss German, German, or French Mac
							  *   layout on ANSI keyboards.
							  */
	keyNames[SDL_SCANCODE_COMMA].assign(stringify(COMMA));
	keyNames[SDL_SCANCODE_PERIOD].assign(stringify(PERIOD));
	keyNames[SDL_SCANCODE_SLASH].assign(stringify(SLASH));

	keyNames[SDL_SCANCODE_CAPSLOCK].assign(stringify(CAPSLOCK));

	keyNames[SDL_SCANCODE_F1].assign(stringify(F1));
	keyNames[SDL_SCANCODE_F2].assign(stringify(F2));
	keyNames[SDL_SCANCODE_F3].assign(stringify(F3));
	keyNames[SDL_SCANCODE_F4].assign(stringify(F4));
	keyNames[SDL_SCANCODE_F5].assign(stringify(F5));
	keyNames[SDL_SCANCODE_F6].assign(stringify(F6));
	keyNames[SDL_SCANCODE_F7].assign(stringify(F7));
	keyNames[SDL_SCANCODE_F8].assign(stringify(F8));
	keyNames[SDL_SCANCODE_F9].assign(stringify(F9));
	keyNames[SDL_SCANCODE_F10].assign(stringify(F10));
	keyNames[SDL_SCANCODE_F11].assign(stringify(F11));
	keyNames[SDL_SCANCODE_F12].assign(stringify(F12));

	keyNames[SDL_SCANCODE_PRINTSCREEN].assign(stringify(PRINTSCREEN));
	keyNames[SDL_SCANCODE_SCROLLLOCK].assign(stringify(SCROLLLOCK));
	keyNames[SDL_SCANCODE_PAUSE].assign(stringify(PAUSE));
	keyNames[SDL_SCANCODE_INSERT].assign(stringify(INSERT)); /**< insert on PC, help on some Mac keyboards (but
								   does send code 73, not 117) */
	keyNames[SDL_SCANCODE_HOME].assign(stringify(HOME));
	keyNames[SDL_SCANCODE_PAGEUP].assign(stringify(PAGEUP));
	keyNames[SDL_SCANCODE_DELETE].assign(stringify(DELETE));
	keyNames[SDL_SCANCODE_END].assign(stringify(END));
	keyNames[SDL_SCANCODE_PAGEDOWN].assign(stringify(PAGEDOWN));
	keyNames[SDL_SCANCODE_RIGHT].assign(stringify(RIGHT));
	keyNames[SDL_SCANCODE_LEFT].assign(stringify(LEFT));
	keyNames[SDL_SCANCODE_DOWN].assign(stringify(DOWN));
	keyNames[SDL_SCANCODE_UP].assign(stringify(UP));

	keyNames[SDL_SCANCODE_NUMLOCKCLEAR].assign(stringify(NUMLOCKCLEAR)); /**< num lock on PC, clear on Mac keyboards
									 */
	keyNames[SDL_SCANCODE_KP_DIVIDE].assign(stringify(SDL_SCANCODE_KP_DIVIDE));
	keyNames[SDL_SCANCODE_KP_MULTIPLY].assign(stringify(SDL_SCANCODE_KP_MULTIPLY));
	keyNames[SDL_SCANCODE_KP_MINUS].assign(stringify(SDL_SCANCODE_KP_MINUS));
	keyNames[SDL_SCANCODE_KP_PLUS].assign(stringify(SDL_SCANCODE_KP_PLUS));
	keyNames[SDL_SCANCODE_KP_ENTER].assign(stringify(SDL_SCANCODE_KP_ENTER));
	keyNames[SDL_SCANCODE_KP_1].assign(stringify(SDL_SCANCODE_KP_1));
	keyNames[SDL_SCANCODE_KP_2].assign(stringify(SDL_SCANCODE_KP_2));
	keyNames[SDL_SCANCODE_KP_3].assign(stringify(SDL_SCANCODE_KP_3));
	keyNames[SDL_SCANCODE_KP_4].assign(stringify(SDL_SCANCODE_KP_4));
	keyNames[SDL_SCANCODE_KP_5].assign(stringify(SDL_SCANCODE_KP_5));
	keyNames[SDL_SCANCODE_KP_6].assign(stringify(SDL_SCANCODE_KP_6));
	keyNames[SDL_SCANCODE_KP_7].assign(stringify(SDL_SCANCODE_KP_7));
	keyNames[SDL_SCANCODE_KP_8].assign(stringify(SDL_SCANCODE_KP_8));
	keyNames[SDL_SCANCODE_KP_9].assign(stringify(SDL_SCANCODE_KP_9));
	keyNames[SDL_SCANCODE_KP_0].assign(stringify(SDL_SCANCODE_KP_0));
	keyNames[SDL_SCANCODE_KP_PERIOD].assign(stringify(SDL_SCANCODE_KP_PERIOD));

	keyNames[SDL_SCANCODE_NONUSBACKSLASH].assign(stringify(SDL_SCANCODE_NONUSBACKSLASH)); /**< This is the additional key that ISO
										*   keyboards have over ANSI ones,
										*   located between left shift and Y.
										*   Produces GRAVE ACCENT and TILDE in a
										*   US or UK Mac layout, REVERSE SOLIDUS
										*   (backslash) and VERTICAL LINE in a
										*   US or UK Windows layout, and
										*   LESS-THAN SIGN and GREATER-THAN SIGN
										*   in a Swiss German, German, or French
										*   layout. */
	keyNames[SDL_SCANCODE_APPLICATION].assign(stringify(SDL_SCANCODE_APPLICATION)); /**< windows contextual menu, compose */
	keyNames[SDL_SCANCODE_POWER].assign(stringify(SDL_SCANCODE_POWER)); /**< The USB document says this is a status flag,
							   *   not a physical key - but some Mac keyboards
							   *   do have a power key. */
	keyNames[SDL_SCANCODE_KP_EQUALS].assign(stringify(SDL_SCANCODE_KP_EQUALS));
	keyNames[SDL_SCANCODE_F13].assign(stringify(SDL_SCANCODE_F13));
	keyNames[SDL_SCANCODE_F14].assign(stringify(SDL_SCANCODE_F14));
	keyNames[SDL_SCANCODE_F15].assign(stringify(SDL_SCANCODE_F15));
	keyNames[SDL_SCANCODE_F16].assign(stringify(SDL_SCANCODE_F16));
	keyNames[SDL_SCANCODE_F17].assign(stringify(SDL_SCANCODE_F17));
	keyNames[SDL_SCANCODE_F18].assign(stringify(SDL_SCANCODE_F18));
	keyNames[SDL_SCANCODE_F19].assign(stringify(SDL_SCANCODE_F19));
	keyNames[SDL_SCANCODE_F20].assign(stringify(SDL_SCANCODE_F20));
	keyNames[SDL_SCANCODE_F21].assign(stringify(SDL_SCANCODE_F21));
	keyNames[SDL_SCANCODE_F22].assign(stringify(SDL_SCANCODE_F22));
	keyNames[SDL_SCANCODE_F23].assign(stringify(SDL_SCANCODE_F23));
	keyNames[SDL_SCANCODE_F24].assign(stringify(SDL_SCANCODE_F24));
	keyNames[SDL_SCANCODE_EXECUTE].assign(stringify(SDL_SCANCODE_EXECUTE));
	keyNames[SDL_SCANCODE_HELP].assign(stringify(SDL_SCANCODE_HELP));
	keyNames[SDL_SCANCODE_MENU].assign(stringify(SDL_SCANCODE_MENU));
	keyNames[SDL_SCANCODE_SELECT].assign(stringify(SDL_SCANCODE_SELECT));
	keyNames[SDL_SCANCODE_STOP].assign(stringify(SDL_SCANCODE_STOP));
	keyNames[SDL_SCANCODE_AGAIN].assign(stringify(SDL_SCANCODE_AGAIN));   /**< redo */
	keyNames[SDL_SCANCODE_UNDO].assign(stringify(SDL_SCANCODE_UNDO));
	keyNames[SDL_SCANCODE_CUT].assign(stringify(SDL_SCANCODE_CUT));
	keyNames[SDL_SCANCODE_COPY].assign(stringify(SDL_SCANCODE_COPY));
	keyNames[SDL_SCANCODE_PASTE].assign(stringify(SDL_SCANCODE_PASTE));
	keyNames[SDL_SCANCODE_FIND].assign(stringify(SDL_SCANCODE_FIND));
	keyNames[SDL_SCANCODE_MUTE].assign(stringify(SDL_SCANCODE_MUTE));
	keyNames[SDL_SCANCODE_VOLUMEUP].assign(stringify(SDL_SCANCODE_VOLUMEUP));
	keyNames[SDL_SCANCODE_VOLUMEDOWN].assign(stringify(SDL_SCANCODE_VOLUMEDOWN));
	/* not sure whether there's a reason to enable these */
	/*     keyNames[SDL_SCANCODE_LOCKINGCAPSLOCK].assign(stringify(A)); 130,  */
	/*     keyNames[SDL_SCANCODE_LOCKINGNUMLOCK].assign(stringify(A)); 131, */
	/*     keyNames[SDL_SCANCODE_LOCKINGSCROLLLOCK].assign(stringify(A)); 132, */
	keyNames[SDL_SCANCODE_KP_COMMA].assign(stringify(SDL_SCANCODE_KP_COMMA));
	keyNames[SDL_SCANCODE_KP_EQUALSAS400].assign(stringify(SDL_SCANCODE_KP_EQUALSAS400));

	keyNames[SDL_SCANCODE_INTERNATIONAL1].assign(stringify(SDL_SCANCODE_INTERNATIONAL1)); /**< used on Asian keyboards, see
											footnotes in USB do
	keyNames[SDL_SCANCODE_INTERNATIONAL2].assign(stringify(A));
	keyNames[SDL_SCANCODE_INTERNATIONAL3].assign(stringify(A)); /**< Yen */
	keyNames[SDL_SCANCODE_INTERNATIONAL4].assign(stringify(SDL_SCANCODE_INTERNATIONAL4));
	keyNames[SDL_SCANCODE_INTERNATIONAL5].assign(stringify(SDL_SCANCODE_INTERNATIONAL5));
	keyNames[SDL_SCANCODE_INTERNATIONAL6].assign(stringify(SDL_SCANCODE_INTERNATIONAL6));
	keyNames[SDL_SCANCODE_INTERNATIONAL7].assign(stringify(SDL_SCANCODE_INTERNATIONAL7));
	keyNames[SDL_SCANCODE_INTERNATIONAL8].assign(stringify(SDL_SCANCODE_INTERNATIONAL8));
	keyNames[SDL_SCANCODE_INTERNATIONAL9].assign(stringify(SDL_SCANCODE_INTERNATIONAL9));
	keyNames[SDL_SCANCODE_LANG1].assign(stringify(SDL_SCANCODE_LANG1)); /**< Hangul/English toggle */
	keyNames[SDL_SCANCODE_LANG2].assign(stringify(SDL_SCANCODE_LANG2)); /**< Hanja conversion */
	keyNames[SDL_SCANCODE_LANG3].assign(stringify(SDL_SCANCODE_LANG3)); /**< Katakana */
	keyNames[SDL_SCANCODE_LANG4].assign(stringify(SDL_SCANCODE_LANG4)); /**< Hiragana */
	keyNames[SDL_SCANCODE_LANG5].assign(stringify(SDL_SCANCODE_LANG5)); /**< Zenkaku/Hankaku */
	keyNames[SDL_SCANCODE_LANG6].assign(stringify(SDL_SCANCODE_LANG6)); /**< reserved */
	keyNames[SDL_SCANCODE_LANG7].assign(stringify(SDL_SCANCODE_LANG7)); /**< reserved */
	keyNames[SDL_SCANCODE_LANG8].assign(stringify(SDL_SCANCODE_LANG8)); /**< reserved */
	keyNames[SDL_SCANCODE_LANG9].assign(stringify(SDL_SCANCODE_LANG9)); /**< reserved */

	keyNames[SDL_SCANCODE_ALTERASE].assign(stringify(SDL_SCANCODE_ALTERASE)); /**< Erase-Eaze */
	keyNames[SDL_SCANCODE_SYSREQ].assign(stringify(SDL_SCANCODE_SYSREQ));
	keyNames[SDL_SCANCODE_CANCEL].assign(stringify(SDL_SCANCODE_CANCEL));
	keyNames[SDL_SCANCODE_CLEAR].assign(stringify(SDL_SCANCODE_CLEAR));
	keyNames[SDL_SCANCODE_PRIOR].assign(stringify(SDL_SCANCODE_PRIOR));
	keyNames[SDL_SCANCODE_RETURN2].assign(stringify(SDL_SCANCODE_RETURN2));
	keyNames[SDL_SCANCODE_SEPARATOR].assign(stringify(SDL_SCANCODE_SEPARATOR));
	keyNames[SDL_SCANCODE_OUT].assign(stringify(SDL_SCANCODE_OUT));
	keyNames[SDL_SCANCODE_OPER].assign(stringify(SDL_SCANCODE_OPER));
	keyNames[SDL_SCANCODE_CLEARAGAIN].assign(stringify(SDL_SCANCODE_CLEARAGAIN));
	keyNames[SDL_SCANCODE_CRSEL].assign(stringify(SDL_SCANCODE_CRSEL));
	keyNames[SDL_SCANCODE_EXSEL].assign(stringify(SDL_SCANCODE_EXSEL));

	keyNames[SDL_SCANCODE_KP_00].assign(stringify(SDL_SCANCODE_KP_00));
	keyNames[SDL_SCANCODE_KP_000].assign(stringify(SDL_SCANCODE_KP_000));
	keyNames[SDL_SCANCODE_THOUSANDSSEPARATOR].assign(stringify(SDL_SCANCODE_THOUSANDSSEPARATOR));
	keyNames[SDL_SCANCODE_DECIMALSEPARATOR].assign(stringify(SDL_SCANCODE_DECIMALSEPARATOR));
	keyNames[SDL_SCANCODE_CURRENCYUNIT].assign(stringify(SDL_SCANCODE_CURRENCYUNIT));
	keyNames[SDL_SCANCODE_CURRENCYSUBUNIT].assign(stringify(SDL_SCANCODE_CURRENCYSUBUNIT));
	keyNames[SDL_SCANCODE_KP_LEFTPAREN].assign(stringify(SDL_SCANCODE_KP_LEFTPAREN));
	keyNames[SDL_SCANCODE_KP_RIGHTPAREN].assign(stringify(SDL_SCANCODE_KP_RIGHTPAREN));
	keyNames[SDL_SCANCODE_KP_LEFTBRACE].assign(stringify(SDL_SCANCODE_KP_LEFTBRACE));
	keyNames[SDL_SCANCODE_KP_RIGHTBRACE].assign(stringify(SDL_SCANCODE_KP_RIGHTBRACE));
	keyNames[SDL_SCANCODE_KP_TAB].assign(stringify(SDL_SCANCODE_KP_TAB));
	keyNames[SDL_SCANCODE_KP_BACKSPACE].assign(stringify(SDL_SCANCODE_KP_BACKSPACE));
	keyNames[SDL_SCANCODE_KP_A].assign(stringify(SDL_SCANCODE_KP_A));
	keyNames[SDL_SCANCODE_KP_B].assign(stringify(SDL_SCANCODE_KP_B));
	keyNames[SDL_SCANCODE_KP_C].assign(stringify(SDL_SCANCODE_KP_C));
	keyNames[SDL_SCANCODE_KP_D].assign(stringify(SDL_SCANCODE_KP_D));
	keyNames[SDL_SCANCODE_KP_E].assign(stringify(SDL_SCANCODE_KP_E));
	keyNames[SDL_SCANCODE_KP_F].assign(stringify(SDL_SCANCODE_KP_F));
	keyNames[SDL_SCANCODE_KP_XOR].assign(stringify(SDL_SCANCODE_KP_XOR));
	keyNames[SDL_SCANCODE_KP_POWER].assign(stringify(SDL_SCANCODE_KP_POWER));
	keyNames[SDL_SCANCODE_KP_PERCENT].assign(stringify(SDL_SCANCODE_KP_PERCENT));
	keyNames[SDL_SCANCODE_KP_LESS].assign(stringify(SDL_SCANCODE_KP_LESS));
	keyNames[SDL_SCANCODE_KP_GREATER].assign(stringify(SDL_SCANCODE_KP_GREATER));
	keyNames[SDL_SCANCODE_KP_AMPERSAND].assign(stringify(SDL_SCANCODE_KP_AMPERSAND));
	keyNames[SDL_SCANCODE_KP_DBLAMPERSAND].assign(stringify(SDL_SCANCODE_KP_DBLAMPERSAND));
	keyNames[SDL_SCANCODE_KP_VERTICALBAR].assign(stringify(SDL_SCANCODE_KP_VERTICALBAR));
	keyNames[SDL_SCANCODE_KP_DBLVERTICALBAR].assign(stringify(SDL_SCANCODE_KP_DBLVERTICALBAR));
	keyNames[SDL_SCANCODE_KP_COLON].assign(stringify(SDL_SCANCODE_KP_COLON));
	keyNames[SDL_SCANCODE_KP_HASH].assign(stringify(SDL_SCANCODE_KP_HASH));
	keyNames[SDL_SCANCODE_KP_SPACE].assign(stringify(SDL_SCANCODE_KP_SPACE));
	keyNames[SDL_SCANCODE_KP_AT].assign(stringify(SDL_SCANCODE_KP_AT));
	keyNames[SDL_SCANCODE_KP_EXCLAM].assign(stringify(SDL_SCANCODE_KP_EXCLAM));
	keyNames[SDL_SCANCODE_KP_MEMSTORE].assign(stringify(SDL_SCANCODE_KP_MEMSTORE));
	keyNames[SDL_SCANCODE_KP_MEMRECALL].assign(stringify(SDL_SCANCODE_KP_MEMRECALL));
	keyNames[SDL_SCANCODE_KP_MEMCLEAR].assign(stringify(SDL_SCANCODE_KP_MEMCLEAR));
	keyNames[SDL_SCANCODE_KP_MEMADD].assign(stringify(SDL_SCANCODE_KP_MEMADD));
	keyNames[SDL_SCANCODE_KP_MEMSUBTRACT].assign(stringify(SDL_SCANCODE_KP_MEMSUBTRACT));
	keyNames[SDL_SCANCODE_KP_MEMMULTIPLY].assign(stringify(SDL_SCANCODE_KP_MEMMULTIPLY));
	keyNames[SDL_SCANCODE_KP_MEMDIVIDE].assign(stringify(SDL_SCANCODE_KP_MEMDIVIDE));
	keyNames[SDL_SCANCODE_KP_PLUSMINUS].assign(stringify(SDL_SCANCODE_KP_PLUSMINUS));
	keyNames[SDL_SCANCODE_KP_CLEAR].assign(stringify(SDL_SCANCODE_KP_CLEAR));
	keyNames[SDL_SCANCODE_KP_CLEARENTRY].assign(stringify(SDL_SCANCODE_KP_CLEARENTRY));
	keyNames[SDL_SCANCODE_KP_BINARY].assign(stringify(SDL_SCANCODE_KP_BINARY));
	keyNames[SDL_SCANCODE_KP_OCTAL].assign(stringify(SDL_SCANCODE_KP_OCTAL));
	keyNames[SDL_SCANCODE_KP_DECIMAL].assign(stringify(SDL_SCANCODE_KP_DECIMAL));
	keyNames[SDL_SCANCODE_KP_HEXADECIMAL].assign(stringify(SDL_SCANCODE_KP_HEXADECIMAL));

	keyNames[SDL_SCANCODE_LCTRL].assign(stringify(LCTRL));
	keyNames[SDL_SCANCODE_LSHIFT].assign(stringify(LSHIFT));
	keyNames[SDL_SCANCODE_LALT].assign(stringify(LALT)); /**< alt, option */
	keyNames[SDL_SCANCODE_LGUI].assign(stringify(LGUI)); /**< windows, command (apple), meta */
	keyNames[SDL_SCANCODE_RCTRL].assign(stringify(RCTRL));
	keyNames[SDL_SCANCODE_RSHIFT].assign(stringify(RSHIFT));
	keyNames[SDL_SCANCODE_RALT].assign(stringify(RALT)); /**< alt gr, option */
	keyNames[SDL_SCANCODE_RGUI].assign(stringify(RGUI)); /**< windows, command (apple), meta */

	keyNames[SDL_SCANCODE_MODE].assign(stringify(SDL_SCANCODE_MODE)); 257,    /**< I'm not sure if this is really not covered
								 *   by any of the above, but since there's a
								 *   special KMOD_MODE for it I'm adding it here
								 */

								 /* @} *//* Usage page 0x07 */

								 /**
								  *  \name Usage page 0x0C
								  *
								  *  These values are mapped from usage page 0x0C (USB consumer page).
								  */
								  /* @{ */

	keyNames[SDL_SCANCODE_AUDIONEXT].assign(stringify(SDL_SCANCODE_AUDIONEXT));
	keyNames[SDL_SCANCODE_AUDIOPREV].assign(stringify(SDL_SCANCODE_AUDIOPREV));
	keyNames[SDL_SCANCODE_AUDIOSTOP].assign(stringify(SDL_SCANCODE_AUDIOSTOP));
	keyNames[SDL_SCANCODE_AUDIOPLAY].assign(stringify(SDL_SCANCODE_AUDIOPLAY));
	keyNames[SDL_SCANCODE_AUDIOMUTE].assign(stringify(SDL_SCANCODE_AUDIOMUTE));
	keyNames[SDL_SCANCODE_MEDIASELECT].assign(stringify(A));
	keyNames[SDL_SCANCODE_WWW].assign(stringify(A));
	keyNames[SDL_SCANCODE_MAIL].assign(stringify(A));
	keyNames[SDL_SCANCODE_CALCULATOR].assign(stringify(A));
	keyNames[SDL_SCANCODE_COMPUTER].assign(stringify(A));
	keyNames[SDL_SCANCODE_AC_SEARCH].assign(stringify(A));
	keyNames[SDL_SCANCODE_AC_HOME].assign(stringify(A));
	keyNames[SDL_SCANCODE_AC_BACK].assign(stringify(A));
	keyNames[SDL_SCANCODE_AC_FORWARD].assign(stringify(A));
	keyNames[SDL_SCANCODE_AC_STOP].assign(stringify(A));
	keyNames[SDL_SCANCODE_AC_REFRESH].assign(stringify(A));
	keyNames[SDL_SCANCODE_AC_BOOKMARKS].assign(stringify(A));

	/* @} *//* Usage page 0x0C */

	/**
	 *  \name Walther keys
	 *
	 *  These are values that Christian Walther added (for mac keyboard?).
	 */
	 /* @{ */

	keyNames[SDL_SCANCODE_BRIGHTNESSDOWN].assign(stringify(A));
	keyNames[SDL_SCANCODE_BRIGHTNESSUP].assign(stringify(A));
	keyNames[SDL_SCANCODE_DISPLAYSWITCH].assign(stringify(A)); /**< display mirroring/dual display
										   switch, video mode switch */
	keyNames[SDL_SCANCODE_KBDILLUMTOGGLE].assign(stringify(A));
	keyNames[SDL_SCANCODE_KBDILLUMDOWN].assign(stringify(A));
	keyNames[SDL_SCANCODE_KBDILLUMUP].assign(stringify(A));
	keyNames[SDL_SCANCODE_EJECT].assign(stringify(A));
	keyNames[SDL_SCANCODE_SLEEP].assign(stringify(A));

	keyNames[SDL_SCANCODE_APP1].assign(stringify(A));
	keyNames[SDL_SCANCODE_APP2].assign(stringify(A));
}