#ifndef __j1ENGINE3D_H__
#define __j1ENGINE3D_H__

#include "j1Module.h"
#include <vector>
#include "p2DynArray.h"
using namespace std;



struct Vector3D {
	float x, y, z;
};

struct Triangle {
	Vector3D vertices[3];
};

struct Mesh {
	vector<Triangle> tris;
};



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

public:
	Mesh mesh_cube;
};

#endif // __j1ENGINE3D_H__