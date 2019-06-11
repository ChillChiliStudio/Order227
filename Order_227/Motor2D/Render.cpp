#include "Defs.h"
#include "Log.h"
#include "Window.h"
#include "Render.h"
#include "Entity_Manager.h"

#include "Brofiler/Brofiler.h"

#define VSYNC true

Render::Render() : Module()
{
	name.assign("renderer");
	background.r = 0;
	background.g = 0;
	background.b = 0;
	background.a = 0;
}

// Destructor
Render::~Render()
{}

// Called before render is available
bool Render::Awake(pugi::xml_node& config)
{
	LOG("Create SDL rendering context");
	bool ret = true;
	// load flags
	Uint32 flags = SDL_RENDERER_ACCELERATED;

	if(config.child("vsync").attribute("value").as_bool(true) == true)
	{
		flags |= SDL_RENDERER_PRESENTVSYNC;
		LOG("Using vsync");
	}

	renderer = SDL_CreateRenderer(myApp->win->window, -1, flags);

	if(renderer == NULL)
	{
		LOG("Could not create the renderer! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		camera.w = myApp->win->screen_surface->w;
		camera.h = myApp->win->screen_surface->h;
		camera.x = 400;
		camera.y = -1000;
	}

	return ret;
}

// Called before the first frame
bool Render::Start()
{
	LOG("render start");
	// back background
	SDL_RenderGetViewport(renderer, &viewport);
	SDL_RenderSetLogicalSize(renderer,1280,720);
	return true;
}

// Called each loop iteration
bool Render::PreUpdate()
{
	BROFILER_CATEGORY("Render Pre-Update", Profiler::Color::AliceBlue);

	SDL_RenderClear(renderer);
	return true;
}

bool Render::Update(float dt)
{
	BROFILER_CATEGORY("Render Update", Profiler::Color::Blue);

	return true;
}

bool Render::PostUpdate()
{
	BROFILER_CATEGORY("Render Post-Update", Profiler::Color::BlueViolet);

	SDL_SetRenderDrawColor(renderer, background.r, background.g, background.g, background.a);
	SDL_RenderPresent(renderer);
	return true;
}

// Called before quitting
bool Render::CleanUp()
{
	LOG("Destroying SDL render");
	SDL_DestroyRenderer(renderer);
	return true;
}

void Render::SetBackgroundColor(SDL_Color color)
{
	background = color;
}

void Render::SetViewPort(const SDL_Rect& rect)
{
	SDL_RenderSetViewport(renderer, &rect);
}

void Render::ResetViewPort()
{
	SDL_RenderSetViewport(renderer, &viewport);
}

void Render::ScreenToWorld(int* x, int* y) const
{
	int scale = myApp->win->GetScale();

	x = (x - camera.x / scale);
	y = (y - camera.y / scale);
}

iPoint Render::ScreenToWorld(int x, int y) const
{
	iPoint ret;
	int scale = myApp->win->GetScale();

	ret.x = (x - camera.x / scale);
	ret.y = (y - camera.y / scale);

	return ret;
}

// Blit to screen
bool Render::Blit(SDL_Texture* texture, int x, int y, const SDL_Rect* section, SDL_RendererFlip flip, bool use_camera, float scale_, SDL_Renderer* renderer_, float speed, double angle, int pivot_x, int pivot_y) const
{
	bool ret = true;
	//if(scale_!=1.0f)
	//	uint scale = myApp->win->GetScale();

	SDL_Rect rect;

	if (use_camera){
		rect.x = (int)(camera.x * speed) + x;
		rect.y = (int)(camera.y * speed) + y;
	}
	else{
		rect.x = x;
		rect.y = y;
	}

	if(section != NULL)
	{
		rect.w = section->w;
		rect.h = section->h;
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}

	rect.w *= scale_;
	rect.h *= scale_;

	SDL_Point* p = NULL;
	SDL_Point pivot;

	if(pivot_x != INT_MAX && pivot_y != INT_MAX)
	{
		pivot.x = pivot_x;
		pivot.y = pivot_y;
		p = &pivot;
	}

	if(SDL_RenderCopyEx(renderer_, texture, section, &rect, angle, p, flip) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

//This function prints all the elements of the queue

bool Render::OrderBlit(std::priority_queue <ImageRender*, std::vector<ImageRender*>, Comparer>& Queue)const {

	bool ret = true;
	while (Queue.empty() == false) {
		ImageRender* Image = Queue.top();
		uint size = myApp->win->GetScale();
		SDL_Rect r;
		r.x = (int)(camera.x * Image->speed) + Image->x * size;
		r.y = (int)(camera.y * Image->speed) + Image->y * size;
		if (Image->section != NULL) {
			r.w = Image->section->w;
			r.h = Image->section->h;
		}
		else {
			SDL_QueryTexture(Image->tex, NULL, NULL, &r.w, &r.h);
		}
		SDL_RendererFlip flag;
		if (Image->scale < 0) {
			flag = SDL_FLIP_HORIZONTAL;
			r.w = (int)(r.w * -Image->scale);
			r.h = (int)(r.h * -Image->scale);
			//r.w *= -Image->scale;
			//r.h *= -Image->scale;
		}
		else {
			flag = SDL_FLIP_NONE;
			r.w = (int)(r.w * Image->scale);
			r.h = (int)(r.h * Image->scale);
			//r.w *= Image->scale;
			//r.h *= Image->scale;
		}

		SDL_Point* point = NULL;
		SDL_Point aux;

		if (Image->pivot_x != INT_MAX && Image->pivot_y != INT_MAX) {
			aux.x = Image->pivot_x;
			aux.y = Image->pivot_y;
			point = &aux;
		}
		if (SDL_RenderCopyEx(renderer, Image->tex, Image->section, &r, Image->angle, point, flag) != 0) {
			LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
			ret = false;
		}
		RELEASE(Image);
		Queue.pop();
	}
	
	/*for (int i = 0; i < myApp->map->Rectvec.size(); i++) {
		RELEASE(myApp->map->Rectvec[i]);
	}
	myApp->map->Rectvec.clear();*/
	
	return ret;
}

bool Render::DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool filled, bool use_camera) const
{
	bool ret = true;	//TODO: Uncomment and see why it fucks up (Symbol file not loaded)
	uint scale = myApp->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	SDL_Rect rec(rect);
	if(use_camera)
	{
		rec.x = (int)(camera.x + rect.x * scale);
		rec.y = (int)(camera.y + rect.y * scale);
		rec.w *= scale;
		rec.h *= scale;
	}

	int result = (filled) ? SDL_RenderFillRect(renderer, &rec) : SDL_RenderDrawRect(renderer, &rec);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Render::DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;	//TODO: Uncomment and see why it fucks up (Symbol file not loaded)
	uint scale = myApp->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;

	if(use_camera)
		result = SDL_RenderDrawLine(renderer, camera.x + x1 * scale, camera.y + y1 * scale, camera.x + x2 * scale, camera.y + y2 * scale);
	else
	result = SDL_RenderDrawLine(renderer, x1 * scale, y1 * scale, x2 * scale, y2 * scale);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Render::DrawCircle(int x, int y, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;	//TODO: Uncomment and see why it fucks up (Symbol file not loaded)
	uint scale = myApp->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	if (use_camera) {
		x = camera.x + x;
		y = camera.y + y;
	}

	int result = -1;
	SDL_Point points[360];

	float factor = (float)M_PI / 180.0f;

	for(uint i = 0; i < 360; ++i)
	{
		points[i].x = (int)(x + radius * cos(i * factor));
		points[i].y = (int)(y + radius * sin(i * factor));
	}

	result = SDL_RenderDrawPoints(renderer, points, 360);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

//Checks if the rect is inside the camera
bool Render::InsideCamera(const SDL_Rect& rect)const {

	bool ret = false;

	if ((rect.x + rect.w > -camera.x && rect.x < (-camera.x + camera.w)) &&
		(rect.y + rect.h > -camera.y && rect.y < (-camera.y + camera.h))) {

		ret = true;
	}

	return ret;
}

//This fucntions create a new element for the Queue with the info of the class ImageRender
void Render::Push(uint order, SDL_Texture* tex, int x, int y, const SDL_Rect* section, float scale, float speed, double angle, int pivot_x, int pivot_y)
{

	SDL_Rect r;
	r.x = x;
	r.y = y;
	if (section != NULL) {
		r.w = section->w;
		r.h = section->h;
	}
	else {
		SDL_QueryTexture(tex, NULL, NULL, &r.w, &r.h);
	}
	
	if (InsideCamera(r)) {
			ImageRender* auxObject = new ImageRender(order, tex, x, y, section, scale, speed, angle, pivot_x, pivot_y, r);
			OrderToRender.push(auxObject);
	}

}

