#include "quadtree.h"
#include "importer.h"
#include <ctgmath>

void QuadTree::ReleaseNode(TreeNode * node)
{
	//Recursively release any child nodes
	for (size_t i = 0; i < 4; i++)
	{
		if (node->nodes[i] != 0)
			ReleaseNode(node->nodes[i]);
	}
	//If there are buffers we release them
	if (node->vertexBuffer)
	{
		node->vertexBuffer->Release();
		node->vertexBuffer = 0;
	}
	if (node->indexBuffer)
	{
		node->indexBuffer->Release();
		node->indexBuffer = 0;
	}
	//Delete and zero the child nodes
	for (size_t i = 0; i < 4; i++)
	{
		if (node->nodes[i])
		{
			delete node->nodes[i];
			node->nodes[i] = 0;
		}

	}
}

void QuadTree::DimensionCalc(int count, float & posX, float & posY, float & meshWidth)
{
	//w = width, d = depth
	float wMax, dMax, wMin, dMin, width, depth, xMax, yMax;

	//centering geometry
	posX = 0.0f;
	posY = 0.0f;

	//Locate center point of geometry
	for (size_t i = 0; i < count; i++)
	{
		posX += this->obj->finalVector.at(i).x;
		posY += this->obj->finalVector.at(i).y;
	}

	posX = posX / (float)count;
	posY = posY / (float)count;


	//locate proper min/max values
	wMax = 0;
	dMax = 0;

	//fabs = absolute values
	wMin = fabsf(this->obj->finalVector.at(0).x - posX);
	dMin = fabsf(this->obj->finalVector.at(0).y - posY);

	for (int i = 0; i < count; i++)
	{

		width = fabsf(this->obj->finalVector.at(i).x - posX);
		depth = fabsf(this->obj->finalVector.at(i).y - posY);

		if (width > wMax) { wMax = width; }
		if (depth > dMax) { dMax = depth; }
		if (width < wMin) { wMin = width; }
		if (depth < dMin) { dMin = depth; }
	}

	//MIN/MAX between depth and width
	xMax = (float)max(fabs(wMin), fabs(wMax));
	yMax = (float)max(fabs(dMin), fabs(dMax));

	//set initial area for quadtree to encompass // use multiplier to tweak for various scene sizes
	meshWidth = max(xMax, yMax) * 5.0f;
	return;
}

void QuadTree::CreateTreeNode(TreeNode * parent, float posX, float posY, float width, ID3D11Device* gDevice)
{
	int numTriangles, count, vertexCount, index, vertexIndex;
	unsigned int i;
	float offsetX, offsetZ;

	//prepare buffer descriptions
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;


	//init initial values
	parent->posX = posX;
	parent->posY = posY;
	parent->width = width;
	parent->triangleCount = 0;
	parent->indexBuffer = 0;
	parent->vertexBuffer = 0;
	parent->VertexCount = 0;
	parent->IndexCount = 0;
	parent->nodes[0] = 0;
	parent->nodes[1] = 0;
	parent->nodes[2] = 0;
	parent->nodes[3] = 0;

	//count triangles
	numTriangles = CountTriangles(posX,posY, width);

	//if there are none, we dont want a treenode
	if (numTriangles == 0)
		return;


	//MAX_TRIANGLES specifies maximum triangles for a single TreeNode. if the ammount we found is too large we need to divide the quadtrant into 4 smaller nodes and recursively try again. (depth first)
	if (numTriangles > MAX_TRIANGLES)
	{
		for (size_t i = 0; i < 4; i++)
		{
			if (float(i % 2) < 1)
				offsetX = -1.0f * (width / 4.0f);
			else
				offsetX = 1.0f * (width / 4.0f);

			if (float(i % 4) < 2)
				offsetZ = -1.0f * (width / 4.0f);
			else
				offsetZ = 1.0f * (width / 4.0f);

			float posXWithOffset, posZWithOffset;
			posXWithOffset = (posX + offsetX);
			posZWithOffset = (posY + offsetZ);
			count = CountTriangles(posXWithOffset, posZWithOffset, (width / 2.0f));

			if (count > 0)
			{
				//If there are triangles inside where this new node would be, then we create the child node
				parent->nodes[i] = new TreeNode;

				//Extend the tree starting from this new child node
				CreateTreeNode(parent->nodes[i], posXWithOffset, posZWithOffset, (width / 2), gDevice);
			}

		}
		return;
	}
	



	//we are within our set Treenode bounds, so we prepare buffers with their vertex data

	parent->triangleCount = numTriangles;
	vertexCount = numTriangles * 3;
	parent->VertexCount = vertexCount;


	//Calculate the number of vertices, a triangle is 3 verts
	vertexCount = numTriangles * 3;
	parent->VertexCount = vertexCount;

	//Create vertex/index arrays
	std::vector<VertexType> newVert;
	newVert.resize(vertexCount);
	std::vector<unsigned int> newInd;
	newInd.resize(this->indexCount);


	//Initialize the index
	index = 0;
	UINT indexCount = 0;
	bool alreadyExist = false;

	//Build triangles for every 3 vertices IF they pass the isContained() check, which specifies if its withing the bounds of the TreeNode
	for (size_t i = 0; i < triangleCount; i++)
	{
		//If the triangle is inside this node then add it to the vertex array
		if (isContained(i, posX, posY, width))
		{
			//Calculate the index into the terrain vertex list
			vertexIndex = i * 3;

			//Get the three vertices of this triangle from the vertex list.
			newVert[index].x = this->obj->finalVector.at(vertexIndex).x;
			newVert[index].y = this->obj->finalVector.at(vertexIndex).y;
			newVert[index].z = this->obj->finalVector.at(vertexIndex).z;
			newVert[index].u = this->obj->finalVector.at(vertexIndex).u;
			newVert[index].v = this->obj->finalVector.at(vertexIndex).v;
			newVert[index].nx = this->obj->finalVector.at(vertexIndex).nx;
			newVert[index].ny = this->obj->finalVector.at(vertexIndex).ny;
			newVert[index].nz = this->obj->finalVector.at(vertexIndex).nz;
			newInd[index] = index;
			index++;
			indexCount++;
			vertexIndex++;

			newVert[index].x = this->obj->finalVector.at(vertexIndex).x;
			newVert[index].y = this->obj->finalVector.at(vertexIndex).y;
			newVert[index].z = this->obj->finalVector.at(vertexIndex).z;
			newVert[index].u = this->obj->finalVector.at(vertexIndex).u;
			newVert[index].v = this->obj->finalVector.at(vertexIndex).v;
			newVert[index].nx = this->obj->finalVector.at(vertexIndex).nx;
			newVert[index].ny = this->obj->finalVector.at(vertexIndex).ny;
			newVert[index].nz = this->obj->finalVector.at(vertexIndex).nz;
			newInd[index] = index;
			index++;
			indexCount++;
			vertexIndex++;

			newVert[index].x = this->obj->finalVector.at(vertexIndex).x;
			newVert[index].y = this->obj->finalVector.at(vertexIndex).y;
			newVert[index].z = this->obj->finalVector.at(vertexIndex).z;
			newVert[index].u = this->obj->finalVector.at(vertexIndex).u;
			newVert[index].v = this->obj->finalVector.at(vertexIndex).v;
			newVert[index].nx = this->obj->finalVector.at(vertexIndex).nx;
			newVert[index].ny = this->obj->finalVector.at(vertexIndex).ny;
			newVert[index].nz = this->obj->finalVector.at(vertexIndex).nz;
			newInd[index] = index;
			index++;
			indexCount++;

		}
	}
	newInd.shrink_to_fit();
	newInd.resize(indexCount);
	parent->IndexCount = indexCount;

	//Set up the description for the vertex buffer
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType)* vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//Give the subresource structure a pointer to the vertex data
	vertexData.pSysMem = newVert.data();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	HRESULT hr;
	//Create the vertex buffer
	hr = gDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &parent->vertexBuffer);

	//Set up the description of the index buffer

	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.ByteWidth = sizeof(unsigned int) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	//Give the subresource structure a pointer to the index data
	indexData.pSysMem = newInd.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	//Create index buffer
	hr = gDevice->CreateBuffer(&indexBufferDesc, &indexData, &parent->indexBuffer);

	//add to the total TreeNode Count
	this->nodeCount += 1;
	return;

}

int QuadTree::CountTriangles(float posX, float posY, float width)
{
	int count;
	unsigned int i;
	bool result;

	count = 0;

	//Loop through all the triangles in the entire mesh and check wich ones should be inside this node
	for (i = 0; i < triangleCount; i++)
	{
		if  (isContained(i, posX, posY, width))
			count++;
	}
	return count;
}

bool QuadTree::isContained(int index, float posX, float posY, float width)
{
	float radius;
	int vertexIndex;
	float x1, z1, x2, z2, x3, z3;
	float minX, maxX, minZ, maxZ;

	//Calculate radius of this node
	radius = width / 2.0f;

	//Get the index into the vertex list
	vertexIndex = index * 3;

	//Get the three vertices of this triangle from the vertex list
	x1 = this->obj->finalVector.at(vertexIndex).x;
	z1 = this->obj->finalVector.at(vertexIndex).z;
								
	x2 = this->obj->finalVector.at(vertexIndex).x;
	z2 = this->obj->finalVector.at(vertexIndex).z;
								   
	x3 = this->obj->finalVector.at(vertexIndex).x;
	z3 = this->obj->finalVector.at(vertexIndex).z;

	//Check if the minimum of the x coords of the triangle is inside the node
	minX = min(x1, min(x2, x3));
	if (minX > (posX + radius))
		return false;

	//Check if the maximum of the x coords is inside the node
	maxX = max(x1, max(x2, x3));
	if (maxX < (posX - radius))
		return false;


	//check if the minimum of the z coords is inside the node
	minZ = min(z1, min(z2, z3));
	if (minZ >(posY + radius))
		return false;

	//Check if the maximum z coord are inside the node
	maxZ = max(z1, max(z2, z3));
	if (maxZ < (posY - radius))
		return false;


	return true;
}

QuadTree::QuadTree()
{

}

QuadTree::QuadTree(const QuadTree & parent)
{
}

QuadTree::~QuadTree()
{
}

bool QuadTree::Initialize(ID3D11Device * gDevice, ID3D11DeviceContext * gDeviceContext, Importer* obj)
{

	//Create the parent node of the mesh
	this->gDevice = gDevice;
	this->gDeviceContext = gDeviceContext;
	this->obj = obj;

	this->vertCount = 0;
	this->indexCount = 0;
	float width;
	float posX, posY;

	//prep initial parent values based on loaded OBJ file
	this->vertCount = 0;
	this->indexCount = 0;
	
	this->vertCount = this->obj->finalVector.size();
	this->indexCount = this->obj->finalVector.size();

	triangleCount = indexCount / 3;

	//calculate node dimensions
	DimensionCalc(indexCount, posX, posY, width);
	TreeNode* tempParentNode = new TreeNode;
	m_parentNode = tempParentNode;

	//Recursively build the quad tree, based on the vertex list and mesh dimensions
	CreateTreeNode(m_parentNode, posX, posY, width, gDevice);

	return true;
}

//THIS FUNCTION IS DEPRICATED
void QuadTree::BindNodeBuffers(TreeNode* node)
{
	UINT32 vertexSize = sizeof(VertexType);
	UINT32 vertexCount = node->VertexCount;
	UINT32 indexSize = this->obj->index_counter;
	UINT32 offset = 0;

	gDeviceContext->IASetVertexBuffers(0, 1, &node->vertexBuffer, &vertexSize, &offset);
	gDeviceContext->IASetIndexBuffer(node->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

}

void QuadTree::Release()
{
	//Recursively release the quad tree data
	ReleaseNode(m_parentNode);
	delete m_parentNode;
	return;
}
