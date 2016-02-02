#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;
int main()
{
	string objFile("cube.obj"), special;
	string mtlFileName;
	string nextLine;
	char slashes;
	ifstream file(objFile);
	istringstream inputString;
	int index_counter = 0;
	int counter = 0;
	int objCounter = 0;
	string tmp_grp;
	string tmp_shadeGroup;
	string tmp_mtlShadeGroup;

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

	struct DiffCoord
	{
		float r, g, b;
	};

	vector<VertexPos> vertices;
	vector<TexCoord> uvs;
	vector<NormDir> normals;
	vector<Indexes> face_idxs;
	vector<DiffCoord> diffuse;
	vector<string> groups;
	vector<string> shadingGroups;
	vector<string> mtlShadingGroups;
	
	VertexPos tmp_vtx;
	TexCoord tmp_tex;
	NormDir tmp_norm;
	Indexes idx;
	DiffCoord tmp_diff;

	while (std::getline(file, nextLine))
	{
		inputString.clear();
		inputString.str(nextLine);
		
		if (nextLine.substr(0, 2) == "v ")
		{
			// Reading Vertex Positions from the file
			// and adding them to a temporary variable
			inputString >> special >> tmp_vtx.x >> tmp_vtx.y >> tmp_vtx.z;

			// Adding the vertex positions to a vector
			vertices.push_back(tmp_vtx);

			/*cout << special << "\n";
			cout << vtx.x << " ";
			cout << vtx.y << " ";
			cout << vtx.z << "\n";*/
		}

		else if (nextLine.substr(0, 3) == "vt ")
		{
			//Reading UV-Values
			inputString >> special >> tmp_tex.u >> tmp_tex.v;
			//Adding UV-Values to a Vector
			uvs.push_back(tmp_tex);
		}
		else if (nextLine.substr(0, 3) == "vn ")
		{
			//Reading normals
			inputString >> special >> tmp_norm.nx >> tmp_norm.ny >> tmp_norm.nz;

			//Adding Normals to a Vector
			normals.push_back(tmp_norm);
		}
		else if (nextLine.substr(0, 2) == "f ")
		{
			//Reading the first Face-Index (*i/j/k* i/j/k i/j/k)
			inputString >> special >> idx.temp_face_pos >> slashes >> idx.temp_face_tex >> slashes >> idx.temp_face_norm;
			//cout << idx.temp_face_pos << "/" << idx.temp_face_tex <<  "/" << idx.temp_face_norm << " ";

			//Clearing the temporairy Vectors they are put in
			idx.face_pos.clear();
			idx.face_tex.clear();
			idx.face_norm.clear();

			//Adding the values to their temporairy Vectors
			idx.face_pos.push_back(idx.temp_face_pos);
			idx.face_tex.push_back(idx.temp_face_tex);
			idx.face_norm.push_back(idx.temp_face_norm);

			//Pushing the temporairy vectors to the final vector
			face_idxs.push_back(idx);

			//How many indexes there are
			index_counter++;

			

			//Reading the second Face-Index (i/j/k *i/j/k* i/j/k)
			inputString >> idx.temp_face_pos >> slashes >> idx.temp_face_tex >> slashes >> idx.temp_face_norm;
			//cout << idx.temp_face_pos << "/" << idx.temp_face_tex << "/" << idx.temp_face_norm << " ";

			//Clearing the temporairy Vectors they are put in
			idx.face_pos.clear();
			idx.face_tex.clear();
			idx.face_norm.clear();

			//Adding the values to their temporairy Vectors
			idx.face_pos.push_back(idx.temp_face_pos);
			idx.face_tex.push_back(idx.temp_face_tex);
			idx.face_norm.push_back(idx.temp_face_norm);

			//Pushing the temporairy vectors to the final vector
			face_idxs.push_back(idx);

			//How many indexes there are
			index_counter++;


			
			//Reading the third Face-Index (i/j/k i/j/k *i/j/k*)
			inputString >> idx.temp_face_pos >> slashes >> idx.temp_face_tex >> slashes >> idx.temp_face_norm;
			//cout << idx.temp_face_pos << "/" << idx.temp_face_tex << "/" << idx.temp_face_norm << "\n";

			//Clearing the temporairy Vectors they are put in
			idx.face_pos.clear();
			idx.face_tex.clear();
			idx.face_norm.clear();


			//Adding the values to their temporairy Vectors
			idx.face_pos.push_back(idx.temp_face_pos);
			idx.face_tex.push_back(idx.temp_face_tex);
			idx.face_norm.push_back(idx.temp_face_norm);

			//Pushing the temporairy vectors to the final vector
			face_idxs.push_back(idx);

			//How many indexes there are
			index_counter++;


		}
		else if (nextLine.substr(0, 2) == "g ")
		{
			inputString >> special >> tmp_grp;

			// If the group we found are the default group, skip it!
			if (tmp_grp != "default")
			{
				groups.push_back(tmp_grp);
				objCounter++;
				//cout << tmp_grp << "\n";
			}
			
			
		}
		else if (nextLine.substr(0, 7) == "mtllib ")
		{
			//Eftersom det bara finns en mtl fil per ".obj" så
			//räcker det med att läsa in ett filnamn.
			inputString >> special >> mtlFileName;
		}
		else if(nextLine.substr(0,7) == "usemtl ")
		{
			//Reading shader group
			inputString >> special >> tmp_shadeGroup;

			//pushing the found shader group to a Vector
			shadingGroups.push_back(tmp_shadeGroup);
			//cout << tmp_shadeGroup;
		}

	}

	ifstream mtlFile(mtlFileName);
	while (std::getline(mtlFile, nextLine))
	{
		inputString.clear();
		inputString.str(nextLine);

		if (nextLine.substr(0, 7) == "newmtl ")
		{
			inputString >> special >> tmp_mtlShadeGroup;

			mtlShadingGroups.push_back(tmp_mtlShadeGroup);
		}
		else if (nextLine.substr(0, 3) == "Kd ")
		{
			inputString >> special >> tmp_diff.r >> tmp_diff.g >> tmp_diff.b;

			//cout << tmp_diff.r << " ###### " << tmp_diff.g << " ###### " << tmp_diff.b << " ###### ";

			diffuse.push_back(tmp_diff);
		}

	}

	/*for (int i = 0; i < vertices.size(); i++)
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
		cout << "nX: " << normals[i].nx << " ";
		cout << "nY: " << normals[i].ny << " ";
		cout << "nZ: " << normals[i].nz << "\n";
	}



	for (int i = 0; i < face_idxs.size(); i++)
	{
		for (int j = 0; j < face_idxs[i].face_pos.size(); j++)
		{
			cout << face_idxs[i].face_pos[j] << "/" << face_idxs[i].face_tex[j] << "/" << face_idxs[i].face_norm[j] << " ";
			counter++;
			if (counter == 3)
			{
				cout << "\n";
				counter = 0;
			}
		}
	}*/


	cin.ignore();


	file.close();
}