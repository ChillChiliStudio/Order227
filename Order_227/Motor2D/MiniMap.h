#include "SDL\include\SDL.h"
#include "App.h"
#include "Module.h"
#include "Point.h"

struct SDL_Texture;
struct SDL_Rect;

class MiniMap : public Module
{
public:
	MiniMap();
	virtual ~MiniMap();

	bool Awake(pugi::xml_node& config);

	bool Start();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();
	bool Draw();

private:

	bool MinimapCoords(int& map_x, int& map_y);
	void DrawCamera();
	void MinimapBorders();
	void DrawEntities();


public:

	iPoint minimapPosition;

	int minimap_width;
	int minimap_height;

private:
	SDL_Texture*		minimap_tex = nullptr;

	float minimapScale;

	int x_offset;
	int y_offset;

	float map_width;

	bool minimapClicked=false;

};

