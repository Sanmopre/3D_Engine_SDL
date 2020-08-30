#ifndef __j1ENGINE3D_H__
#define __j1ENGINE3D_H__

#include "j1Module.h"

struct SDL_Texture;

class j1Engine3D : public j1Module
{
public:

	j1Engine3D();

	// Destructor
	virtual ~j1Engine3D();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

private:
};

#endif // __j1ENGINE3D_H__