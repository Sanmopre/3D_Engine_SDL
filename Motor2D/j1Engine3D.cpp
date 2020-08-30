#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Engine3D.h"

j1Engine3D::j1Engine3D() : j1Module()
{
	name.create("engine");
}


j1Engine3D::~j1Engine3D()
{}


bool j1Engine3D::Awake()
{
	LOG("Loading Engine3D");

	return	true;
}


bool j1Engine3D::Start()
{

	return true;
}


bool j1Engine3D::PreUpdate()
{
	return true;
}


bool j1Engine3D::Update(float dt)
{

	return true;
}



bool j1Engine3D::CleanUp()
{
	LOG("Freeing Engine3D");

	return true;
}
