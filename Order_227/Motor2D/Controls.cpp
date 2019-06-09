#include "Controls.h"
#include "App.h"
#include "Input.h"

#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "Input.h"
#include "Window.h"
#include "SDL/include/SDL.h"

Controls::Controls()
{
	name.assign("controls");
}

Controls::~Controls()
{}

bool Controls::Awake(pugi::xml_node& node)
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

bool Controls::Start()
{
	for (int i = 0; i < 10; i++)
		spawnHotKeys[i] = SDL_SCANCODE_1 + i;

	AllocateLists();

	return true;
}

bool Controls::PreUpdate()
{
	return true;
}

bool Controls::Update(float dt)
{
	if (myApp->input->GetKey(SDL_SCANCODE_O) == KEY_DOWN) {
		PrepareInputChange(orders.patrol);
	}

	if (awaitingInput) {

		const Uint8* keys = SDL_GetKeyboardState(NULL);

		for (int i = 0; i < MAX_KEYS; ++i) {

			if (keys[i] == 1) {
				if (KeyInUse(i)) {
					//TODO: Play error sound
				}
				else {
					UpdateKey(i);
				}

				awaitingInput = false;
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
	for (int i = 0; i < 10; i++)
		keysInUse.push_back(spawnHotKeys[i]);

	//Keys - Others
	keysInUse.push_back(pause);
}

void Controls::PrepareInputChange(int& selectedInput)
{
	inputToChange = &selectedInput;
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
	UpdateConfig(newVal);

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
	UpdateConfig(newVal);

	for (int i = 0; i < keysInUse.size(); i++) {
		if (keysInUse[i] == *inputToChange) {
			keysInUse[i] = newVal;
			break;
		}
	}
	
	*inputToChange = newVal;
	inputToChange = nullptr;
}

void Controls::UpdateConfig(int newVal)
{
	pugi::xml_document	config_file;
	pugi::xml_node		config;

	pugi::xml_parse_result result = config_file.load_file("config.xml");

	if (result == NULL)
		LOG("Could not load map xml file config.xml. pugi error: %s", result.description());
	else
		config = config_file.child("config");

	if (config.empty() == false)
	{
		config = config.child("controls");

		for (pugi::xml_node i = config.first_child(); i; i = i.next_sibling()) {
			for (pugi::xml_attribute j = i.first_attribute(); j; j = j.next_attribute()) {
				if (j.as_int() == *inputToChange) {
					j.set_value(newVal);
					break;
				}
			}
		}

		config_file.save_file("config.xml");
	}
}