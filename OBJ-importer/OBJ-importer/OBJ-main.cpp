#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;
int main()
{
	string objFile("testing.obj"), special;
	string nextLine;
	string slashes;
	ifstream file(objFile);
	istringstream inputString;

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
		vector<float> face_pos;
		vector<float> face_tex;
		vector<float> face_norm;
	};

	vector<VertexPos> vertices;
	vector<TexCoord> uvs;
	vector<NormDir> normals;
	vector<Indexes> face_idxs;
	
	VertexPos vtx;
	TexCoord tex;
	NormDir norm;
	Indexes idx;

	while (std::getline(file, nextLine))
	{
		inputString.clear();
		inputString.str(nextLine);
		
		if (nextLine.substr(0, 2) == "v ")
		{
			inputString >> special >> vtx.x >> vtx.y >> vtx.z;
			vertices.push_back(vtx);
			/*cout << special << "\n";
			cout << vtx.x << " ";
			cout << vtx.y << " ";
			cout << vtx.z << "\n";*/
		}

		else if (nextLine.substr(0, 3) == "vt ")
		{
			inputString >> special >> tex.u >> tex.v;
			uvs.push_back(tex);
		}
		else if (nextLine.substr(0, 3) == "vn ")
		{
			inputString >> special >> norm.nx >> norm.ny >> norm.nz;
			normals.push_back(norm);
		}
		else if (nextLine.substr(0, 2) == "f ")
		{
			inputString >> special >> idx.face_pos.push_back >> slashes >> idx.face_tex >> slashes >> idx.face_norm >> 
				idx.face_pos.push_back >> slashes >> idx.face_tex >> slashes >> idx.face_norm >> 
				idx.face_pos.push_back >> slashes >> idx.face_tex >> slashes >> idx.face_norm;
			
			face_idxs.push_back(idx);
		}
	}

	for (int i = 0; i < vertices.size(); i++)
	{
		cout << "x: " << vertices[i].x << " ";
		cout << "y: " << vertices[i].y << " ";
		cout << "z: " << vertices[i].z << "\n";
	}

	for (int i = 0; i < uvs.size(); i++)
	{
		cout << "u: " << uvs[i].u << " ";
		cout << "v: " << uvs[i].v << "\n";
	}

	for (int i = 0; i < normals.size(); i++)
	{
		cout << "nx: " << normals[i].nx << " ";
		cout << "ny: " << normals[i].ny << " ";
		cout << "nz: " << normals[i].nz << "\n";
	}

	cin.ignore();


	file.close();
}