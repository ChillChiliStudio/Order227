#ifndef RENDER_H
#define RENDER_H

#include <queue>
#include <vector>
#include "SDL/include/SDL.h"
#include "Point.h"
#include "Module.h"

class ImageRender
{
public:

	ImageRender(uint order, SDL_Texture* texture, int x, int y, const SDL_Rect* section, float scale, float speed, double angle, int pivot_x, int pivot_y, SDL_Rect rect) :
		order(order), tex(texture), x(x), y(y), section(section), scale(scale), speed(speed), angle(angle), pivot_x(pivot_x), pivot_y(pivot_y), rect(rect) {}

	uint Ordering()const
	{
		return order;
	}

public:
	SDL_Texture*		tex;
	int					x;
	int					y;
	const SDL_Rect*		section;
	SDL_Rect			rect;
	float				scale;
	float				speed;
	double				angle;
	int					pivot_x;
	int					pivot_y;

	uint				order;
};

struct Comparer
{
	bool operator()(const ImageRender* img1, const ImageRender* img2)const
	{
		return img1->Ordering() > img2->Ordering();
	}
};

class Render : public Module
{
public:

	Render();

	// Destructor
	virtual ~Render();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Utils
	void SetViewPort(const SDL_Rect& rect);
	void ResetViewPort();
	void ScreenToWorld(int* x, int* y) const;
	iPoint ScreenToWorld(int x, int y) const;

	// Draw & Blit
	bool Blit(SDL_Texture* texture, int x, int y, const SDL_Rect* section = NULL, float speed = 1.0f, double angle = 0, int pivot_x = INT_MAX, int pivot_y = INT_MAX, float scale_ = 1.0f ) const;
	bool OrderBlit(std::priority_queue <ImageRender*, std::vector<ImageRender*>, Comparer>& Queue)const;//
	bool DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool filled = true, bool use_camera = true) const;
	bool DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool use_camera = true) const;
	bool DrawCircle(int x1, int y1, int redius, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool use_camera = true) const;
	bool InsideCamera(const SDL_Rect& rect)const;

	void Push(uint order, SDL_Texture* texture, int x, int y, const SDL_Rect* section = NULL, float scale = 1.0f, float speed = 1.0f, double angle = 0, int pivot_x = INT_MAX, int pivot_y = INT_MAX);

	// Set background color
	void SetBackgroundColor(SDL_Color color);



public:

	SDL_Renderer*	renderer;
	SDL_Rect		camera;
	SDL_Rect		viewport;
	SDL_Color		background;
	
	std::priority_queue <ImageRender*, std::vector<ImageRender*>, Comparer> OrderToRender;


};

#endif // RENDER_H