#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <vector>
#include "importer.h"

#define MAX_TRIANGLES 7200

struct VertexType
{
	float x, y, z;
	float u, v;
	float nx, ny, nz;
};

struct TreeNode
{
	float posX;
	float posY;
	float width;
	unsigned int triangleCount;
	ID3D11Buffer *vertexBuffer;
	ID3D11Buffer *indexBuffer;
	unsigned int VertexCount;
	unsigned int IndexCount;
	TreeNode* nodes[4];
};

class QuadTree
{
	private:
		Importer* obj = nullptr;
		ID3D11DeviceContext* gDeviceContext = nullptr;
		ID3D11Device* gDevice = nullptr;

		std::vector<TreeNode*>*	parentNode;
		//std::vector<> vertexList;
		unsigned int nodeCount;
		unsigned int vertCount;
		unsigned int indexCount;
		unsigned int triangleCount;
		unsigned int drawCount;

		TreeNode*	m_parentNode;

		void ReleaseNode(TreeNode* node);
		void DimensionCalc(int count, float& posX, float& posY, float& width);
		void CreateTreeNode(TreeNode* parent, float posX, float posY, float width, ID3D11Device* gDevice);
		int	 CountTriangles(float posX, float posY, float width);
		bool isContained(int index, float posX, float posY, float width);
	public:
		QuadTree();
		QuadTree(const QuadTree &parent);
		~QuadTree();

		bool Initialize(ID3D11Device *gDevice, ID3D11DeviceContext *gDeviceContext, Importer* obj);
		void BindNodeBuffers(TreeNode* node);
		void Release();
		TreeNode* getParent() { return this->m_parentNode; };
		unsigned int getNodeAmt() { return this->nodeCount; };
};