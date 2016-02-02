#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;
int main()
{
	string objFile("cube.obj"), special;
	string nextLine;
	char slashes;
	ifstream file(objFile);
	istringstream inputString;
	int index_counter = 0;
	int counter = 0;

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
			// Reading Vertex Positions from the file
			// and adding them to a temporary variable
			inputString >> special >> vtx.x >> vtx.y >> vtx.z;

			// Adding the vertex positions to a vector
			vertices.push_back(vtx);

			/*cout << special << "\n";
			cout << vtx.x << " ";
			cout << vtx.y << " ";
			cout << vtx.z << "\n";*/
		}

		else if (nextLine.substr(0, 3) == "vt ")
		{
			//Reading UV-Values
			inputString >> special >> tex.u >> tex.v;
			//Adding UV-Values to a Vector
			uvs.push_back(tex);
		}
		else if (nextLine.substr(0, 3) == "vn ")
		{
			//Reading normals
			inputString >> special >> norm.nx >> norm.ny >> norm.nz;

			//Adding Normals to a Vector
			normals.push_back(norm);
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
	}


	cin.ignore();


	file.close();
}