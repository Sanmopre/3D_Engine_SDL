#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Window.h"
#include "j1Render.h"
#include "j1Engine3D.h"


#define VSYNC true

j1Render::j1Render() : j1Module()
{
	name.create("renderer");
	background.r = 0;
	background.g = 0;
	background.b = 0;
	background.a = 0;
}

// Destructor
j1Render::~j1Render()
{}

// Called before render is available
bool j1Render::Awake(pugi::xml_node& config)
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

	renderer = SDL_CreateRenderer(App->win->window, -1, flags);

	if(renderer == NULL)
	{
		LOG("Could not create the renderer! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		camera.w = App->win->screen_surface->w;
		camera.h = App->win->screen_surface->h;
		camera.x = 0;
		camera.y = 0;
	}

	return ret;
}

// Called before the first frame
bool j1Render::Start()
{
	LOG("render start");
	// back background
	SDL_RenderGetViewport(renderer, &viewport);

	rect = { 0,0,App->engine->resolution,App->engine->resolution };

	return true;
}

// Called each loop iteration
bool j1Render::PreUpdate()
{
	SDL_RenderClear(renderer);
	return true;
}

bool j1Render::Update(float dt)
{
	return true;
}

bool j1Render::PostUpdate()
{
	SDL_SetRenderDrawColor(renderer, background.r, background.g, background.g, background.a);
	SDL_RenderPresent(renderer);
	return true;
}

// Called before quitting
bool j1Render::CleanUp()
{
	LOG("Destroying SDL render");
	SDL_DestroyRenderer(renderer);
	return true;
}

// Load Game State
bool j1Render::Load(pugi::xml_node& data)
{
	camera.x = data.child("camera").attribute("x").as_int();
	camera.y = data.child("camera").attribute("y").as_int();

	return true;
}

// Save Game State
bool j1Render::Save(pugi::xml_node& data) const
{
	pugi::xml_node cam = data.append_child("camera");

	cam.append_attribute("x") = camera.x;
	cam.append_attribute("y") = camera.y;

	return true;
}

void j1Render::SetBackgroundColor(SDL_Color color)
{
	background = color;
}

void j1Render::SetViewPort(const SDL_Rect& rect)
{
	SDL_RenderSetViewport(renderer, &rect);
}

void j1Render::ResetViewPort()
{
	SDL_RenderSetViewport(renderer, &viewport);
}

// Blit to screen
bool j1Render::Blit(SDL_Texture* texture, int x, int y, const SDL_Rect* section, float speed, double angle, int pivot_x, int pivot_y) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_Rect rect;
	rect.x = (int)(camera.x * speed) + x * scale;
	rect.y = (int)(camera.y * speed) + y * scale;

	if(section != NULL)
	{
		rect.w = section->w;
		rect.h = section->h;
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}

	rect.w *= scale;
	rect.h *= scale;

	SDL_Point* p = NULL;
	SDL_Point pivot;

	if(pivot_x != INT_MAX && pivot_y != INT_MAX)
	{
		pivot.x = pivot_x;
		pivot.y = pivot_y;
		p = &pivot;
	}

	if(SDL_RenderCopyEx(renderer, texture, section, &rect, angle, p, SDL_FLIP_NONE) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool j1Render::DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool filled, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

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

bool j1Render::DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

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

bool j1Render::DrawCircle(int x, int y, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

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

void j1Render::DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3)
{
	DrawLine(x1,y1,x2,y2,200, 200, 200, 100, true);
	DrawLine(x2, y2, x3, y3, 200, 200, 200, 100, true);
	DrawLine(x3, y3, x1, y1, 200, 200, 200, 100, true);
}

float j1Render::Area(int x1, int y1, int x2, int y2, int x3, int y3)
{
	return abs((x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2)) / 2.0);
}

bool j1Render::IsInside(int x1, int y1, int x2, int y2, int x3, int y3, int x, int y)
{
	float A = Area(x1, y1, x2, y2, x3, y3);
	float A1 = Area(x, y, x2, y2, x3, y3);
	float A2 = Area(x1, y1, x, y, x3, y3);
	float A3 = Area(x1, y1, x2, y2, x, y);
	return (A == A1 + A2 + A3);
}

void j1Render::DrawFilledTriangle(int x1, int y1, int x2, int y2, int x3, int y3, float a)
{
	int min_x;
	int max_x;
	int min_y;
	int max_y;
	
	//min x
	min_x = x1;
	if (min_x > x2) {
		min_x = x2;
	}
	if (min_x > x3) {
		min_x = x3;
	}

	//max x
	max_x = x1;
	if (max_x < x2) {
		max_x = x2;
	}
	if (max_x < x3) {
		max_x = x3;
	}

	//min y
	min_y = y1;
	if (min_y > y2) {
		min_y = y2;
	}
	if (min_y > y3) {
		min_y = y3;
	}

	//max y
	max_y = y1;
	if (max_y < y2) {
		max_y = y2;
	}
	if (max_y < y3) {
		max_y = y3;
	}



		for (int k = min_x; k <= max_x; ++k) {
			int first_y = 0;
			int last_y = 0;
			bool in_triangle = false;
			bool first_time = true;
			for (int i = min_y; i <= max_y; ++i) {

			if (IsInside(x1, y1, x2, y2, x3, y3, k, i)) {
				if (in_triangle == false) {
					first_y = i;
					in_triangle = true;
				}
			}
			else {
				if (in_triangle == true && first_time == true) {
					last_y = i - 1;
					first_time = false;
				}
			}

			if (i == max_y && IsInside(x1, y1, x2, y2, x3, y3, k, i)) {
				last_y = i;
			}


			}
				DrawLine(k, first_y, k, last_y, 0, a * 128, a * 255, 255, true);
			}
		
		

}

void j1Render::DrawTriangleLowRes(int res, int x1, int y1, int x2, int y2, int x3, int y3, float a)
{
	int min_x;
	int max_x;
	int min_y;
	int max_y;

	//min x
	min_x = x1;
	if (min_x > x2) {
		min_x = x2;
	}
	if (min_x > x3) {
		min_x = x3;
	}

	//max x
	max_x = x1;
	if (max_x < x2) {
		max_x = x2;
	}
	if (max_x < x3) {
		max_x = x3;
	}

	//min y
	min_y = y1;
	if (min_y > y2) {
		min_y = y2;
	}
	if (min_y > y3) {
		min_y = y3;
	}

	//max y
	max_y = y1;
	if (max_y < y2) {
		max_y = y2;
	}
	if (max_y < y3) {
		max_y = y3;
	}

	
	min_x /= res; 
	max_x /= res;
	min_y /= res;
	max_y /= res;

	for (int k = min_x + res/2; k <= max_x; k = k + res) {
		for (int i = min_y + res / 2; i <= max_y; i = i + res) {
			if (IsInside(x1/res, y1/ res, x2/ res, y2/ res, x3/ res, y3/ res, k, i)) {
				DrawQuad({ k * res,i * res,res * res,res * res }, 0, a * 128, a * 255, 255, true, false);
			}
		}
	}
}



