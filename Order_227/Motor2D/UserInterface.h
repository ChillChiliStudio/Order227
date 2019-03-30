#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H	// @Carles

#include "Module.h"

#define CURSOR_WIDTH 2

enum class ui_type;
class UI_Element;
class Image;
class Text;
//class Window;
struct _TTF_Font;
struct SDL_Rect;
struct SDL_Texture;

class User_Interface : public Module
{
public:

	User_Interface();

	// Destructor
	virtual ~User_Interface();

	// Called when before render is available
	bool Awake(pugi::xml_node&);

	// Call before first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each frame (logic)
	bool Update(float dt);

	// Called each frame (graphic)
	bool Draw();

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();
	
public:
	SDL_Texture* GetAtlas() const;
	
	void AddElement(UI_Element* element);
	void DestroyElement(UI_Element* element);
	std::list<UI_Element*>::iterator DestroyElement(std::list<UI_Element*>::iterator iter);

	UI_Element* CreateImage(fPoint center, SDL_Rect texRect = { 0, 0, 0, 0 }, SDL_Texture* tex = NULL, bool dynamic = false, UI_Element* parent = NULL, std::list<UI_Element*>* children = NULL);
	UI_Element* CreateText(fPoint center, const char* content, SDL_Color color = { 255, 255, 255, 255 }, _TTF_Font* font = NULL, bool dynamic = false, UI_Element* parent = NULL, std::list<UI_Element*>* children = NULL);

	template <class Ret, class... Args>
	UI_Element* CreateActionBox(Ret(*action)(Args...), fPoint center, SDL_Rect spriteList[4], SDL_Texture* tex = NULL, UI_Element* parent = NULL)
	{
		UI_Element* ret = nullptr;

		if (tex == NULL) {
			tex = GetAtlas();
		}

		ret = NewActionBox<Ret, Args...>(action, center, spriteList, tex, parent);
		AddElement(ret);

		return ret;
	}

	//UI_Element* CreateCheckBox(void(*action)(bool*), bool* value, fPoint center, SDL_Rect spriteList[3], SDL_Texture* tex, bool dynamic = false, UI_Element* parent = NULL, std::list<UI_Element*>* children = NULL);
	//UI_Element* CreateInputText();

	//Window* CreateWindowPanel(fPoint center, p2List<Image*> children, SDL_Rect* texRect = NULL, SDL_Texture* tex = NULL, Text* label = NULL, UI_Element* parent = NULL);
	
public:
	bool mustDebugDraw = false;
	//uint defaultScale;	//IMPROVE: Future use

private:
	std::list<UI_Element*> screenElements;

	SDL_Texture* atlas;
	std::string atlasFileName;
};

#endif // __USER_INTERFACE_H__