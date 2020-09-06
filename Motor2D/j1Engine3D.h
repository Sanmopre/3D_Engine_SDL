#ifndef __j1ENGINE3D_H__
#define __j1ENGINE3D_H__

#include "j1Module.h"
#include <vector>
#include <fstream>
#include <strstream>
#include "p2DynArray.h"
using namespace std;



struct Vector3D {
	float x = 0;
	float y = 0;
	float z = 0;
	float w = 1;
};

struct Triangle_s {
	Vector3D vertices[3];
	float shader_value;
};

struct Mesh {
	vector<Triangle_s> tris;

	bool LoadFromObjectFile(string sFilename) 
	{
		ifstream f(sFilename);
		if (!f.is_open())
			return false;

		vector<Vector3D> verts;
		
		while (!f.eof())
		{
			char line[128];
			f.getline(line, 128);

			strstream s;
			s << line;

			char letter;

			if (line[0] == 'v') {
				Vector3D v;
				s >> letter >> v.x >> v.y >> v.z;
				verts.push_back(v);
			}

			if (line[0] == 'f') {
				int f[3];
				s >> letter >> f[0] >> f[1] >> f[2];
				tris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
			}

		}

		return true;
	}

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

	Vector3D MultiplyMatrixVector(Matrix4x4& m, Vector3D& i);
	Matrix4x4 Matrix_MakeIdentity();
	Matrix4x4 Matrix_MakeRotationX(float fAngleRad);
	Matrix4x4 Matrix_MakeRotationY(float fAngleRad);
	Matrix4x4 Matrix_MakeRotationZ(float fAngleRad);
	Matrix4x4 Matrix_MakeTranslation(float x, float y, float z);
	Matrix4x4 Matrix_MakeProjection(float fFovDegrees, float fAspectRatio, float fNear, float fFar);
	Matrix4x4 Matrix_MultiplyMatrix(Matrix4x4& m1, Matrix4x4& m2);
	Matrix4x4 Matrix_PointAt(Vector3D& pos, Vector3D& target, Vector3D& up);
	Matrix4x4 Matrix_QuickInverse(Matrix4x4& m);
	Vector3D Vector_Add(Vector3D& v1, Vector3D& v2);
	Vector3D Vector_Sub(Vector3D& v1, Vector3D& v2);
	Vector3D Vector_Mul(Vector3D& v1, float k);
	Vector3D Vector_Div(Vector3D& v1, float k);
	float Vector_DotProduct(Vector3D& v1, Vector3D& v2);
	float Vector_Length(Vector3D& v);
	Vector3D Vector_Normalise(Vector3D& v);
	Vector3D Vector_CrossProduct(Vector3D& v1, Vector3D& v2);
	Vector3D Vector_IntersectPlane(Vector3D& plane_p, Vector3D& plane_n, Vector3D& lineStart, Vector3D& lineEnd);

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
	Vector3D LookDir;

	Triangle_s tri;

	int resolution = 2;

};

#endif // __j1ENGINE3D_H__