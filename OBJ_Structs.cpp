
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <DirectXMath.h>


using namespace std;

struct Indexes
{
	float face_pos,
		  face_tex,
		  face_norm;
};

struct AllInfo 
{
	float x, y, z, 
		  u, v, 
		  nx, ny, nz;
};

struct VertexPos
{
	float x, y, z;
};
struct UVCoord
{
	float u, v;
};
struct Normals
{
	float x, y, z;
};

struct Diffuse
{
	float x, y, z;
};
struct Ambient
{
	float x, y, z;
};
struct Specular
{
	float x, y, z;
};
struct Trans
{
	float x, y, z;
};