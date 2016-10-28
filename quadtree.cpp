#include "quadtree.h"
#include "importer.h"
#include <ctgmath>
Importer obj;
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



void QuadTree::DimensionCalc(int count, float & posX, float & posY, float & width)
{
	//w = width, d = depth
	float wMax, dMax, wMin, dMin, width, depth, xMax, yMax;
	//centering geometry
	posX = 0.0f;
	posY = 0.0f;
	//Locate center point of geometry
	for (size_t i = 0; i < count; i++)
	{
		posX += obj.finalVector.at(i).x;
		posY += obj.finalVector.at(i).y;
	}

	posX = posX / (float)count;
	posY = posY / (float)count;


	//locate proper min/max values
	wMax = 0;
	dMax = 0;
	//fabs = absolute values
	wMin = fabsf(obj.finalVector.at(0).x - posX);
	dMin = fabsf(obj.finalVector.at(0).y - posY);

	for (int i = 0; i < count; i++)
	{

		width = fabsf(obj.finalVector.at(i).x - posX);
		depth = fabsf(obj.finalVector.at(i).y - posY);

		if (width > wMax) { wMax = width; }
		if (depth > dMax) { dMax = depth; }
		if (width < wMin) { wMin = width; }
		if (depth < dMin) { dMin = depth; }
	}

	xMax = (float)max(fabs(wMin), fabs(wMax));
	yMax = (float)max(fabs(dMin), fabs(dMax));

	width = max(xMax, yMax) * 10.95f;
	return;
}

void QuadTree::CreateTreeNode(TreeNode * parent, float posX, float posY, float width, ID3D11Device* gDevice)
{
	int numTriangles, count, vertexCount, index, vertexIndex;
	unsigned int i;
	float offsetX, offsetZ;


	VertexType* vertices = nullptr;
	unsigned int* indices = nullptr;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	parent->posX = posX;
	parent->posY = posY;
	parent->width = width;
	parent->triangleCount = 0;
	parent->indexBuffer = 0;
	parent->vertexBuffer = 0;
	parent->nodes[0] = 0;
	parent->nodes[1] = 0;
	parent->nodes[2] = 0;
	parent->nodes[3] = 0;
	numTriangles = CountTriangles(posX,posY, width);

	if (numTriangles == 0)
		return;

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

			float posXWithOffset, posYWithOffset;
			posXWithOffset = (posX + offsetX);
			posYWithOffset = (posY + offsetZ);
			count = CountTriangles(posXWithOffset, posYWithOffset, (width / 2.0f));

			if (count > 0)
			{
				//If there are triangles inside where this new node would be, then we create the child node
				parent->nodes[i] = new TreeNode;

				//Extend the tree starting from this new child node
				CreateTreeNode(parent->nodes[i], posXWithOffset, posYWithOffset, (width / 2), gDevice);
			}

		}
		return;
	}
	
	parent->triangleCount = numTriangles;
	vertexCount = numTriangles * 3;
	parent->VertexCount = vertexCount;


	//Calculate the number of vertices
	vertexCount = numTriangles * 3;
	parent->VertexCount = vertexCount;
	//Create vertex array
	vertices = new VertexType[vertexCount];
	std::vector<VertexType> newVert;
	newVert.resize(vertexCount);
	std::vector<unsigned int> newInd;
	newInd.resize(this->indexCount);
	//Create the index array
	indices = new unsigned int[this->indexCount];
	//std::vector<UINT> indices2;


	//Initialize the index
	index = 0;
	UINT indexCount = 0;
	bool alreadyExist = false;

	for (size_t i = 0; i < triangleCount; i++)
	{
		//If the triangle is inside this node then add it to the vertex array
		if (isContained(i, posX, posY, width))
		{
			//Calculate the index into the terrain vertex list
			vertexIndex = i * 3;

			//Get the three vertices of this triangle from the vertex list.
			newVert[index].x = obj.finalVector.at(vertexIndex).x;
			newVert[index].y = obj.finalVector.at(vertexIndex).y;
			newVert[index].z = obj.finalVector.at(vertexIndex).z;
			newVert[index].u = obj.finalVector.at(vertexIndex).u;
			newVert[index].v = obj.finalVector.at(vertexIndex).v;
			newInd[index] = index;
			index++;
			indexCount++;
			vertexIndex++;

			newVert[index].x = obj.finalVector.at(vertexIndex).x;
			newVert[index].y = obj.finalVector.at(vertexIndex).y;
			newVert[index].z = obj.finalVector.at(vertexIndex).z;
			newVert[index].u = obj.finalVector.at(vertexIndex).u;
			newVert[index].v = obj.finalVector.at(vertexIndex).v;
			newInd[index] = index;
			index++;
			indexCount++;
			vertexIndex++;

			newVert[index].x = obj.finalVector.at(vertexIndex).x;
			newVert[index].y = obj.finalVector.at(vertexIndex).y;
			newVert[index].z = obj.finalVector.at(vertexIndex).z;
			newVert[index].u = obj.finalVector.at(vertexIndex).u;
			newVert[index].v = obj.finalVector.at(vertexIndex).v;
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

	//Delete the vertices and indices arrays, as they are now stored in the buffers

	delete[] vertices;
	vertices = 0;
	delete[] indices;
	indices = 0;
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
	x1 = obj.finalVector.at(vertexIndex).x;
	z1 = obj.finalVector.at(vertexIndex).z;
	vertexIndex++;

	x2 = obj.finalVector.at(vertexIndex).x;
	z2 = obj.finalVector.at(vertexIndex).z;
	vertexIndex++;

	x3 = obj.finalVector.at(vertexIndex).x;
	z3 = obj.finalVector.at(vertexIndex).z;

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

bool QuadTree::Initialize(ID3D11Device * gDevice, ID3D11DeviceContext * gDeviceContext)
{
	//Create the parent node of the mesh

	this->gDevice = gDevice;
	this->gDeviceContext = gDeviceContext;

	this->vertCount = 0;
	this->indexCount = 0;
	float width;
	float posX, posY;

	//prep 1 scenemesh for quadtreeing
	this->vertCount = 0;
	this->indexCount = 0;

	this->vertCount = obj.finalVector.size();
	this->indexCount = obj.finalVector.size();

	triangleCount = indexCount / 3;

	DimensionCalc(indexCount, posX, posY, width);
	TreeNode* tempParentNode = new TreeNode;
	m_parentNode = tempParentNode;
	//Recursively build the quad tree, based on the vertex list and mesh dimensions
	CreateTreeNode(m_parentNode, posX, posY, width, gDevice);

	return true;
}

void QuadTree::BindBuffersforATree(TreeNode* node)
{
	//for each tree in the quadtree, recursively find trees with info, then bind that buffer and render, then continue to the next.
	for (size_t i = 0; i < 4; i++)
	{
		if (node->nodes[i] != 0)
			BindBuffersforATree(node->nodes[i]);
	}

	UINT32 vertexSize = sizeof(VertexType);
	UINT32 vertexCount = node->VertexCount;
	UINT32 indexSize = obj.index_counter;
	UINT32 offset = 0;

	gDeviceContext->IASetVertexBuffers(0, 1, &node->vertexBuffer, &vertexSize, &offset);
	gDeviceContext->IASetIndexBuffer(node->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	callRender();


}

void QuadTree::Release()
{
	//Recursively release the quad tree data
	for (size_t i = 0; i < m_parentNode->size(); i++)
	{
		if (m_parentNode->at(i))
		{
			ReleaseNode(m_parentNode->at(i));
			delete m_parentNode->at(i);
			m_parentNode->at(i) = 0;
		}
	}
	delete m_parentNode;

	return;
}
