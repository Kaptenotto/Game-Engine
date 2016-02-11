
#include "importer.h"


using namespace std;
Importer::Importer()
{
	objFile = ("./objFiles/normalCube.obj");

	index_counter = 0;
	objCounter = 0;
	//string mtlFileName;
	//string nextLine;
	//char slashes;
	//ifstream file(objFile);
	//istringstream inputString;
	

	//vector<VertexPos> vertices;
	//vector<TexCoord> uvs;
	//vector<NormDir> normals;
	//vector<Indexes> face_idxs;
	//vector<DiffColor> diffuses;
	//vector<AmbientColor> ambients;
	//vector<SpecularColor> speculars;
	//vector<TransparencyRGB> transes;
	//vector<string> groups;
	//vector<string> shadingGroups;
	//vector<string> mtlShadingGroups;
	//vector<string> textureMap;
	//vector<string> normalMap;
	//
	////Temporary variables

	//VertexPos tmp_vtx;
	//TexCoord tmp_tex;
	//NormDir tmp_norm;
	//Indexes idx;
	//string tmp_grp;
	//string tmp_shadeGroup;
	//string tmp_mtlShadeGroup;
	//DiffColor tmp_diff;
	//AmbientColor tmp_ambient;
	//SpecularColor tmp_specular;
	//TransparencyRGB tmp_trans;
	//string tmp_texMap;
	//string tmp_normMap;

}

void Importer::read()
{
	ifstream file(objFile);
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
			inputString >> special >> tmp_norm.x >> tmp_norm.y >> tmp_norm.z;

			//Adding Normals to a Vector
			normals.push_back(tmp_norm);
		}
		else if (nextLine.substr(0, 2) == "f ")
		{
			//Reading the first Face-Index (*i/j/k* i/j/k i/j/k)
			inputString >> special >> tmp_idx1.face_pos >> slashes >> tmp_idx1.face_tex >> slashes >> tmp_idx1.face_norm;
			//cout << idx.temp_face_pos << "/" << idx.temp_face_tex <<  "/" << idx.temp_face_norm << " ";

			//Pushing the temporairy vectors to the final vector
			//face_idxs.push_back(tmp_idx);

			//How many indexes there are
			index_counter++;



			//Reading the second Face-Index (i/j/k *i/j/k* i/j/k)
			inputString >> tmp_idx2.face_pos >> slashes >> tmp_idx2.face_tex >> slashes >> tmp_idx2.face_norm;
			//cout << idx.temp_face_pos << "/" << idx.temp_face_tex << "/" << idx.temp_face_norm << " ";


	
			//Pushing the temporairy vectors to the final vector
			//face_idxs.push_back(tmp_idx);

			//How many indexes there are
			index_counter++;



			//Reading the third Face-Index (i/j/k i/j/k *i/j/k*)
			inputString >> tmp_idx3.face_pos >> slashes >> tmp_idx3.face_tex >> slashes >> tmp_idx3.face_norm;
			//cout << idx.temp_face_pos << "/" << idx.temp_face_tex << "/" << idx.temp_face_norm << "\n";

			//Pushing the temporairy vectors to the final vector
			//face_idxs.push_back(tmp_idx);

			//How many indexes there are
			index_counter++;

			face_idxs.push_back(tmp_idx1);
			face_idxs.push_back(tmp_idx3);
			face_idxs.push_back(tmp_idx2);
			
			materialIndex++;


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
		else if (nextLine.substr(0, 7) == "usemtl ")
		{
			//Reading shader group
			inputString >> special >> tmp_materialInfo.shadingGroup;

			tmp_materialInfo.materialOffset = materialIndex;

			//pushing the found shader group to a Vector
			materialInfo.push_back(tmp_materialInfo);
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
			inputString >> special >> tmp_diff.x >> tmp_diff.y >> tmp_diff.z;

			//cout << tmp_diff.r << " ###### " << tmp_diff.g << " ###### " << tmp_diff.b << " ###### ";

			diffuses.push_back(tmp_diff);
		}
		else if (nextLine.substr(0, 3) == "Ka ")
		{
			inputString >> special >> tmp_ambient.x >> tmp_ambient.y >> tmp_ambient.z;

			//cout << tmp_ambient.r << " ###### " << tmp_ambient.g << " ###### " << tmp_ambient.b << " ###### ";

			ambients.push_back(tmp_ambient);
		}
		else if (nextLine.substr(0, 3) == "Ks ")
		{
			inputString >> special >> tmp_specular.x >> tmp_specular.y >> tmp_specular.z;

			//cout << tmp_specular.r << " ###### " << tmp_specular.g << " ###### " << tmp_specular.b << "\n";

			speculars.push_back(tmp_specular);
		}

		//Transparency
		else if (nextLine.substr(0, 3) == "Tf ")
		{
			inputString >> special >> tmp_trans.x >> tmp_trans.y >> tmp_trans.z;

			//cout << tmp_trans.r << " ###### " << tmp_trans.g << " ###### " << tmp_trans.b << "\n";
			transes.push_back(tmp_trans);
		}
		else if (nextLine.substr(0, 7) == "map_Kd ")
		{
			inputString >> special >> tmp_texMap;

			//cout << tmp_texMap << "\n";

			textureMap.push_back(tmp_texMap);
		}
		else if (nextLine.substr(0, 5) == "bump ")
		{
			inputString >> special >> tmp_normMap;

			tmp_normMap = "./objFiles/" + tmp_normMap;

			//cout << tmp_normMap << "\n";
			normalMap.push_back(tmp_normMap);
		}

	}
	mtlFile.close();
	file.close();
	for (int i = 0; i < face_idxs.size(); i++)
	{
			tmp_fin.x = vertices[face_idxs[i].face_pos-1].x;
			tmp_fin.y = vertices[face_idxs[i].face_pos-1].y;
			tmp_fin.z = vertices[face_idxs[i].face_pos-1].z * -1;
			tmp_fin.u = uvs[face_idxs[i].face_tex-1].u;
			tmp_fin.v = 1 - uvs[face_idxs[i].face_tex-1].v;
			tmp_fin.nx = normals[face_idxs[i].face_norm-1].x;
			tmp_fin.ny = normals[face_idxs[i].face_norm-1].y;
			tmp_fin.nz = normals[face_idxs[i].face_norm-1].z * -1;

			finalVector.push_back(tmp_fin);
			//cout << import.face_idxs[i].face_pos[j] << ", " << import.face_idxs[i].face_tex[j] << ", " << import.face_idxs[i].face_norm[j] << ", ";
		
	}
	//reverse(finalVector.begin(), finalVector.end());
}


//SEBBE NORMAL SHEIT



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
	*/

//int main()
//{
//	Importer import;
//	import.read();
//
//	int counter = 0;
//	for (int i = 0; i < import.face_idxs.size(); i++)
//	{
//		for (int j = 0; j < import.face_idxs[i].face_pos.size(); j++)
//		{
//			import.finalVector[i].x = import.vertices[import.face_idxs[i].face_pos[j]].x;
//			import.finalVector[i].y = import.vertices[import.face_idxs[i].face_pos[j]].y;
//			import.finalVector[i].z = import.vertices[import.face_idxs[i].face_pos[j]].z;
//			import.finalVector[i].u = import.uvs[import.face_idxs[i].face_tex[j]].u;
//			import.finalVector[i].v = import.uvs[import.face_idxs[i].face_tex[j]].v;
//			import.finalVector[i].nx = import.normals[import.face_idxs[i].face_norm[j]].x;
//			import.finalVector[i].ny = import.normals[import.face_idxs[i].face_norm[j]].y;
//			import.finalVector[i].nz = import.normals[import.face_idxs[i].face_norm[j]].z;
//			//cout << import.face_idxs[i].face_pos[j] << ", " << import.face_idxs[i].face_tex[j] << ", " << import.face_idxs[i].face_norm[j] << ", ";
//			counter++;
//			if (counter == 3)
//			{
//				//cout << "\n";
//				counter = 0;
//			}
//		}
//	}
//
//
//	//cin.ignore();
//}
