
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <DirectXMath.h>


using namespace std;

struct Indexes
{
	int face_pos, face_tex, face_norm;
	
};
struct Temp_vert
{
	float x, y, z;
};
struct TriangleVertex2
{
	float x, y, z;
	float u, v;
};
