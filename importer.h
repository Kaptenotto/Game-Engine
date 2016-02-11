//#ifndef IMPORTER_H
//#define IMPORTER_H
//
//
//
//#include "OBJ_Structs.cpp"
//
//using namespace std;
//
//class Importer
//{
//public:
//	Importer();
//	void read();
//
//	string objFile, special;
//	string mtlFileName;
//	string nextLine;
//	char slashes;
//	//ifstream file;
//	istringstream inputString;
//	int index_counter = 0;
//	int counter = 0;
//	int objCounter = 0;
//
//	vector<VertexPos> vertices;
//	vector<TexCoord> uvs;
//	vector<NormDir> normals;
//	vector<Indexes> face_idxs;
//	vector<DiffColor> diffuses;
//	vector<AmbientColor> ambients;
//	vector<SpecularColor> speculars;
//	vector<TransparencyRGB> transes;
//	vector<string> groups;
//	vector<string> shadingGroups;
//	vector<string> mtlShadingGroups;
//	vector<string> textureMap;
//	vector<string> normalMap;
//
//	//Temporary variables
//private:
//	VertexPos tmp_vtx;
//	TexCoord tmp_tex;
//	NormDir tmp_norm;
//	Indexes idx;
//	string tmp_grp;
//	string tmp_shadeGroup;
//	string tmp_mtlShadeGroup;
//	DiffColor tmp_diff;
//	AmbientColor tmp_ambient;
//	SpecularColor tmp_specular;
//	TransparencyRGB tmp_trans;
//	string tmp_texMap;
//	string tmp_normMap;
//};
//
//
//
//#endif // IMPORTER_H
//
