#include "SDL\include\SDL.h"
#include "App.h"
#include "Module.h"


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

private:

	bool MinimapCoords(int& map_x, int& map_y);
	void DrawCamera();
	void MinimapBorders();
	//void DrawEntities();
	void DrawMinimap();

private:
	SDL_Texture*		minimap_tex = nullptr;
	SDL_Texture*		tex = nullptr;

	SDL_Surface*		map_surface = nullptr;

	SDL_Renderer*		map_renderer = nullptr;

	float minimap_scale;

	int x_offset;
	int y_offset;

	float map_width;

	int minimap_width;
	int minimap_height;

};

