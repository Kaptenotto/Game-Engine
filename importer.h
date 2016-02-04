#ifndef IMPORTER_H
#define IMPORTER_H


#include <DirectXMath.h>
#include "OBJ_Structs.cpp"

using namespace DirectX;
using namespace std;

class Importer
{
public:
	Importer();
	void read();

	string objFile, special;
	string mtlFileName;
	string nextLine;
	char slashes;
	//ifstream file;
	istringstream inputString;
	int index_counter = 0;
	int counter = 0;
	int objCounter = 0;

	//vector<XMFLOAT3> vertices;

	//TriangleVertex2;

	vector<XMFLOAT2> uvs;
	vector<XMFLOAT3> normals;
	vector<Indexes> face_idxs;
	vector<TriangleVertex2> vertices;
	vector<XMFLOAT3> diffuses;
	vector<XMFLOAT3> ambients;
	vector<XMFLOAT3> speculars;
	vector<XMFLOAT3> transes;
	vector<string> groups;
	vector<string> shadingGroups;
	vector<string> mtlShadingGroups;
	vector<string> textureMap;
	vector<string> normalMap;

	//Temporary variables
private:
	int temp_face_pos, temp_face_tex, temp_face_norm;
	TriangleVertex2 tmp_vert;
	XMFLOAT2 tmp_tex;
	XMFLOAT3 tmp_norm;
	Indexes idx;
	string tmp_grp;
	string tmp_shadeGroup;
	string tmp_mtlShadeGroup;
	XMFLOAT3 tmp_diff;
	XMFLOAT3 tmp_ambient;
	XMFLOAT3 tmp_specular;
	XMFLOAT3 tmp_trans;
	string tmp_texMap;
	string tmp_normMap;
};



#endif // IMPORTER_H

