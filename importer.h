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
	int vtCounter = 0;


	vector<AllInfo> finalVector;
	vector<VertexPos> vertices;
	vector<UVCoord> uvs;
	vector<Normals> normals;
	vector<Indexes> face_idxs;
	vector<Diffuse> diffuses;
	vector<Ambient> ambients;
	vector<Specular> speculars;
	vector<Trans> transes;
	vector<string> groups;
	vector<string> shadingGroups;
	vector<string> mtlShadingGroups;
	vector<string> textureMap;
	vector<string> normalMap;
	

	//Temporary variables
private:
	
	float temp_face_pos, temp_face_tex, temp_face_norm;
	VertexPos tmp_vtx;
	UVCoord tmp_tex;
	Normals tmp_norm;
	Indexes idx;
	string tmp_grp;
	string tmp_shadeGroup;
	string tmp_mtlShadeGroup;
	Diffuse tmp_diff;
	Ambient tmp_ambient;
	Specular tmp_specular;
	Trans tmp_trans;
	string tmp_texMap;
	string tmp_normMap;
	AllInfo tmp_fin;
};



#endif // IMPORTER_H

