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
	mesh_cube.tris = {
		// SOUTH
			{ 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
			{ 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

			// EAST                                                      
			{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
			{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

			// NORTH                                                     
			{ 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
			{ 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

			// WEST                                                      
			{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

			// TOP                                                       
			{ 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
			{ 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

			// BOTTOM                                                    
			{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
			{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },
	};


	matProj.m[0][0] = fAspectRatio * fFovRad;
	matProj.m[1][1] = fFovRad;
	matProj.m[2][2] = fFar / (fFar - fNear);
	matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
	matProj.m[2][3] = 1.0f;
	matProj.m[3][3] = 0.0f;


	return true;
}


bool j1Engine3D::PreUpdate()
{
	return true;
}


bool j1Engine3D::Update(float dt)
{
	
	Matrix4x4 matRotZ, matRotX;
	fTheta += 0.0004f;

	//ROT Z
	matRotZ.m[0][0] = cosf(fTheta);
	matRotZ.m[0][1] = sinf(fTheta);
	matRotZ.m[1][0] = -sinf(fTheta);
	matRotZ.m[1][1] = cosf(fTheta);
	matRotZ.m[2][2] = 1;
	matRotZ.m[3][3] = 1;

	//ROT X
	matRotX.m[0][0] = 1;
	matRotX.m[1][1] = cosf(fTheta * 0.5f);
	matRotX.m[1][2] = sinf(fTheta * 0.5f);
	matRotX.m[2][1] = -sinf(fTheta * 0.5f);
	matRotX.m[2][2] = cosf(fTheta * 0.5f);
	matRotX.m[3][3] = 1;

	for (auto tri : mesh_cube.tris) 
	{
		Triangle_s triProjected, triTranslated, triRotatedZ, triRotatedZX;

		MultiplyMatrixVector(tri.vertices[0], triRotatedZ.vertices[0], matRotZ);
		MultiplyMatrixVector(tri.vertices[1], triRotatedZ.vertices[1], matRotZ);
		MultiplyMatrixVector(tri.vertices[2], triRotatedZ.vertices[2], matRotZ);

		MultiplyMatrixVector(triRotatedZ.vertices[0], triRotatedZX.vertices[0], matRotX);
		MultiplyMatrixVector(triRotatedZ.vertices[1], triRotatedZX.vertices[1], matRotX);
		MultiplyMatrixVector(triRotatedZ.vertices[2], triRotatedZX.vertices[2], matRotX);

		triTranslated = triRotatedZX;
		triTranslated.vertices[0].z = triRotatedZX.vertices[0].z + 3.0f;
		triTranslated.vertices[1].z = triRotatedZX.vertices[1].z + 3.0f;
		triTranslated.vertices[2].z = triRotatedZX.vertices[2].z + 3.0f;


		MultiplyMatrixVector(triTranslated.vertices[0], triProjected.vertices[0], matProj);
		MultiplyMatrixVector(triTranslated.vertices[1], triProjected.vertices[1], matProj);
		MultiplyMatrixVector(triTranslated.vertices[2], triProjected.vertices[2], matProj);





		triProjected.vertices[0].x += 1.0f;
		triProjected.vertices[0].y += 1.0f;

		triProjected.vertices[1].x += 1.0f;
		triProjected.vertices[1].y += 1.0f;

		triProjected.vertices[2].x += 1.0f;
		triProjected.vertices[2].y += 1.0f;





		triProjected.vertices[0].x *= 0.5f * (float)App->win->width;
		triProjected.vertices[0].y *= 0.5f * (float)App->win->height;

		triProjected.vertices[1].x *= 0.5f * (float)App->win->width;
		triProjected.vertices[1].y *= 0.5f * (float)App->win->height;

		triProjected.vertices[2].x *= 0.5f * (float)App->win->width;
		triProjected.vertices[2].y *= 0.5f * (float)App->win->height;



		App->render->DrawTriangle(triProjected.vertices[0].x, triProjected.vertices[0].y, triProjected.vertices[1].x, triProjected.vertices[1].y, triProjected.vertices[2].x, triProjected.vertices[2].y);

	}


	return true;
}



bool j1Engine3D::CleanUp()
{
	LOG("Freeing Engine3D");

	return true;
}

void j1Engine3D::MultiplyMatrixVector(Vector3D& i, Vector3D& o, Matrix4x4& m)
{
	o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
	o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
	o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
	float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

	if (w != 0.0f)
	{
		o.x /= w; o.y /= w; o.z /= w;
	}
}
