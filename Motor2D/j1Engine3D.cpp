#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Engine3D.h"
#include <algorithm>

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

	mesh_cube.LoadFromObjectFile("map.obj");

	matProj = Matrix_MakeProjection(90.0f, (float)App->win->height / (float)App->win->width, 0.1f, 1000.0f);
	return true;
}


bool j1Engine3D::PreUpdate()
{ 
	return true;
}


bool j1Engine3D::Update(float dt)
{
	
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT) 
		Camera.y -= 1;

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		Camera.y += 1;

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		Camera.x -= 1;

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		Camera.x += 1;

	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
		resolution = 1;

	Vector3D vForward = Vector_Mul(LookDir, 2.0f);


	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		Camera = Vector_Add(Camera,vForward);

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		Camera = Vector_Sub(Camera, vForward);


	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		fYaw += 0.05f;

	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		fYaw -= 0.05f;

	if (App->input->GetKey(SDL_SCANCODE_C) == KEY_REPEAT)
		fXaw += 0.05f;

	if (App->input->GetKey(SDL_SCANCODE_X) == KEY_REPEAT)
		fXaw -= 0.05f;



	Matrix4x4 matRotZ, matRotX;
	//fTheta += 0.02f;

	matRotZ = Matrix_MakeRotationZ(fTheta * 0.5f);
	matRotX = Matrix_MakeRotationX(fTheta);

	Matrix4x4 matTrans;
	matTrans = Matrix_MakeTranslation(0.0f, 0.0f, 20.0f);

	Matrix4x4 matWorld;
	matWorld = Matrix_MakeIdentity();
	matWorld = Matrix_MultiplyMatrix(matRotZ, matRotX); 
	matWorld = Matrix_MultiplyMatrix(matWorld, matTrans); 


	Vector3D vUp = { 0,1,0 };
	Vector3D vTarget = {0,0,1};

	Matrix4x4 matCameraRot = Matrix_MakeRotationY(fYaw);
	LookDir = MultiplyMatrixVector(matCameraRot, vTarget);
	vTarget = Vector_Add(Camera, LookDir);


	/*
	Matrix4x4 matCameraRot_x = Matrix_MakeRotationX(fXaw);
	LookDir = MultiplyMatrixVector(matCameraRot_x, vTarget);
	vTarget = Vector_Add(Camera, LookDir);
	*/
	Matrix4x4 matCamera = Matrix_PointAt(Camera, vTarget, vUp);
	Matrix4x4 matView = Matrix_QuickInverse(matCamera);

	vector<Triangle_s> TrianglesToDraw;
	vector<float> ShaderValue;
	for (auto tri : mesh_cube.tris)
	{
		Triangle_s triProjected, triTransformed, triViewed;

		// World Matrix Transform
		triTransformed.vertices[0] = MultiplyMatrixVector(matWorld, tri.vertices[0]);
		triTransformed.vertices[1] = MultiplyMatrixVector(matWorld, tri.vertices[1]);
		triTransformed.vertices[2] = MultiplyMatrixVector(matWorld, tri.vertices[2]);


		// Calculate triangle Normal
		Vector3D normal, line1, line2;

		// Get lines either side of triangle
		line1 = Vector_Sub(triTransformed.vertices[1], triTransformed.vertices[0]);
		line2 = Vector_Sub(triTransformed.vertices[2], triTransformed.vertices[0]);

		// Take cross product of lines to get normal to triangle surface
		normal = Vector_CrossProduct(line1, line2);

		float l = sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
		normal.x /= l;
		normal.y /= l;
		normal.z /= l;

		// Get Ray from triangle to camera
		Vector3D vCameraRay = Vector_Sub(triTransformed.vertices[0], Camera);

		// If ray is aligned with normal, then triangle is visible
		if (Vector_DotProduct(normal, vCameraRay) < 0.0f)
		{

			Vector3D light_direction = { 0.0f, 1.0f, 1.0f };
			light_direction = Vector_Normalise(light_direction);

			float dp = max(0.1f, Vector_DotProduct(light_direction, normal));

			triViewed.vertices[0] = MultiplyMatrixVector(matView, triTransformed.vertices[0]);
			triViewed.vertices[1] = MultiplyMatrixVector(matView, triTransformed.vertices[1]);
			triViewed.vertices[2] = MultiplyMatrixVector(matView, triTransformed.vertices[2]);


			int nClippedTriangles = 0;
			Triangle_s clipped[2];
			nClippedTriangles = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.1f }, { 0.0f, 0.0f, 1.0f }, triViewed, clipped[0], clipped[1]);

			for (int n = 0; n < nClippedTriangles; n++)
			{
				triProjected.vertices[0] = MultiplyMatrixVector(matProj, clipped[n].vertices[0]);
				triProjected.vertices[1] = MultiplyMatrixVector(matProj, clipped[n].vertices[1]);
				triProjected.vertices[2] = MultiplyMatrixVector(matProj, clipped[n].vertices[2]);

				triProjected.vertices[0] = Vector_Div(triProjected.vertices[0], triProjected.vertices[0].w);
				triProjected.vertices[1] = Vector_Div(triProjected.vertices[1], triProjected.vertices[1].w);
				triProjected.vertices[2] = Vector_Div(triProjected.vertices[2], triProjected.vertices[2].w);


				Vector3D vOffsetView = { 1,1,0 };

				triProjected.vertices[0] = Vector_Add(triProjected.vertices[0], vOffsetView);
				triProjected.vertices[1] = Vector_Add(triProjected.vertices[1], vOffsetView);
				triProjected.vertices[2] = Vector_Add(triProjected.vertices[2], vOffsetView);

				triProjected.vertices[0].x *= 0.5f * (float)App->win->width;
				triProjected.vertices[0].y *= 0.5f * (float)App->win->height;

				triProjected.vertices[1].x *= 0.5f * (float)App->win->width;
				triProjected.vertices[1].y *= 0.5f * (float)App->win->height;

				triProjected.vertices[2].x *= 0.5f * (float)App->win->width;
				triProjected.vertices[2].y *= 0.5f * (float)App->win->height;

				triProjected.shader_value = dp;
				TrianglesToDraw.push_back(triProjected);
			}
		//App->render->DrawTriangle(triProjected.vertices[0].x, triProjected.vertices[0].y, triProjected.vertices[1].x, triProjected.vertices[1].y, triProjected.vertices[2].x, triProjected.vertices[2].y);
		//App->render->DrawFilledTriangle(triProjected.vertices[0].x, triProjected.vertices[0].y, triProjected.vertices[1].x, triProjected.vertices[1].y, triProjected.vertices[2].x, triProjected.vertices[2].y, dp);
		
		//App->render->DrawTriangle(30, 10, 80, 100, 120, 10);
		//App->render->DrawTriangleLowRes(resolution, 30, 10, 80, 100, 120, 10,1);
		//App->render->DrawFilledTriangle(30, 10, 80, 100, 120, 10,1);
		}
	}

	
	sort(TrianglesToDraw.begin(), TrianglesToDraw.end(), [](Triangle_s& t1, Triangle_s& t2) {

		float z1 = (t1.vertices[0].z + t1.vertices[1].z + t1.vertices[2].z) / 3.0f;
		float z2 = (t2.vertices[0].z + t2.vertices[1].z + t2.vertices[2].z) / 3.0f;

		return z1 > z2;
		});


	for (auto& triToRaster: TrianglesToDraw) {

		Triangle_s clipped[2];
		list<Triangle_s> listTriangles;
		listTriangles.push_back(triToRaster);
		int nNewTriangles = 1;

		for (int p = 0; p < 4; p++)
		{
			int nTrisToAdd = 0;
			while (nNewTriangles > 0)
			{
				Triangle_s test = listTriangles.front();
				listTriangles.pop_front();
				nNewTriangles--;

				switch (p)
				{
				case 0:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
				case 1:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, (float)App->win->height - 1, 0.0f }, { 0.0f, -1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
				case 2:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
				case 3:	nTrisToAdd = Triangle_ClipAgainstPlane({ (float)App->win->width - 1, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
				}

				for (int w = 0; w < nTrisToAdd; w++)
					listTriangles.push_back(clipped[w]);
			}
			nNewTriangles = listTriangles.size();
		}

		for (auto& t : listTriangles) {
			//App->render->DrawTriangle(t.vertices[0].x, t.vertices[0].y, t.vertices[1].x, t.vertices[1].y, t.vertices[2].x, t.vertices[2].y);
			App->render->DrawTriangleLowRes(resolution, t.vertices[0].x, t.vertices[0].y, t.vertices[1].x, t.vertices[1].y, t.vertices[2].x, t.vertices[2].y, t.shader_value);
		}
		}

	return true;
}



bool j1Engine3D::CleanUp()
{
	LOG("Freeing Engine3D");

	return true;
}

Vector3D j1Engine3D::MultiplyMatrixVector(Matrix4x4& m, Vector3D& i)
{
	Vector3D v;
	v.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + i.w * m.m[3][0];
	v.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + i.w * m.m[3][1];
	v.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + i.w * m.m[3][2];
	v.w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + i.w * m.m[3][3];
	return v;
}


Matrix4x4 j1Engine3D::Matrix_MakeIdentity()
{
	Matrix4x4 matrix;
	matrix.m[0][0] = 1.0f;
	matrix.m[1][1] = 1.0f;
	matrix.m[2][2] = 1.0f;
	matrix.m[3][3] = 1.0f;
	return matrix;
}

Matrix4x4 j1Engine3D::Matrix_MakeRotationX(float fAngleRad)
{
	Matrix4x4 matrix;
	matrix.m[0][0] = 1.0f;
	matrix.m[1][1] = cosf(fAngleRad);
	matrix.m[1][2] = sinf(fAngleRad);
	matrix.m[2][1] = -sinf(fAngleRad);
	matrix.m[2][2] = cosf(fAngleRad);
	matrix.m[3][3] = 1.0f;
	return matrix;
}

Matrix4x4 j1Engine3D::Matrix_MakeRotationY(float fAngleRad)
{
	Matrix4x4 matrix;
	matrix.m[0][0] = cosf(fAngleRad);
	matrix.m[0][2] = sinf(fAngleRad);
	matrix.m[2][0] = -sinf(fAngleRad);
	matrix.m[1][1] = 1.0f;
	matrix.m[2][2] = cosf(fAngleRad);
	matrix.m[3][3] = 1.0f;
	return matrix;
}

Matrix4x4 j1Engine3D::Matrix_MakeRotationZ(float fAngleRad)
{
	Matrix4x4 matrix;
	matrix.m[0][0] = cosf(fAngleRad);
	matrix.m[0][1] = sinf(fAngleRad);
	matrix.m[1][0] = -sinf(fAngleRad);
	matrix.m[1][1] = cosf(fAngleRad);
	matrix.m[2][2] = 1.0f;
	matrix.m[3][3] = 1.0f;
	return matrix;
}

Matrix4x4 j1Engine3D::Matrix_MakeTranslation(float x, float y, float z)
{
	Matrix4x4 matrix;
	matrix.m[0][0] = 1.0f;
	matrix.m[1][1] = 1.0f;
	matrix.m[2][2] = 1.0f;
	matrix.m[3][3] = 1.0f;
	matrix.m[3][0] = x;
	matrix.m[3][1] = y;
	matrix.m[3][2] = z;
	return matrix;
}

Matrix4x4 j1Engine3D::Matrix_MakeProjection(float fFovDegrees, float fAspectRatio, float fNear, float fFar)
{
	float fFovRad = 1.0f / tanf(fFovDegrees * 0.5f / 180.0f * 3.14159f);
	Matrix4x4 matrix;
	matrix.m[0][0] = fAspectRatio * fFovRad;
	matrix.m[1][1] = fFovRad;
	matrix.m[2][2] = fFar / (fFar - fNear);
	matrix.m[3][2] = (-fFar * fNear) / (fFar - fNear);
	matrix.m[2][3] = 1.0f;
	matrix.m[3][3] = 0.0f;
	return matrix;
}

Matrix4x4 j1Engine3D::Matrix_MultiplyMatrix(Matrix4x4& m1, Matrix4x4& m2)
{
	Matrix4x4 matrix;
	for (int c = 0; c < 4; c++)
		for (int r = 0; r < 4; r++)
			matrix.m[r][c] = m1.m[r][0] * m2.m[0][c] + m1.m[r][1] * m2.m[1][c] + m1.m[r][2] * m2.m[2][c] + m1.m[r][3] * m2.m[3][c];
	return matrix;
}

Matrix4x4 j1Engine3D::Matrix_PointAt(Vector3D& pos, Vector3D& target, Vector3D& up)
{
	// Calculate new forward direction
	Vector3D newForward = Vector_Sub(target, pos);
	newForward = Vector_Normalise(newForward);

	// Calculate new Up direction
	Vector3D a = Vector_Mul(newForward, Vector_DotProduct(up, newForward));
	Vector3D newUp = Vector_Sub(up, a);
	newUp = Vector_Normalise(newUp);

	// New Right direction is easy, its just cross product
	Vector3D newRight = Vector_CrossProduct(newUp, newForward);

	// Construct Dimensioning and Translation Matrix	
	Matrix4x4 matrix;
	matrix.m[0][0] = newRight.x;	matrix.m[0][1] = newRight.y;	matrix.m[0][2] = newRight.z;	matrix.m[0][3] = 0.0f;
	matrix.m[1][0] = newUp.x;		matrix.m[1][1] = newUp.y;		matrix.m[1][2] = newUp.z;		matrix.m[1][3] = 0.0f;
	matrix.m[2][0] = newForward.x;	matrix.m[2][1] = newForward.y;	matrix.m[2][2] = newForward.z;	matrix.m[2][3] = 0.0f;
	matrix.m[3][0] = pos.x;			matrix.m[3][1] = pos.y;			matrix.m[3][2] = pos.z;			matrix.m[3][3] = 1.0f;
	return matrix;

}

Matrix4x4 j1Engine3D::Matrix_QuickInverse(Matrix4x4& m)
{
	Matrix4x4 matrix;
	matrix.m[0][0] = m.m[0][0]; matrix.m[0][1] = m.m[1][0]; matrix.m[0][2] = m.m[2][0]; matrix.m[0][3] = 0.0f;
	matrix.m[1][0] = m.m[0][1]; matrix.m[1][1] = m.m[1][1]; matrix.m[1][2] = m.m[2][1]; matrix.m[1][3] = 0.0f;
	matrix.m[2][0] = m.m[0][2]; matrix.m[2][1] = m.m[1][2]; matrix.m[2][2] = m.m[2][2]; matrix.m[2][3] = 0.0f;
	matrix.m[3][0] = -(m.m[3][0] * matrix.m[0][0] + m.m[3][1] * matrix.m[1][0] + m.m[3][2] * matrix.m[2][0]);
	matrix.m[3][1] = -(m.m[3][0] * matrix.m[0][1] + m.m[3][1] * matrix.m[1][1] + m.m[3][2] * matrix.m[2][1]);
	matrix.m[3][2] = -(m.m[3][0] * matrix.m[0][2] + m.m[3][1] * matrix.m[1][2] + m.m[3][2] * matrix.m[2][2]);
	matrix.m[3][3] = 1.0f;
	return matrix;
}




Vector3D j1Engine3D::Vector_Add(Vector3D& v1, Vector3D& v2)
{
	return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}

Vector3D j1Engine3D::Vector_Sub(Vector3D& v1, Vector3D& v2)
{
	return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
}

Vector3D j1Engine3D::Vector_Mul(Vector3D& v1, float k)
{
	return { v1.x * k, v1.y * k, v1.z * k };
}

Vector3D j1Engine3D::Vector_Div(Vector3D& v1, float k)
{
	return { v1.x / k, v1.y / k, v1.z / k };
}

float j1Engine3D::Vector_DotProduct(Vector3D& v1, Vector3D& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

float j1Engine3D::Vector_Length(Vector3D& v)
{
	return sqrtf(Vector_DotProduct(v, v));
}

Vector3D j1Engine3D::Vector_Normalise(Vector3D& v)
{
	float l = Vector_Length(v);
	return { v.x / l, v.y / l, v.z / l };
}

Vector3D j1Engine3D::Vector_CrossProduct(Vector3D& v1, Vector3D& v2)
{
	Vector3D v;
	v.x = v1.y * v2.z - v1.z * v2.y;
	v.y = v1.z * v2.x - v1.x * v2.z;
	v.z = v1.x * v2.y - v1.y * v2.x;
	return v;
}

Vector3D j1Engine3D::Vector_IntersectPlane(Vector3D& plane_p, Vector3D& plane_n, Vector3D& lineStart, Vector3D& lineEnd)
{
	plane_n = Vector_Normalise(plane_n);
	float plane_d = -Vector_DotProduct(plane_n, plane_p);
	float ad = Vector_DotProduct(lineStart, plane_n);
	float bd = Vector_DotProduct(lineEnd, plane_n);
	float t = (-plane_d - ad) / (bd - ad);
	Vector3D lineStartToEnd = Vector_Sub(lineEnd, lineStart);
	Vector3D lineToIntersect = Vector_Mul(lineStartToEnd, t);
	return Vector_Add(lineStart, lineToIntersect);
}


int j1Engine3D::Triangle_ClipAgainstPlane(Vector3D plane_p, Vector3D plane_n, Triangle_s& in_tri, Triangle_s& out_tri1, Triangle_s& out_tri2)
{
	plane_n = Vector_Normalise(plane_n);

	auto dist = [&](Vector3D& p)
	{
		Vector3D n = Vector_Normalise(p);
		return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - Vector_DotProduct(plane_n, plane_p));
	};
	
	Vector3D* inside_points[3];  int nInsidePointCount = 0;
	Vector3D* outside_points[3]; int nOutsidePointCount = 0;


	float d0 = dist(in_tri.vertices[0]);
	float d1 = dist(in_tri.vertices[1]);
	float d2 = dist(in_tri.vertices[2]);

	if (d0 >= 0) { inside_points[nInsidePointCount++] = &in_tri.vertices[0]; }
	else { outside_points[nOutsidePointCount++] = &in_tri.vertices[0]; }
	if (d1 >= 0) { inside_points[nInsidePointCount++] = &in_tri.vertices[1]; }
	else { outside_points[nOutsidePointCount++] = &in_tri.vertices[1]; }
	if (d2 >= 0) { inside_points[nInsidePointCount++] = &in_tri.vertices[2]; }
	else { outside_points[nOutsidePointCount++] = &in_tri.vertices[2]; }


	if (nInsidePointCount == 0)
	{

		return 0; 
	}

	if (nInsidePointCount == 3)
	{

		out_tri1 = in_tri;

		return 1; 
	}

	if (nInsidePointCount == 1 && nOutsidePointCount == 2)
	{

		out_tri1.shader_value = in_tri.shader_value;


		out_tri1.vertices[0] = *inside_points[0];

		out_tri1.vertices[1] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);
		out_tri1.vertices[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[1]);

		return 1; 
	}

	if (nInsidePointCount == 2 && nOutsidePointCount == 1)
	{



		out_tri1.shader_value = in_tri.shader_value;
		out_tri2.shader_value = in_tri.shader_value;



		out_tri1.vertices[0] = *inside_points[0];
		out_tri1.vertices[1] = *inside_points[1];
		out_tri1.vertices[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);


		out_tri2.vertices[0] = *inside_points[1];
		out_tri2.vertices[1] = out_tri1.vertices[2];
		out_tri2.vertices[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[1], *outside_points[0]);

		return 2;
	}
}   