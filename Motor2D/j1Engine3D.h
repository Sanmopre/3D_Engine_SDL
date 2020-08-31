#ifndef __j1ENGINE3D_H__
#define __j1ENGINE3D_H__

#include "j1Module.h"
#include <vector>
#include "p2DynArray.h"
using namespace std;



struct Vector3D {
	float x, y, z;
};

struct Triangle_s {
	Vector3D vertices[3];
};

struct Mesh {
	vector<Triangle_s> tris;
};

struct Matrix4x4 {
	float m[4][4] = { 0 };
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

	void MultiplyMatrixVector(Vector3D& i, Vector3D& o, Matrix4x4& m);

	float fTheta = 0;

	Matrix4x4 matProj;

	//Projection matrix variables
	float fNear = 0.1f;
	float fFar = 1000.0f;
	float fFov = 90.0f;
	float fAspectRatio = 1.0f;//screen height / screen width
	float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

public:
	Mesh mesh_cube;

	Vector3D Camera;

	Triangle_s tri;

};

#endif // __j1ENGINE3D_H__