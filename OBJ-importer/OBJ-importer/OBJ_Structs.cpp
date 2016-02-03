#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;
struct VertexPos
{
	float x, y, z;
};

struct TexCoord
{
	float u, v;
};

struct NormDir
{
	float nx, ny, nz;
};

struct Indexes
{
	vector<float> face_pos, face_tex, face_norm;
	float temp_face_pos, temp_face_tex, temp_face_norm;
};

struct DiffColor
{
	float r, g, b;
};

struct AmbientColor
{
	float r, g, b;
};
struct SpecularColor
{
	float r, g, b;
};
struct TransparencyRGB
{
	float r, g, b;
};

string objFile("normalCube.obj"), special;
string mtlFileName;
string nextLine;
char slashes;
ifstream file(objFile);
istringstream inputString;
int index_counter = 0;
int counter = 0;
int objCounter = 0;

vector<VertexPos> vertices;
vector<TexCoord> uvs;
vector<NormDir> normals;
vector<Indexes> face_idxs;
vector<DiffColor> diffuses;
vector<AmbientColor> ambients;
vector<SpecularColor> speculars;
vector<TransparencyRGB> transes;
vector<string> groups;
vector<string> shadingGroups;
vector<string> mtlShadingGroups;
vector<string> textureMap;
vector<string> normalMap;

VertexPos tmp_vtx;
TexCoord tmp_tex;
NormDir tmp_norm;
Indexes idx;
string tmp_grp;
string tmp_shadeGroup;
string tmp_mtlShadeGroup;
DiffColor tmp_diff;
AmbientColor tmp_ambient;
SpecularColor tmp_specular;
TransparencyRGB tmp_trans;
string tmp_texMap;
string tmp_normMap;