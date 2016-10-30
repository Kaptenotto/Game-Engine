#pragma region Includes
#include <windows.h>
#include "Constants.h"

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include <string>
#include <dinput.h>
#include <vector>
#include<cstdlib>
#include<ctime>
#include <d2d1.h>
#include <dxgi.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include "importer.h"
#include "Wic.h"
//#include "ParticleSystem.h"
#include <Wincodec.h>

using namespace DirectX;
using namespace std;

#pragma endregion

#pragma region pragmaComments
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")
#pragma endregion

HWND InitWindow(HINSTANCE hInstance);

HWND hwnd = NULL;

// message procedure, HWND = handle to window.
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT CreateDirect3DContext(HWND wndHandle);
IDXGISwapChain* gSwapChain = nullptr;

#pragma region Devices
ID3D11Device* gDevice = nullptr;
ID3D11DeviceContext* gDeviceContext = nullptr;
#pragma endregion

ID3D11InputLayout* gVertexLayout = nullptr;
#pragma region Init shaders
ID3D11VertexShader* gVertexShader = nullptr;

ID3D11PixelShader* gPixelShader = nullptr;

ID3D11GeometryShader* gGeometryShader = nullptr;

ID3D11VertexShader* gVertexShaderShadow = nullptr;

ID3D11PixelShader* gPixelShaderShadow = nullptr;

ID3D11GeometryShader* gGeometryShaderShadow = nullptr;

ID3D11VertexShader* gVertexShaderParticle = nullptr;

ID3D11PixelShader* gPixelShaderParticle = nullptr;

ID3D11GeometryShader* gGeometryShaderParticle = nullptr;

ID3D11VertexShader* GBuffer_VS = nullptr;
ID3D11GeometryShader* GBuffer_GS = nullptr;
ID3D11PixelShader* GBuffer_PS = nullptr;
ID3D11InputLayout* GBuffer_VertexLayout = nullptr;

ID3D11VertexShader*	FinalPass_VS = nullptr;
ID3D11PixelShader* FinalPass_PS = nullptr;
ID3D11InputLayout* FinalPass_VertexLayout = nullptr;
#pragma endregion

int numOfModels = 20; // amount of models that are going to be culled.
int numOfModelsToDraw;
const int numTrees = 4000;
void cullAABB(vector<XMFLOAT4> &frustrumPlanes);
vector<XMFLOAT3> CreateAABB(vector<XMFLOAT3> &vertPosArray);
vector<XMFLOAT4> getFrustrumPlanes(XMMATRIX& viewProj);
std::vector<XMFLOAT3> AABBs;

int numTreesToDraw;
struct InstanceData
{
	XMFLOAT3 pos;
};
vector<InstanceData>instancedData(numOfModels);

#pragma region init Structs
struct Vertex    //Overloaded Vertex Structure
{
	Vertex() {}
	Vertex(float x, float y, float z,
		float u, float v,
		float nx, float ny, float nz)
		: pos(x, y, z), texCoord(u, v), normal(nx, ny, nz) {}

	XMFLOAT3 pos;
	XMFLOAT2 texCoord;
	XMFLOAT3 normal;
};

struct HeightMapInfo {        // Heightmap structure
	int terrainWidth;        // Width of heightmap
	int terrainHeight;        // Height (Length) of heightmap
	XMFLOAT3 *heightMap;    // Array to store terrain's vertex positions
};

struct MatrixBuffer {
	XMMATRIX World;
	XMMATRIX camView;
	XMMATRIX Projection;
	XMMATRIX WVP;
	XMVECTOR camPos;

};
MatrixBuffer matrices;

struct LightBuffer
{
	XMVECTOR position;
	XMMATRIX view;
	XMMATRIX projection;
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT3 lightDir;
};
LightBuffer Lights;

typedef struct DIMOUSESTATES
{
	LONG IX;
	LONG IY;
	LONG IZ;
	BYTE rgbButtons[4];
};

struct BillboardData
{
	XMFLOAT3 worldPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 direction = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3 color = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float height = 1.0f;
	float width = 1.0f;
	float depth = 1.0f;
	float r, g, b;
};

struct StartingPos
{
	float x, y, z;
};

#pragma endregion

#pragma region init buffers

ID3D11Buffer* gVertexBuffer = nullptr;
ID3D11Buffer* gVertexBufferParticle = nullptr;
ID3D11Buffer* gVertexBufferExplosion = nullptr;
ID3D11Buffer* gIndexBuffer = nullptr;

ID3D11Buffer* gConstantBuffer = nullptr;
ID3D11Buffer* gConstantLightBuffer = nullptr;
ID3D11Buffer* gVertexBufferFinalPass = nullptr;

ID3D11Buffer* groundIndexBuffer = nullptr;
ID3D11Buffer* groundVertexBuffer = nullptr;
ID3D11Texture2D* gBackBuffer = nullptr;

ID3D11RenderTargetView* gBackBufferRTV = nullptr;

//ID3D11Texture2D* gShadowBackBuffer = nullptr;
//ID3D11RenderTargetView* gShadowRenderTarget = nullptr; //egen

ID3D11DepthStencilView* gDepthStencilView = nullptr;
ID3D11DepthStencilView* GBufferDepthStencilView = nullptr;

ID3D11DepthStencilView* gShadowDepthStencilView = nullptr;
ID3D11ShaderResourceView* ShadowDepthResource = nullptr;

D3D11_RASTERIZER_DESC rasterDesc;
ID3D11RasterizerState* gRasterState = nullptr;

ID3D11Texture2D* GBuffer_Textures[numRTVs] = { nullptr };
ID3D11RenderTargetView* textureRTVs[numRTVs] = { nullptr };
ID3D11ShaderResourceView* shaderResourceViews[numRTVs] = { nullptr };
#pragma endregion

#pragma region collisionVariables
struct Collision {
	// Information about ellipsoid (in world space)
	XMVECTOR ellipsoidSpace;
	XMVECTOR w_Position;
	XMVECTOR w_Velocity;

	// Information about ellipsoid (in ellipsoid space)
	XMVECTOR e_Position;
	XMVECTOR e_Velocity;
	XMVECTOR e_normalizedVelocity;

	// Collision Information
	bool foundCollision;
	float nearestDistance;
	XMVECTOR intersectionPoint;
	int collisionRecursionDepth;
};

const float unitsPerMeter = 100.0f;
XMVECTOR gravity = XMVectorSet(0.0f, -0.2f, 0.0f, 0.0f);

vector<XMFLOAT3> collidableGeometryPositions;
vector<DWORD> collidableGeometryIndices;

int NumFaces = 0;
int NumVertices = 0;

UINT32 groundVertexSize;

#pragma endregion

#pragma region preDefinedFunctions

void RenderExplosion();

void calcRay(float mouseX, float mouseY, XMVECTOR& rayOrigin, XMVECTOR& rayDirection);
float Intersection(XMVECTOR rayOrigin, XMVECTOR rayDirection);
bool PointInTriangle(XMVECTOR&triV1, XMVECTOR&triV2, XMVECTOR&triV3, XMVECTOR&point);

XMVECTOR CollisionSlide(Collision& cP,
	std::vector<XMFLOAT3>& vertPos,
	std::vector<DWORD>& indices);
bool getLowestRoot(float a, float b, float c, float maxR, float*root);
bool checkPointInTriangle(const XMVECTOR& point, const XMVECTOR& triV1, const XMVECTOR& triV2, const XMVECTOR& triV3);
bool SphereCollidingWithTriangle(Collision& cP, XMVECTOR &p0, XMVECTOR &p1, XMVECTOR &p2, XMVECTOR &triNormal);
XMVECTOR CollideWithWorld(Collision& cP,
	std::vector<XMFLOAT3>& vertPos,
	std::vector<DWORD>& indices);
#pragma endregion

// INITIALIZE SHADER THINGS

vector<ID3D11ShaderResourceView*> textureResources;
vector<ID3D11ShaderResourceView*> normalResources;

ID3D11SamplerState* texSamplerState;
ID3D11SamplerState* linearSamplerState;
ID3D11SamplerState* pointSamplerState;
Importer obj;

#pragma region GlobalsFPSCAM
XMVECTOR defaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
XMVECTOR defaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
XMVECTOR camForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
XMVECTOR camRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

//XMVECTOR defaultUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
//XMVECTOR camUpDown = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

XMMATRIX camRotationMatrix;
XMMATRIX groundWorld;

float moveLeftRight = 0.0f;		// Used to move along the camFoward and camRight vectors
float moveBackForward = 0.0f;   // Used to move along the camFoward and camRight vectors
float moveupDown = 0.0f;

float camYaw = 0.0f;
float camPitch = 0.0f;
#pragma endregion

#pragma region camVectors

XMVECTOR camPosition = XMVectorSet(10.0, 10.0, 20.0, 0.0);
XMVECTOR camTarget = XMVectorSet(0.0, 0.0, 0.0, 0.0);
XMVECTOR camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

XMVECTOR lightPosition = XMVectorSet(10, 7, -5, 1);
XMVECTOR lightDir = XMVectorSet(0, 0, 0, 0);
XMVECTOR lightUp = XMVectorSet(0, 1, 0, 0);

#pragma endregion

float clearColor[] = { 0, 0, 0, 1 };

float particleSize;

int maxParticles;

vector<BillboardData> explosionTest;

vector<BillboardData> vertexList;

vector<StartingPos> explosionStartingPos;

vector<float> velocity;

int maxExplosionParticles;
float expDeviationX, expDeviationY, expDeviationZ;
bool explosion = false;
XMVECTOR origin;

float particleDeviationX, particleDeviationY, particleDeviationZ;

//GLOBALS FOR INPUT ************************************************

HWND wndHandle = NULL;

ID3D11Resource* HeightTex;
ID3D11ShaderResourceView* heightResource;

IDirectInputDevice8* DIKeyboard;
IDirectInputDevice8* DIMouse;

DIMOUSESTATES mouseLastState;
LPDIRECTINPUT8 directInput;

float rotx = 0;
float rotz = 0;
float scaleX = 1.0f;
float scaleY = 1.0f;

int clientWidth = 0;
int clientHeight = 0;
std::wstring printText;

bool isShot = false;

XMMATRIX rotationX;
XMMATRIX rotationY;

XMVECTOR camUpDown = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

//TEXT
//IDXGIKeyedMutex *keyedMutex11;
//IDXGIKeyedMutex *keyedMutex10;
//ID2D1RenderTarget *D2DRenderTarget;
//ID2D1SolidColorBrush *Brush;
// TIME GLOBALS ****************************************************

double countsPerSecond = 0.0;
__int64 counterStart = 0;

int frameCount = 0;
int fps = 0;

__int64 frameTimeOld = 0;
double frameTime;

ID3D11Buffer *triBuffer = nullptr;    // global


// FUNCTIONS********************************************************



#pragma region Shaders

void CreateShaders()
{
	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 3 , D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * 5, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * 11, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//{ "TEXCOORD1", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 13 , D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//{ "TEXCOORD2", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 15 , D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	//Create the vertex shader

	ID3DBlob* pVS = nullptr;
	D3DCompileFromFile(
		L"Vertex.hlsl",	//Name of file
		nullptr,
		nullptr,
		"VS_main",				// Name of main in file
		"vs_4_0",
		0,
		0,
		&pVS,
		nullptr
	);

	HRESULT Hr = gDevice->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &gVertexShader);

	//Create an input-layout to describe the input buffer data for the input-assembler stage
	gDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &gVertexLayout);

	//Do not need the com object anymore therefor releasing it
	pVS->Release();

	//Create pixel shader

	ID3DBlob* pPS = nullptr;
	D3DCompileFromFile(
		L"Fragment.hlsl",	//name of file
		nullptr,			//optional macros
		nullptr,			// optional include files
		"PS_main",			// Entry point
		"ps_4_0",			// Shader model target
		0,					//shader compile options
		0,					// Effect compile options
		&pPS,				//double pointer to ID3DBlob
		nullptr				// point for error blob messages
	);
	Hr = gDevice->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &gPixelShader);

	pPS->Release();

	//Create geometry shader

	ID3DBlob* pGS = nullptr;
	D3DCompileFromFile(
		L"GeometryShader.hlsl",
		nullptr,
		nullptr,
		"main",
		"gs_4_0",
		0,
		0,
		&pGS,
		nullptr
	);
	gDevice->CreateGeometryShader(pGS->GetBufferPointer(), pGS->GetBufferSize(), nullptr, &gGeometryShader);
	pGS->Release();

	//CREATE SHADERS FOR SHADOWMAP
	//Create the vertex shader shadow
	ID3DBlob* pVSShadow = nullptr;
	D3DCompileFromFile(
		L"VertexShadow.hlsl",
		nullptr,
		nullptr,
		"VS_main",
		"vs_4_0",
		0,
		0,
		&pVSShadow,
		nullptr
	);
	Hr = gDevice->CreateVertexShader(pVSShadow->GetBufferPointer(), pVSShadow->GetBufferSize(), nullptr, &gVertexShaderShadow);
	gDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVSShadow->GetBufferPointer(), pVSShadow->GetBufferSize(), &gVertexLayout);
	pVSShadow->Release();

	//CREATE SHADERS FOR SHADOWMAP DONE

	D3D11_INPUT_ELEMENT_DESC inputDesc2[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "DIRECTION", 0,  DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * 3, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * 6, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "HEIGHT",0,DXGI_FORMAT_R32_FLOAT, 0, sizeof(float) * 7,D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WIDTH",0,DXGI_FORMAT_R32_FLOAT, 0, sizeof(float) * 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	};



	ID3DBlob* pVSParticle = nullptr;
	D3DCompileFromFile(
		L"VertexShaderParticle.hlsl",
		nullptr,
		nullptr,
		"main",
		"vs_4_0",
		0,
		0,
		&pVSParticle,
		nullptr
	);
	Hr = gDevice->CreateVertexShader(pVSParticle->GetBufferPointer(), pVSParticle->GetBufferSize(), nullptr, &gVertexShaderParticle);
	gDevice->CreateInputLayout(inputDesc2, ARRAYSIZE(inputDesc2), pVSParticle->GetBufferPointer(), pVSParticle->GetBufferSize(), &gVertexLayout);
	pVSParticle->Release();

	ID3DBlob* pGSParticle = nullptr;
	D3DCompileFromFile(
		L"GeometryShaderParticle.hlsl",
		nullptr,
		nullptr,
		"main",
		"gs_4_0",
		0,
		0,
		&pGSParticle,
		nullptr
	);
	gDevice->CreateGeometryShader(pGSParticle->GetBufferPointer(), pGSParticle->GetBufferSize(), nullptr, &gGeometryShaderParticle);
	pGSParticle->Release();

	ID3DBlob* pPSParticles = nullptr;
	D3DCompileFromFile(
		L"PixelShaderParticle.hlsl",	//name of file
		nullptr,			//optional macros
		nullptr,			// optional include files
		"main",			// Entry point
		"ps_4_0",			// Shader model target
		0,					//shader compile options
		0,					// Effect compile options
		&pPSParticles,				//double pointer to ID3DBlob
		nullptr				// point for error blob messages
	);
	Hr = gDevice->CreatePixelShader(pPSParticles->GetBufferPointer(), pPSParticles->GetBufferSize(), nullptr, &gPixelShaderParticle);

	pPSParticles->Release();
}
void CreateDefferedShaders()
{
	//IMPUT LAYOUT
	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 3 , D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * 5, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * 11, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	HRESULT hr;
	ID3DBlob* pVS = nullptr;

	//Vertex shader
	D3DCompileFromFile(
		L"GBuffer.hlsl",
		nullptr,
		nullptr,
		"GBUFFER_VS_main",
		"vs_5_0",
		0,
		0,
		&pVS,
		nullptr);

	hr = gDevice->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &GBuffer_VS);

	hr = gDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &GBuffer_VertexLayout);
	pVS->Release();

	//Geometry shader
	ID3DBlob* pGS = nullptr;
	D3DCompileFromFile(
		L"GBuffer.hlsl",
		nullptr,
		nullptr,
		"GBUFFER_GS_main",
		"gs_5_0",
		0,
		0,
		&pGS,
		nullptr);

	hr = gDevice->CreateGeometryShader(pGS->GetBufferPointer(), pGS->GetBufferSize(), nullptr, &GBuffer_GS);
	pGS->Release();

	//Pixel shader
	ID3DBlob *pPs = nullptr;
	D3DCompileFromFile(
		L"GBuffer.hlsl",
		nullptr,
		nullptr,
		"GBUFFER_PS_main",
		"ps_5_0",
		0,
		0,
		&pPs,
		nullptr);

	hr = gDevice->CreatePixelShader(pPs->GetBufferPointer(), pPs->GetBufferSize(), nullptr, &GBuffer_PS);
	pPs->Release();
}
void CreateTextureViews()
{
	for (int i = 0; i < numRTVs; i++)
	{
		HRESULT hr;
		D3D11_TEXTURE2D_DESC textureDesc;
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetDesc;
		D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc;


		ZeroMemory(&textureDesc, sizeof(textureDesc));
		//Set up the render texture desciption

		textureDesc.Width = (UINT)WIN_WIDTH;
		textureDesc.Height = (UINT)WIN_HEIGHT;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		//Create the render target Texture

		hr = gDevice->CreateTexture2D(&textureDesc, NULL, &GBuffer_Textures[i]);
		if (FAILED(hr))
			MessageBox(NULL, L"Failed to create  Gbuffer", L"Error", MB_ICONERROR | MB_OK);

		//set up description for render target view
		ZeroMemory(&renderTargetDesc, sizeof(renderTargetDesc));
		renderTargetDesc.Format = textureDesc.Format;
		renderTargetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetDesc.Texture2D.MipSlice = 0;

		//Create render target

		hr = gDevice->CreateRenderTargetView(GBuffer_Textures[i], &renderTargetDesc, &textureRTVs[i]);
		if (FAILED(hr))
			MessageBox(NULL, L"Failed to create  Gbuffer", L"Error", MB_ICONERROR | MB_OK);

		//Set up the shader resource view
		ZeroMemory(&resourceViewDesc, sizeof(resourceViewDesc));
		resourceViewDesc.Format = textureDesc.Format;
		resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		resourceViewDesc.Texture2D.MostDetailedMip = 0;
		resourceViewDesc.Texture2D.MipLevels = 1;

		//Create the resourceView;


		hr = gDevice->CreateShaderResourceView(GBuffer_Textures[i], nullptr, &shaderResourceViews[i]);
		if (FAILED(hr))
			MessageBox(NULL, L"Failed to create  Gbuffer", L"Error", MB_ICONERROR | MB_OK);
	}

	HRESULT hr;

	ID3D11Texture2D* GBufferStencilTest;

	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	//Set up the render texture desciption

	textureDesc.Width = (UINT)WIN_WIDTH;
	textureDesc.Height = (UINT)WIN_HEIGHT;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_D32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	hr = gDevice->CreateTexture2D(&textureDesc, NULL, &GBufferStencilTest);
	if (FAILED(hr))
		MessageBox(NULL, L"nyfel1", L"Error", MB_ICONERROR | MB_OK);

	hr = gDevice->CreateDepthStencilView(GBufferStencilTest, nullptr, &GBufferDepthStencilView);
	if (FAILED(hr))
		MessageBox(NULL, L"nyfel2", L"Error", MB_ICONERROR | MB_OK);
	GBufferStencilTest->Release();


	for (int i = 0; i < numRTVs; i++)
	{
		GBuffer_Textures[i]->Release();
	}


	ID3D11Resource* HeightTex;
	ID3D11ShaderResourceView* heightResource;

	const wchar_t* wcharFilePath2 = L"grass.jpg";
	CreateWICTextureFromFile(gDevice, gDeviceContext, wcharFilePath2, &HeightTex, &heightResource, 0);





}
void finalPassQuadData()
{
	struct TriangleVertex
	{
		float x, y, z;
		float u, v;
	};
	TriangleVertex triangleVertices[6] =
	{
		//T1

		-1.0f, -1.0f, 0.0f,	//v0 pos
							//1.0f, 0.0f, 0.0f,	//v0 color
							0.0f, 1.0f,			//v0 UV

							-1.0f, 1.0f, 0.0f,	//v1
												//0.0f, 1.0f, 0.0f,	//v1 color
												0.0f, 0.0f,			//v1 UV

												1.0f, -1.0f, 0.0f, //v2
																   //0.0f, 0.0f, 0.0f,	//v2 color
																   1.0f, 1.0f,			//v2 UV

																						//T2

																						-1.0f, 1.0f, 0.0f,	//v3 pos
																											//0.5f, 0.0f, 0.5f,	//v3 color
																											0.0f, 0.0f,			//v3 UV

																											1.0f, 1.0f, 0.0f,	//v4
																																//1.0f, 0.0f, 0.0f,	//v4 color
																																1.0f, 0.0f,			//v4 UV

																																1.0f, -1.0f, 0.0f,	//v5
																																					//0.0f, 0.0f, 0.0f	//v5 color
																																					1.0f, 1.0f			//v5 UV

	};

	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(triangleVertices);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = triangleVertices;
	gDevice->CreateBuffer(&bufferDesc, &data, &gVertexBufferFinalPass);
}
void CreateFinalPassShaders()
{
	HRESULT hr;

	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP; //wrap, (repeat) for use of tiling texutures
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	samplerDesc.BorderColor[0] = 0.0f;
	samplerDesc.BorderColor[1] = 0.0f;
	samplerDesc.BorderColor[2] = 0.0f;
	samplerDesc.BorderColor[3] = 1.0f;

	hr = gDevice->CreateSamplerState(&samplerDesc, &linearSamplerState);

	gDeviceContext->PSSetSamplers(0, 1, &linearSamplerState);


	D3D11_SAMPLER_DESC samplerDescPoint;
	samplerDescPoint.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDescPoint.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDescPoint.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDescPoint.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDescPoint.MipLODBias = 0.0f;  //mipmap offset level
	samplerDescPoint.MaxAnisotropy = 1;
	samplerDescPoint.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDescPoint.MinLOD = 0;
	samplerDescPoint.MaxLOD = D3D11_FLOAT32_MAX;

	hr = gDevice->CreateSamplerState(&samplerDesc, &pointSamplerState);

	gDeviceContext->PSSetSamplers(1, 1, &pointSamplerState);


	ID3DBlob* pVS = nullptr;

	D3DCompileFromFile(
		L"FinalPass.hlsl",
		nullptr,
		nullptr,
		"VS_main",
		"vs_5_0",
		0,
		0,
		&pVS,
		nullptr);

	hr = gDevice->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &FinalPass_VS);

	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
		/*POSITION*/{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		/*UV*/{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = gDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &FinalPass_VertexLayout);
	pVS->Release();

	ID3DBlob *pPs = nullptr;
	D3DCompileFromFile(
		L"FinalPass.hlsl",
		nullptr,
		nullptr,
		"PS_main",
		"ps_5_0",
		0,
		0,
		&pPs,
		nullptr);

	hr = gDevice->CreatePixelShader(pPs->GetBufferPointer(), pPs->GetBufferSize(), nullptr, &FinalPass_PS);
	pPs->Release();
}

#pragma endregion

void createTextures()
{
	HRESULT hr;

	ID3D11Resource* texResource;
	ID3D11Resource* norResource;

	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	if (!obj.textureMap.empty())
	{
		for (int i = 0; i < obj.textureMap.size(); i++)
		{
			wstring filePath;

			ID3D11ShaderResourceView* textureResource;

			textureResources.push_back(textureResource);

			filePath.assign(obj.textureMap[i].begin(), obj.textureMap[i].end());

			const wchar_t* wcharFilePath = filePath.c_str();


			hr = CreateWICTextureFromFile(
				gDevice,
				gDeviceContext,
				wcharFilePath,
				&texResource,
				&textureResources[i],
				0
			);
		}
		texResource->Release();
	}

	if (!obj.normalMap.empty())
	{
		for (int i = 0; i < obj.normalMap.size(); i++)
		{
			wstring filePath;
			ID3D11ShaderResourceView* normalResource;

			normalResources.push_back(normalResource);

			filePath.assign(obj.normalMap[i].begin(), obj.normalMap[i].end());

			const wchar_t* wcharFilePath = filePath.c_str();

			hr = CreateWICTextureFromFile(
				gDevice,
				gDeviceContext,
				wcharFilePath,
				&norResource,
				&normalResources[i],
				0
			);
		}

		norResource->Release();
	}


	const wchar_t* wcharFilePath2 = L"grass.jpg";
	CreateWICTextureFromFile(gDevice, gDeviceContext, wcharFilePath2, &HeightTex, &heightResource, 0);

}

#pragma region triangleToCheckCulling

//Just a simple struct for the triangle
struct TriangleVertex
{
	float x, y, z;
	float u, v;
};

void CreateTriangleForBackFace()
{
	
	TriangleVertex triangleVertices[6]  =
	{
		-1.0f, -1.0f, 0.0f,	//v0 pos
		0.0f, 1.0f,			//v0 UV

		-1.0f, 1.0f, 0.0f,	//v1
		0.0f, 0.0f,			//v1 UV

		1.0f, -1.0f, 0.0f, //v2
		1.0f, 1.0f,			//v2 UV

		//T2

		-1.0f, 1.0f, 0.0f,	//v3 pos
		0.0f, 0.0f,			//v3 UV

		1.0f, 1.0f, 0.0f,	//v4
		1.0f, 0.0f,			//v4 UV

		1.0f, -1.0f, 0.0f,	//v5
		1.0f, 1.0f			//v5 UV

	};


	D3D11_BUFFER_DESC bd;
	memset(&bd, 0, sizeof(bd));
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(triangleVertices);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = triangleVertices;

	gDevice->CreateBuffer(&bd, &data, &triBuffer);       // create the buffer
}

#pragma endregion

void createTriangle()
{
	D3D11_BUFFER_DESC bufferdesc;
	std::memset(&bufferdesc, 0, sizeof(bufferdesc));
	bufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferdesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferdesc.ByteWidth = sizeof(obj.finalVector[0]) * obj.finalVector.size();
	bufferdesc.MiscFlags = 0;
	bufferdesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &obj.finalVector[0];
	HRESULT hr = gDevice->CreateBuffer(&bufferdesc, &data, &gVertexBuffer);

	D3D11_BUFFER_DESC bufferDesc2;
	bufferDesc2.ByteWidth = sizeof(obj.face_idxs[0]) * obj.face_idxs.size();
	bufferDesc2.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc2.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc2.CPUAccessFlags = 0;
	bufferDesc2.MiscFlags = 0;
	bufferDesc2.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &obj.face_idxs[0];
	hr = gDevice->CreateBuffer(&bufferDesc2, &initData, &gIndexBuffer);
}

void createLightDepthStencil()
{
	ID3D11Texture2D* shadowDepthStencil = NULL;

	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = (float)2048;
	descDepth.Height = (float)2048;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_R32_TYPELESS;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;

	HRESULT hr = gDevice->CreateTexture2D(&descDepth, NULL, &shadowDepthStencil);

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	hr = gDevice->CreateDepthStencilView(shadowDepthStencil, &descDSV, &gShadowDepthStencilView);

	D3D11_SHADER_RESOURCE_VIEW_DESC ShadowRDesc;
	ShadowRDesc.Format = DXGI_FORMAT_R32_FLOAT;
	ShadowRDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	ShadowRDesc.Texture2D.MostDetailedMip = 0;
	ShadowRDesc.Texture2D.MipLevels = 1;

	hr = gDevice->CreateShaderResourceView(shadowDepthStencil, &ShadowRDesc, &ShadowDepthResource);

	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	//rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	hr = gDevice->CreateRasterizerState(&rasterDesc, &gRasterState);
	gDeviceContext->RSSetState(gRasterState);
}

void createDepthStencil()
{
	ID3D11Texture2D* gDepthStencil = NULL;
	D3D11_TEXTURE2D_DESC backBufferSurfaceDesc;

	gBackBuffer->GetDesc(&backBufferSurfaceDesc);

	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = backBufferSurfaceDesc.Width;
	descDepth.Height = backBufferSurfaceDesc.Height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = backBufferSurfaceDesc.SampleDesc.Count;
	descDepth.SampleDesc.Quality = backBufferSurfaceDesc.SampleDesc.Quality;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	HRESULT hr = gDevice->CreateTexture2D(&descDepth, NULL, &gDepthStencil);

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	descDSV.Texture2D.MipSlice = 0;

	hr = gDevice->CreateDepthStencilView(gDepthStencil, &descDSV, &gDepthStencilView);
}

void ConstantBuffer()
{
	float fovangleY = XM_PI * 0.45;
	float aspectRatio = 1280.0 / 720.0;
	float nearZ = 0.01; // adjusts how close we can get
	float farZ = 1000.0; // adjusts how far you can see

	//LIGHT
	float lightfovangleY = XM_PI * 0.5;
	float lightaspectRatio = 2048.0f / 2048.0f;
	float lightnearZ = 5.0;
	float lightfarZ = 60.0;

	matrices.camView = XMMatrixLookAtLH(
		(camPosition),
		(camTarget),
		(camUp));

	matrices.camPos = camPosition;

	matrices.Projection = XMMatrixPerspectiveFovLH(
		(fovangleY),    //  The field of view in radians along the y-axis
		(aspectRatio),  //  The aspect ratio, usually width/height
		(nearZ),		//  A float describing the distance from the camera to the near z-plane
		(farZ)			//  A float describing the distance from the camera to the far plane
	);

	matrices.camView = XMMatrixTranspose(matrices.camView);
	matrices.Projection = XMMatrixTranspose(matrices.Projection);
	matrices.World = XMMatrixIdentity();

	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(D3D11_BUFFER_DESC));
	desc.ByteWidth = sizeof(MatrixBuffer);  // size of the buffer in bytes, here the size as the MatrixBuffer struct containing the matrices.

	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // Identifies how the buffer will be bound to the pipeline here as a constant buffer
	desc.Usage = D3D11_USAGE_DEFAULT; // how the buffer is be read and written to , here specified to default
	desc.MiscFlags = 0;               // miscellenous flags
	desc.StructureByteStride = 0;     // the size of each element in the buffer structure

	D3D11_SUBRESOURCE_DATA data;  // specifices data for the initialization data
	data.pSysMem = &matrices; // pSysMem is a pointer to the initialization data and sets the initialization data to be the matrices.

	HRESULT hr = gDevice->CreateBuffer(&desc, &data, &gConstantBuffer); // Creating a buffer in this case constantbuffer.

	gDeviceContext->GSSetConstantBuffers(0, 1, &gConstantBuffer); //Setting the constant buffer to the geometry shader.
	gDeviceContext->VSSetConstantBuffers(0, 1, &gConstantBuffer); //Setting the constant buffer to the Vertex shader.
	gDeviceContext->PSSetConstantBuffers(0, 1, &gConstantBuffer); //Setting the constant buffer to the Vertex shader.

	Lights.ambient = { 0.4f, 0.4f, 0.4f, 1.0f };
	Lights.diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	Lights.position = lightPosition;
	Lights.lightDir = { 1.0f, 1.0f, 1.0f };

	Lights.view = XMMatrixLookAtLH(
		(lightPosition),
		(lightDir),
		(lightUp));
	Lights.view = XMMatrixTranspose(Lights.view);

	Lights.projection = XMMatrixPerspectiveFovLH(
		(lightfovangleY),    //  The field of view in radians along the y-axis
		(lightaspectRatio),  //  The aspect ratio, usually width/height
		(lightnearZ),		//  A float describing the distance from the camera to the near z-plane
		(lightfarZ)			//  A float describing the distance from the camera to the far plane
	);
	Lights.projection = XMMatrixTranspose(Lights.projection);

	D3D11_BUFFER_DESC lightDesc;
	memset(&lightDesc, 0, sizeof(D3D11_BUFFER_DESC));
	lightDesc.ByteWidth = sizeof(LightBuffer);  // size of the buffer in bytes, here the size as the MatrixBuffer struct containing the matrices.

	lightDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // Identifies how the buffer will be bound to the pipeline here as a constant buffer
	lightDesc.Usage = D3D11_USAGE_DEFAULT; // how the buffer is be read and written to , here specified to default
	lightDesc.MiscFlags = 0;               // miscellenous flags
	lightDesc.StructureByteStride = 0;     // the size of each element in the buffer structure

	D3D11_SUBRESOURCE_DATA lightData;  // specifices data for the initialization data
	lightData.pSysMem = &Lights; // pSysMem is a pointer to the initialization data and sets the initialization data to be the matrices.

	hr = gDevice->CreateBuffer(&lightDesc, &lightData, &gConstantLightBuffer); // Creating a buffer in this case constantbuffer.

	gDeviceContext->GSSetConstantBuffers(1, 1, &gConstantLightBuffer); //Setting the constant buffer to the geometry shader.
																	   //gDeviceContext->VSSetConstantBuffers(1, 1, &gConstantLightBuffer); //Setting the constant buffer to the geometry shader.
	gDeviceContext->PSSetConstantBuffers(1, 1, &gConstantLightBuffer); //Setting the constant buffer to the geometry shader.

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.MipLODBias = 0.0f;
	sampDesc.MaxAnisotropy = 1;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	gDevice->CreateSamplerState(&sampDesc, &texSamplerState);

}

#pragma region settingViewPorts

void SetViewport()
{
	D3D11_VIEWPORT vP;
	vP.Width = WIN_WIDTH;
	vP.Height = WIN_HEIGHT;
	vP.MinDepth = 0.0f;
	vP.MaxDepth = 1.0f;
	vP.TopLeftX = 0;
	vP.TopLeftY = 0;
	gDeviceContext->RSSetViewports(1, &vP);
}

void SetViewportShadow()
{
	D3D11_VIEWPORT vP;
	vP.Width = SHADOW_WIDTH;
	vP.Height = SHADOW_HEIGHT;
	vP.MinDepth = 0.0f;
	vP.MaxDepth = 1.0f;
	vP.TopLeftX = 0;
	vP.TopLeftY = 0;
	gDeviceContext->RSSetViewports(1, &vP);
}
#pragma endregion

bool initDirectInput(HINSTANCE hIstancen)
{
	HRESULT hr = DirectInput8Create(
		hIstancen,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&directInput,
		NULL);

	hr = directInput->CreateDevice(GUID_SysKeyboard,
		&DIKeyboard,
		NULL);

	hr = directInput->CreateDevice(GUID_SysMouse,
		&DIMouse,
		NULL);

	hr = DIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	hr = DIKeyboard->SetCooperativeLevel(wndHandle, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	hr = DIMouse->SetDataFormat(&c_dfDIMouse);
	hr = DIMouse->SetCooperativeLevel(wndHandle, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);

	return true;
}

void updateCamera()
{
	camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0); // Used to rotate around all the axis at the same time with the functoin XMMatixRotationpitchyaw
	camTarget = XMVector3TransformCoord(defaultForward, camRotationMatrix); // sets the camera target vector by rotating the defaultforward vector with the																	// rotation matrix we created
	camTarget = XMVector3Normalize(camTarget); // normalizing the camtarget vector

	XMMATRIX RotateYTempMatrix;
	RotateYTempMatrix = XMMatrixRotationY(camYaw);

	camRight = XMVector3TransformCoord(defaultRight, RotateYTempMatrix);
	camUp = XMVector3TransformCoord(camUp, RotateYTempMatrix);
	camForward = XMVector3TransformCoord(defaultForward, RotateYTempMatrix);

	Collision cameraCP;
	cameraCP.ellipsoidSpace = XMVectorSet(1.0f, 3.0f, 1.0f, 0.0f);
	cameraCP.w_Position = camPosition;
	cameraCP.w_Velocity = (moveLeftRight*camRight) + (moveBackForward*camForward);

	camPosition = CollisionSlide(cameraCP,
		collidableGeometryPositions,
		collidableGeometryIndices);

	/*camPosition += moveLeftRight* camRight;
	camPosition += moveBackForward* camForward;*/
	camPosition += moveupDown * camUpDown;

	moveLeftRight = 0.0f;
	moveBackForward = 0.0f;
	moveupDown = 0.0f;

	camTarget = camPosition + camTarget;

	matrices.camPos = camPosition;
	matrices.camView = XMMatrixLookAtLH(camPosition, camTarget, camUp);
	matrices.camView = XMMatrixTranspose(matrices.camView);

}

void detectInput(double time) // checking keyboard and mouse input for movement in Engine
{

	DIMOUSESTATES mouseCurrentState;

	BYTE keyBoardState[256]; // the amount of buttons a char array of 256.

	DIKeyboard->Acquire();
	DIMouse->Acquire();

	DIMouse->GetDeviceState(sizeof(DIMOUSESTATES), &mouseCurrentState);

	DIKeyboard->GetDeviceState(sizeof(keyBoardState), (LPVOID)&keyBoardState);

	float speed = 10.0 * time;

	if (keyBoardState[DIK_ESCAPE] & 0x80)
	{
		PostMessage(wndHandle, WM_DESTROY, 0, 0);
	}
	if (keyBoardState[DIK_A] & 0x80)
	{
		moveLeftRight -= speed;
	}
	if (keyBoardState[DIK_D] & 0x80)
	{
		moveLeftRight += speed;
	}
	if (keyBoardState[DIK_W] & 0x80)
	{
		moveBackForward += speed;
	}
	if (keyBoardState[DIK_S] & 0x80)
	{
		moveBackForward -= speed;
	}
	if (keyBoardState[DIK_LSHIFT] & 0x80)
	{
		moveupDown -= speed;
	}
	if (keyBoardState[DIK_SPACE] & 0x80)
	{
		moveupDown += speed;
	}
	if (mouseCurrentState.rgbButtons[0])
	{
		//ShowCursor(TRUE);
		XMVECTOR rayOrigin, rayDirection;
		POINT mousePos;

		GetCursorPos(&mousePos);
		ScreenToClient(hwnd, &mousePos);

		int mouseX = mousePos.x;
		int mouseY = mousePos.y;
		if (mouseX > WIN_WIDTH)
			mouseX = WIN_WIDTH;
		else if (mouseX < 0)
			mouseX = 0;
		if (mouseY > WIN_HEIGHT)
			mouseY = WIN_HEIGHT;
		else if (mouseY < 0)
			mouseY = 0;

		float tempDist;
		float closestDist = FLT_MAX;

		calcRay((float)mouseX, (float)mouseY, rayOrigin, rayDirection);

		tempDist = Intersection(rayOrigin, rayDirection);

		if (tempDist < closestDist)
		{
			
			closestDist = tempDist;
		}

		if (closestDist < FLT_MAX)
		{
			for (int i = 0; i < 3; i++)
			{
				clearColor[i] = 1.0f;
			}
		}
		if (closestDist == FLT_MAX)
		{
			for (int i = 0; i < 3; i++)
			{
				clearColor[i] = 0.0f;
			}
		}
	}
	if (!mouseCurrentState.rgbButtons[0])
	{
		isShot = false;
	}

	if ((mouseCurrentState.IX != mouseLastState.IX) || (mouseCurrentState.IY != mouseLastState.IY))
	{
		camYaw += mouseLastState.IX * 0.001f;

		camPitch += mouseCurrentState.IY * 0.001f;

		mouseLastState = mouseCurrentState;
	}
	if (keyBoardState[DIK_R] & 0x80)
	{
		explosion = true;
	}
	updateCamera();

	return;
}

#pragma region timeFunctions
void startTimer()
{
	LARGE_INTEGER frequencycount;

	QueryPerformanceFrequency(&frequencycount);
	countsPerSecond = double(frequencycount.QuadPart);

	QueryPerformanceCounter(&frequencycount);
	counterStart = frequencycount.QuadPart;
}

double getTime()
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	return double(currentTime.QuadPart - counterStart) / countsPerSecond;
}

double getFrameTime()
{
	LARGE_INTEGER currentTime;
	__int64 tickCount;
	QueryPerformanceCounter(&currentTime);

	tickCount = currentTime.QuadPart - frameTimeOld;
	frameTimeOld = currentTime.QuadPart;

	if (tickCount < 0.0f)
	{
		tickCount = 0.0f;
	}

	return float(tickCount) / countsPerSecond;
}
#pragma endregion

#pragma region Explosion

void initExplosion()
{
	origin = { 0.0f,0.0f,0.0f };

	expDeviationX = 20.0f;
	expDeviationY = 2.0f;
	expDeviationZ = 20.0f;

	maxExplosionParticles = 5000;

	float positionX, positionY, positionZ, red, green, blue;
	//int index, i, j;
	int i = 0;
	/*while (i < maxExplosionParticles)
	{
		//generate randomized particle properties.
		positionX = ((float)rand() / (RAND_MAX)+(rand() % 1)) - 0.5f;
		positionY = ((float)rand() / (RAND_MAX)+(rand() % 1)) - 0.5f;
		positionZ = ((float)rand() / (RAND_MAX)+(rand() % 1)) - 0.5f;

		//saving the starting position for all particles for reset;
		explosionStartingPos.push_back(StartingPos{ positionX,positionY,positionZ });

		explosionTest.push_back(VertexType{ positionX, positionY, positionZ, 0.02f, 0.02f, 1.0f, 0.0f, 0.0f });

		i++;
	}*/

	D3D11_BUFFER_DESC bufferdesc;
	std::memset(&bufferdesc, 0, sizeof(bufferdesc));
	bufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferdesc.ByteWidth = sizeof(BillboardData) * maxExplosionParticles;
	bufferdesc.MiscFlags = 0;
	bufferdesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = explosionTest.data();
	HRESULT hr = gDevice->CreateBuffer(&bufferdesc, &data, &gVertexBufferExplosion);

}

void updateExplosion()
{
	XMVECTOR travelDir;
	float x, y, z;

	for (int i = 0; i < explosionTest.size() - 1; i++)
	{
		XMVECTOR temp = { explosionTest[i].height,explosionTest[i].width,explosionTest[i].depth };
		travelDir = temp - origin;
		x = XMVectorGetX(travelDir);
		y = XMVectorGetY(travelDir);
		z = XMVectorGetZ(travelDir);

		explosionTest[i].height = explosionTest[i].height + x / 10.0f;
		explosionTest[i].width = explosionTest[i].width + y / 10.0f;
		explosionTest[i].depth = explosionTest[i].depth + z / 10.0f;
	}

	D3D11_MAPPED_SUBRESOURCE gMappedResource;
	BillboardData* vtxPtr;
	HRESULT hr = gDeviceContext->Map(gVertexBufferExplosion, 0, D3D11_MAP_WRITE_DISCARD, 0, &gMappedResource);

	vtxPtr = (BillboardData*)gMappedResource.pData;
	memcpy(vtxPtr, explosionTest.data(), sizeof(BillboardData) * maxExplosionParticles);

	gDeviceContext->Unmap(gVertexBufferExplosion, 0);
}

void killExplosion()
{
	if (explosion)
	{
		if (explosionTest[0].height >= 5 || explosionTest[0].width >= 5 || explosionTest[0].depth >= 5)
		{
			for (int i = 0; i < maxExplosionParticles - 1; i++)
			{
				explosionTest[i].height = explosionStartingPos[i].y;
				explosionTest[i].width = explosionStartingPos[i].x;
				explosionTest[i].depth = explosionStartingPos[i].z;
			}
			explosion = false;
		}
	}
}

#pragma endregion

#pragma region CreateHeightMap

bool HeightMapLoad(char *fileName, HeightMapInfo &hminfo)
{
	FILE *filePointer;                            // Point to the current position in the file
	BITMAPFILEHEADER bitmapFileHeader;        // Structure which stores information about file
	BITMAPINFOHEADER bitmapInfoHeader;        // Structure which stores information about image
	int imageSize, index;
	unsigned char height;

	// Open the file
	filePointer = fopen(fileName, "rb");
	if (filePointer == NULL)
		return 0;

	// Läser bitmapens header
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePointer);

	// läser in infoheader för bitmapen
	fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePointer);

	// hämtar width och height från bitmapen och lagrar de som width och height i hminfo structen
	hminfo.terrainWidth = bitmapInfoHeader.biWidth;
	hminfo.terrainHeight = bitmapInfoHeader.biHeight;

	// Storlek av bilden i bytes, 3an representerar rgb för varje pixel
	imageSize = hminfo.terrainWidth * hminfo.terrainHeight * 3;

	//Initierar arrayen som lagrar bildens data 
	unsigned char* bitmapImageArray = new unsigned char[imageSize];

	// Sätter filens pekare till början av bildens data
	fseek(filePointer, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Lagrar bild datan i bitmapimage.
	fread(bitmapImageArray, 1, imageSize, filePointer);

	// Stänger filen.
	fclose(filePointer);

	// Initierar heightmap arrayen och lagrar verticerna i vår terräng
	hminfo.heightMap = new XMFLOAT3[hminfo.terrainWidth * hminfo.terrainHeight];

	// Vi anänvder en gråskalad Bmp bild så alla 3 rgb väden är det samma men vi  behöver bara en för höjd
	// Därför anävnder vi denna int som räknare för att skippa två komponenter i bildens data, vi läser endast R 
	int skipGB = 0;

	// Denna float delar vi med för att sänka terrängens terräng och minska att den inte är lika "spiky", utan istället mer jämn.
	float heightFactor = 25.0f;

	// Läser bildens data in i vår heightmap array
	//två loopar för att läsa höjd och bredd.
	for (int j = 0; j< hminfo.terrainHeight; j++)
	{
		for (int i = 0; i< hminfo.terrainWidth; i++)
		{
			height = bitmapImageArray[skipGB];

			index = (hminfo.terrainHeight * j) + i;

			hminfo.heightMap[index].x = (float)i;
			// Behöver endast dela med heightfactor på y för att det är där vi har vår höjd i y led.
			hminfo.heightMap[index].y = (float)height / heightFactor;
			hminfo.heightMap[index].z = (float)j;

			skipGB += 3;
		}
	}

	//Deletar sedan vår bitmapimage då vi är klara med den.
	delete[] bitmapImageArray;
	bitmapImageArray = 0;

	return true;

}

void CreateHeightMap()
{
	//Här anger vi ett namn för att komma åt vår struct heightMapInfo och väljer även namnet på vår heightmapfil.
	HeightMapInfo hmInfo;
	HeightMapLoad("Heightmap2.bmp", hmInfo);        // Load the heightmap and store it into hmInfo

	//Lägger in terrägens brädd och höjd i respektive int variabel.
	int cols = hmInfo.terrainWidth;
	int rows = hmInfo.terrainHeight;

	//Skapar gridden verticer endast genom att multiplicera rader och kolumner som står för höjd och bredd.
	//Vi subtraherar här med -1 i varje för att undvika hörnen och sedan multiplicerar vi med 2 för att få fram antalet faces.
	NumVertices = rows * cols;
	NumFaces = (rows - 1)*(cols - 1) * 2;

	//Gör vertex structen med antalet vertices som vi beräknade tidigare.
	std::vector<Vertex> v(NumVertices);


	// Denna loopen loopar igenom informationen i hminfo för att kunna ge vår vertices våra positioner som var lagrade där.
	// Vi sätter även normalen uppåt för att kunna lysa upp ytan
	for (DWORD i = 0; i < rows; ++i)
	{
		for (DWORD j = 0; j < cols; ++j)
		{
			v[i*cols + j].pos = hmInfo.heightMap[i*cols + j];
			v[i*cols + j].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		}
	}

	std::vector<DWORD> indices(NumFaces * 3);

	int k = 0;
	int textureUIndex = 0;
	int texureVIndex = 0;
	// Anledningen  till varför vi även använder textureUVindex är för att om vi sätter samma på alla så kommer nästa quad skriva över texturkoordinaterna
	// där de är vertices som delar de två quadsen. Därför annvänder vi wrappin för att komma runt det. Där du använder värden högre än ett eller lägre än 0 för texturering
	for (DWORD i = 0; i < rows - 1; i++)
	{
		for (DWORD j = 0; j < cols - 1; j++)
		{
			indices[k] = i*cols + j;        // Bottom left of quad
			v[i*cols + j].texCoord = XMFLOAT2(textureUIndex + 0.0f, texureVIndex + 1.0f);

			indices[k + 1] = (i + 1)*cols + j;    // Top left of quad
			v[(i + 1)*cols + j].texCoord = XMFLOAT2(textureUIndex + 0.0f, texureVIndex + 0.0f);

			indices[k + 2] = i*cols + j + 1;        // Bottom right of quad
			v[i*cols + j + 1].texCoord = XMFLOAT2(textureUIndex + 1.0f, texureVIndex + 1.0f);

			indices[k + 3] = (i + 1)*cols + j;    // Top left of quad
			v[(i + 1)*cols + j].texCoord = XMFLOAT2(textureUIndex + 0.0f, texureVIndex + 0.0f);

			indices[k + 4] = (i + 1)*cols + j + 1;    // Top right of quad
			v[(i + 1)*cols + j + 1].texCoord = XMFLOAT2(textureUIndex + 1.0f, texureVIndex + 0.0f);

			indices[k + 5] = i*cols + j + 1;        // Bottom right of quad
			v[i*cols + j + 1].texCoord = XMFLOAT2(textureUIndex + 1.0f, texureVIndex + 1.0f);

			k += 6; // adderar med 6 för att komma till nästa quad med indices.

			textureUIndex++;
		}
		textureUIndex = 0;
		texureVIndex++;
	}

	//Lagra terrängens vertex positioner och index i polygon soppan som vi kollar kollisioner med
	// Vi kan lagra all statisk eftersom de är en mesh som inte ändras , det vill säga den vi vill kolla kollisioner med.

	//Vertex offset för att varje mesh lägs till i slutet på positionsarrayen.
	int vertexOffset = collidableGeometryPositions.size();    

	//Temporära arrayer för att vi behöver lagra geometrin i världsspace
	XMVECTOR temporaryVertexPositionVector;
	XMFLOAT3 temporaryVertFloat3;

	// Här lägger vi till vertex positionerna i polygon soppan att kolla med sedan det vill säga i collidableGeometryPositions
	// Där vi först laddar in v i en temp array för att sedan transformera de i världsspace.
	// Sedan lägger vi till de i vår float3 variabel och pussar in den i vår collidablegeometry vector
	for (int i = 0; i < v.size(); i++)
	{
		temporaryVertexPositionVector = XMLoadFloat3(&v[i].pos);

		temporaryVertexPositionVector = XMVector3TransformCoord(temporaryVertexPositionVector, matrices.World);

		XMStoreFloat3(&temporaryVertFloat3, temporaryVertexPositionVector);

		collidableGeometryPositions.push_back(temporaryVertFloat3);
	}

	// Här är sedan nästa steg där vi lägger in indices i "polygon soppan" Vi måste dock vara säkra att vi
	// LÄgger till de ovanpå tidigare inlagda vertex positioner därför använder vi vertexoffset variabeln.

	for (int i = 0; i < indices.size(); i++)
	{
		collidableGeometryIndices.push_back(indices[i] + vertexOffset);
	}

	D3D11_SUBRESOURCE_DATA indexBufferdata;

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * NumFaces * 3;

	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	// När vi använder en vector som bufferns data behöver vi ge en pekare till första elementet i vector arrayen
	// Eller de första elementen vi vill läsa ifrån
	indexBufferdata.pSysMem = &indices[0];

	gDevice->CreateBuffer(&indexBufferDesc, &indexBufferdata, &groundIndexBuffer);


	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	vertexBufferDesc.ByteWidth = sizeof(Vertex) * NumVertices;

	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));

	// När vi använder en vector som bufferns data behöver vi ge en pekare till första elementet i vector arrayen
	// Eller de första elementen vi vill läsa ifrån
	vertexBufferData.pSysMem = &v[0];

	gDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &groundVertexBuffer);
}

#pragma endregion

#pragma region CollisionWitHeightMap

XMVECTOR CollisionSlide(Collision& collisionPoint,
	std::vector<XMFLOAT3>& vertPos,
	std::vector<DWORD>& indices)
{
	//Transformerar hastighets/velocity vektorn till ellipsoidens space
	collisionPoint.e_Velocity = collisionPoint.w_Velocity / collisionPoint.ellipsoidSpace;

	//Vi transformerar även positionvektorn till ellipsoidsens space
	collisionPoint.e_Position = collisionPoint.w_Position / collisionPoint.ellipsoidSpace;

	// Här kollar vi kollision med vår värld som i detta fallet är vår heightmap, denna funktion kallar på sig själv 5 gånger som mest eller
	// till velocity vektorn är helt använd( väldigt liten eller nära noll)
	collisionPoint.collisionRecursionDepth = 0;
	XMVECTOR finalPosition = CollideWithWorld(collisionPoint, vertPos, indices);


	//Vi vill även ha gravitation mot vår heightmap för att kunna skapa sliding beteendet eller för att komma tillbaka ner efter vi hoppat eller flygit
	// Detta gör vi genom att lägga till en ny hastighetsvektor nedåt. Vi definierar denna variabel globalt vid våra variabler och ger den att dra nedåt y axeln
	// Om vi inte hade haft detta hade den slidat ner för den brantare kanterna.

	//Eftersom vi definierar gravitationen i worldspace ändrar vi även om denna till ellipsoid space.
	collisionPoint.e_Velocity = gravity / collisionPoint.ellipsoidSpace;   


	collisionPoint.e_Position = finalPosition;
	collisionPoint.collisionRecursionDepth = 0;
	finalPosition = CollideWithWorld(collisionPoint, vertPos, indices);


	// Slutligen ändrar vi finalposition till world space för att kunna uppdatera vår kamera.
	finalPosition = finalPosition * collisionPoint.ellipsoidSpace;

	// Return our final position!
	return finalPosition;
}

//inte klar
XMVECTOR CollideWithWorld(Collision& collisionPoint,
	std::vector<XMFLOAT3>& vertPos,
	std::vector<DWORD>& indices)
{
	// These are based off the unitsPerMeter from above
	float unitScale = unitsPerMeter / 100.0f;
	// VerycloseDistance variabeln är viktig eftersom den håller spheren/ellipsen borta från att faktiskt röra triangeln
	// annars skulle det skapa stora poblem eftersom den vid varje loop skulle alltid hitta en kollision istället för
	// att glida över trianglen.
	float veryCloseDistance = 0.005f * unitScale; 



	// Denna ifsats är väldigt viktig eftersom den kommer att stoppa oss från att gå in i en infinite eller väldigt long loop
	// Exempelvis är det tillfällen då spheren faktiskt kan be knuffad lite in i triangelns center där, där rekursionen skulle fortsätta upprepa
	// och hitta en kollision även fast att hastigheten inte ändrar.										
	if (collisionPoint.collisionRecursionDepth > 5)
		return collisionPoint.e_Position;

	//normaliserar ellipsens hastighetsvektor
	collisionPoint.e_normalizedVelocity = XMVector3Normalize(collisionPoint.e_Velocity);

	// initierar variabler i collision paketet"
	collisionPoint.foundCollision = false;
	collisionPoint.nearestDistance = 0.0f;

	// loopa igenom varje triangle i meshen och kolla efter en kollision
	for (int triCounter = 0; triCounter < indices.size() / 3; triCounter++)
	{
		//hämta punkterna från trianglen(hämta trianglen)

		XMVECTOR p0, p1, p2, tempVec;
		p0 = XMLoadFloat3(&vertPos[indices[3 * triCounter]]);
		p1 = XMLoadFloat3(&vertPos[indices[3 * triCounter + 1]]);
		p2 = XMLoadFloat3(&vertPos[indices[3 * triCounter + 2]]);

		// Sätter trianglen in i ellipsen space.
		p0 = p0 / collisionPoint.ellipsoidSpace;
		p1 = p1 / collisionPoint.ellipsoidSpace;
		p2 = p2 / collisionPoint.ellipsoidSpace;

		// Beräkna triangelns normal för att skicka med den i collidefunktionen.
		XMVECTOR triNormal;
		triNormal = XMVector3Normalize(XMVector3Cross((p1 - p0), (p2 - p0)));

		// Här använder vi funktionen för att se om sphären kolliderar med triangeln.
		SphereCollidingWithTriangle(collisionPoint, p0, p1, p2, triNormal);
	}

	// Om de inte var någon kollision returnera helt enkelt positionen och hastigheten på ellipsen.
	if (collisionPoint.foundCollision == false) {
		return collisionPoint.e_Position + collisionPoint.e_Velocity;
	}

	// Om vi däremot kommit hit har en kollision inträffat

	// If we've made it here, a collision occured
	// destinationPoint is where the sphere would travel if there was
	// no collisions, however, at this point, there has a been a collision
	// detected. We will use this vector to find the new "sliding" vector
	// based off the plane created from the sphere and collision point
	XMVECTOR destinationPoint = collisionPoint.e_Position + collisionPoint.e_Velocity;

	XMVECTOR newPosition = collisionPoint.e_Position;    // Just initialize newPosition

											 // If the position is further than "veryCloseDistance" from the point
											 // of collision, we will move the sphere along the velocity path until
											 // it "almost" touches the triangle, or point of collision. We do this so
											 // that the next recursion (if there is one) does not detect a collision
											 // with the triangle we just collided with. We don't need to find a collision
											 // with the triangle we just collided with because we will be moving parallel
											 // to it now, and if we were finding the collision with it every recursion
											 // (since it's the closest triangle we would collide with), we would 
											 // finish our 5 recursions (checked above) without ever moving after
											 // touching a triangle, because before the triangle has a chance to move
											 // down the new velocity path (even though it's about parallel with the triangle)
											 // it would find the collision with the triangle, and simply recompute the same
											 // velocity vector it computed the first time. This would happen because of
											 // floating point innacuracy. theoretically, we would not have to worry about this
											 // because after the new velocity vector is created, it SHOULD be perfectly parallel
											 // to the triangle, and we detect that in our code and basically skip triangles
											 // who are perfectly parallel with the velocity vector. But like i said, because
											 // of innacuracy, the new velocity vector might be VERY SLIGHTLY pointed down towards
											 // the triangles plane, which would make us waste a recursion just to recompute the same
											 // velocity vector. Basically, the whole sliding thing works without this, but it's a lot
											 // more "choppy" and "sticky", where you get stuck in random places.
	if (collisionPoint.nearestDistance >= veryCloseDistance)
	{
		// Move the new position down velocity vector to ALMOST touch the collision point
		XMVECTOR V = collisionPoint.e_Velocity;
		V = XMVector3Normalize(V);
		V = V * (collisionPoint.nearestDistance - veryCloseDistance);
		newPosition = collisionPoint.e_Position + V;

		// Adjust polygon intersection point (so sliding
		// plane will be unaffected by the fact that we
		// move slightly less than collision tells us)
		V = XMVector3Normalize(V);
		collisionPoint.intersectionPoint -= veryCloseDistance * V;
	}

	// This is our sliding plane (point in the plane and plane normal)
	XMVECTOR slidePlaneOrigin = collisionPoint.intersectionPoint;
	XMVECTOR slidePlaneNormal = newPosition - collisionPoint.intersectionPoint;
	slidePlaneNormal = XMVector3Normalize(slidePlaneNormal);

	// We will use the sliding plane to compute our new "destination" point
	// and new velocity vector. To do this, we will need to solve another quadratic
	// equation (Ax + By + Cz + D = 0), where D is what we call the plane constant,
	// which we use to find the distance between the sliding plane and our original
	// destination point (original as up until now, since it's likely that this is
	// not the first recursion of this function, and the original original destination
	// has been changed up until now).
	// First the point in the plane
	float x = XMVectorGetX(slidePlaneOrigin);
	float y = XMVectorGetY(slidePlaneOrigin);
	float z = XMVectorGetZ(slidePlaneOrigin);

	// Next the planes normal
	float A = XMVectorGetX(slidePlaneNormal);
	float B = XMVectorGetY(slidePlaneNormal);
	float C = XMVectorGetZ(slidePlaneNormal);
	float D = -((A*x) + (B*y) + (C*z));

	// To keep the variable names clear, we will rename D to planeConstant
	float planeConstant = D;

	// Get the distance between sliding plane and destination point
	float signedDistFromDestPointToSlidingPlane = XMVectorGetX(XMVector3Dot(destinationPoint, slidePlaneNormal)) + planeConstant;

	// Now we calculate the new destination point. To get the new destination point, we will subtract
	// the distance from the plane to the original destination point (down the planes normal) from the
	// original destination point. It's easier to picture this in your head than explain, so let me try
	// to give you a very simple picture. Pretend you are this equation, standing on the plane, where UP
	// (your head) is pointing the same direction as the plane's normal. directly below you is the "destination"
	// point of the sphere. Your job as this equation is to "pull" the destination point up (towards the planes
	// normal) until it is resting "in" the plane. If you can picture this the way i'm trying to get you to, you
	// can see that the new velocity vector (from the point of collision between sphere and plane) to the new
	// destination is "shorter" and parallel to the plane, so that now when the sphere follows this new velocity
	// vector, it will be traveling parallel (sliding) across the triangle, at the same time, it does not travel
	// as far as it would have if there was no collision. This is exactly what we want, because when you think about
	// it, we do not run up mountains as fast as we run on flat ground, and if we run straight into a wall in our
	// game, we will just stop moving, or if we run ALMOST straight into the wall, we will not go cruising sideways,
	// but instead slowly move to either side. In my lesson on braynzarsoft.net, This is explained in pictures
	XMVECTOR newDestinationPoint = destinationPoint - signedDistFromDestPointToSlidingPlane * slidePlaneNormal;

	// I believe this line was covered briefly in the above explanation
	XMVECTOR newVelocityVector = newDestinationPoint - collisionPoint.intersectionPoint;

	// After this check, we will recurse. This check makes sure that we have not
	// come to the end of our velocity vector (or very close to it, because if the velocity
	// vector is very small, there is no reason to lose performance by doing an extra recurse
	// when we won't even notice the distance "thrown away" by this check anyway) before
	// we recurse
	if (XMVectorGetX(XMVector3Length(newVelocityVector)) < veryCloseDistance) {
		return newPosition;
	}

	// We are going to recurse now since a collision was found and the velocity
	// changed directions. we need to check if the new velocity vector will
	// cause the sphere to collide with other geometry.
	collisionPoint.collisionRecursionDepth++;
	collisionPoint.e_Position = newPosition;
	collisionPoint.e_Velocity = newVelocityVector;
	return CollideWithWorld(collisionPoint, vertPos, indices);
}

//inte klar
bool SphereCollidingWithTriangle(Collision& cP, XMVECTOR &p0, XMVECTOR &p1, XMVECTOR &p2, XMVECTOR &triNormal)
{
	// This function assumes p0, p1, p2, and the triangle normal are in ellipsoid space
	// and that e_Position e_Velocity, and e_normalizedVelocity are defined in ellipsoid space
	// In other words, this function checks for a collision between a SPHERE and a triangle,
	// not an ellipsoid and a triangle. Because of this, the results from this function
	// (specifically cP.nearestDistance and cP.intersectionPoint) are in ellipsoid space

	// Check to see if triangle is facing velocity vector
	// We will not triangle facing away from the velocity vector to speed this up
	// since we assume that we will never run into the back face of triangles
	float facing = XMVectorGetX(XMVector3Dot(triNormal, cP.e_normalizedVelocity));
	if (facing <= 0)
	{
		// Create these because cP.e_Velocity and cP.e_Position add slightly to the difficulty
		// of reading the equations
		XMVECTOR velocity = cP.e_Velocity;
		XMVECTOR position = cP.e_Position;

		// t0 and t1 hold the time it takes along the velocity vector that the sphere (called a swept sphere)
		// will "collide" (resting on or touching), once on the front side of the triangle (t0), and once on the
		// backside after it goes "through" the triangle (t1) (or vertex or edge).
		float t0, t1;

		// If sphere is in the plane, it will not intersect with the center of the triangle
		// but instead possibly intersect with one of the vertices or edges first
		bool sphereInPlane = false;

		// Find the plane equation in which the triangle lies in (Ax + By + Cz + D = 0)
		// A, B, and C are the planes normal, x, y, and z respectively
		// We can find D (a.k.a the plane constant) using some simple algebra, which we will do below
		// x, y, and z in the equation defines a point in the plane. Any point in the plane
		// will do, so we will just use p0

		// First the point in the plane
		float x = XMVectorGetX(p0);
		float y = XMVectorGetY(p0);
		float z = XMVectorGetZ(p0);

		// Next the planes normal
		float A = XMVectorGetX(triNormal);
		float B = XMVectorGetY(triNormal);
		float C = XMVectorGetZ(triNormal);

		// Lets solve for D
		// step 1: 0 = Ax + By + Cz + D
		// step 2: subtract D from both sides
		//            -D = Ax + By + Cz
		// setp 3: multiply both sides by -1
		//            -D*-1 = -1 * (Ax + By + Cz)
		// final answer: D = -(Ax + By + Cz)
		float D = -((A*x) + (B*y) + (C*z));

		// To keep the variable names clear, we will rename D to planeConstant
		float planeConstant = D;

		// Get the signed distance from the cameras position (or object if you are using an object)
		// We can get the signed distance between a point and plane with the equation:
		// SignedDistance = PlaneNormal * Point + PlaneConstant 

		// I've mentioned this before, but i'll do it again. When using xna math library vector function
		// that return a scalar value (like a float) such as "XMVector3Dot", an XMVECTOR is returned, with
		// all elements (x,y,z,w) containing that scalar value. We need to extract one, and any will do since
		// they are all the same, so we extract the x component using "XMVectorGetX"
		float signedDistFromPositionToTriPlane = XMVectorGetX(XMVector3Dot(position, triNormal)) + planeConstant;

		// This will be used a couple times below, so we'll just calculate and store it now
		float planeNormalDotVelocity = XMVectorGetX(XMVector3Dot(triNormal, velocity));

		/////////////////////////////////////Sphere Plane Collision Test////////////////////////////////////////////
		// Check to see if the velocity vector is parallel with the plane
		if (planeNormalDotVelocity == 0.0f)
		{
			if (fabs(signedDistFromPositionToTriPlane) >= 1.0f)
			{
				// sphere not in plane, and velocity is
				// parallel to plane, no collision possible
				return false;
			}
			else
			{
				// sphere is in the plane, so we will now only test for a collision
				// with the triangle's vertices and edges
				// Set sphereInPlane to true so we do not do the operation
				// which will divide by zero if the velocity and plane are parallel
				sphereInPlane = true;
			}
		}
		else
		{
			// We know the velocity vector at some point intersects with the plane, we just
			// need to find how far down the velocity vector the sphere will "touch" or rest
			// on the plane. t0 is when it first touches the plane (front side of sphere touches)
			// and t1 is when the back side of the sphere touches.

			// To find when (the time or how far down the velocity vector) the "velocity vector" itself
			//intersects with the plane, we use the equation: (* stands for a dot product)
			// t = (PlaneNormal * Point + PlaneConstant) / (PlaneNormal * Velocity);
			// We have already calculated both sides of the divide sign "/", so:
			// t = signedDistance / normalDotVelocity;

			// Now remember we are working with a unit sphere (since everything has been moved from
			// the usual space to our ellipsoid space). The unit sphere means that the distance from
			// the center of the sphere to ANYWHERE on it's surface is "1". We are not interested in
			// finding when the actual velocity vector intersects with the plane, but instead when
			// the surface of the sphere "touches" the surface of the plane. We know that the distance
			// from the center of the sphere is "1", so all we have to do to find when the sphere touches
			// the plane is subtract and subtract 1 from the signed distance to get when both sides of the
			// sphere touch the plane (t0, and t1)
			t0 = (1.0f - signedDistFromPositionToTriPlane) / planeNormalDotVelocity;
			t1 = (-1.0f - signedDistFromPositionToTriPlane) / planeNormalDotVelocity;

			// We will make sure that t0 is smaller than t1, which means that t0 is when the sphere FIRST
			// touches the planes surface
			if (t0 > t1)
			{
				float temp = t0;
				t0 = t1;
				t1 = temp;
			}

			// If the swept sphere touches the plane outside of the 0 to 1 "timeframe", we know that
			// the sphere is not going to intersect with the plane (and of course triangle) this frame
			if (t0 > 1.0f || t1 < 0.0f)
			{
				return false;
			}

			// If t0 is smaller than 0 then we will make it 0
			// and if t1 is greater than 1 we will make it 1
			if (t0 < 0.0) t0 = 0.0;
			if (t1 > 1.0) t1 = 1.0;
		}

		////////////////////////////////Sphere-(Inside Triangle) Collision Test///////////////////////////////////////
		// If we've made it this far, we know that the sphere will intersect with the triangles plane
		// This frame, so now we will check to see if the collision happened INSIDE the triangle
		XMVECTOR collisionPoint;        // Point on plane where collision occured
		bool collidingWithTri = false;    // This is set so we know if there was a collision with the CURRENT triangle
		float t = 1.0;                    // Time 

										  // If the sphere is not IN the triangles plane, we continue the sphere to inside of triangle test
		if (!sphereInPlane)
		{
			// We get the point on the triangles plane where the sphere "touches" the plane
			// using the equation: planeIntersectionPoint = (Position - Normal) + t0 * Velocity
			// Where t0 is the distance down the velocity vector that the sphere first makes
			// contact with the plane
			XMVECTOR planeIntersectionPoint = (position + t0 * velocity - triNormal);

			// Now we call the function that checks if a point on a triangle's plane is inside the triangle
			if (checkPointInTriangle(planeIntersectionPoint, p0, p1, p2))
			{
				// If the point on the plane IS inside the triangle, we know that the sphere is colliding
				// with the triangle now, so we set collidingWithTri to true so we don't do all the extra
				// calculations on the triangle. We set t to t0, which is the time (or distance) down
				// the velocity vector that the sphere first makes contact, then we set the point of 
				// collision, which will be used later for our collision response
				collidingWithTri = true;
				t = t0;
				collisionPoint = planeIntersectionPoint;
			}
		}

		/////////////////////////////////////Sphere-Vertex Collision Test//////////////////////////////////////////////
		// If the sphere is not colliding with the triangles INSIDE, we check to see if it will collide with one of
		// the vertices of the triangle using the sweep test we did above, but this time check for each vertex instead
		// of the triangles plane
		if (collidingWithTri == false)
		{
			// We will be working with the quadratic function "At^2 + Bt + C = 0" to find when (t) the "swept sphere"s center
			// is 1 unit (spheres radius) away from the vertex position. Remember the swept spheres position is actually a line defined
			// by the spheres position and velocity. t represents it's position along the velocity vector.
			// a = sphereVelocityLength * sphereVelocityLength
			// b = 2(sphereVelocity . (spherePosition - vertexPosition))    // . denotes dot product
			// c = (vertexPosition - spherePosition)^2 - 1
			// This equation allows for two solutions. One is when the sphere "first" touches the vertex, and the other is when
			// the "other" side of the sphere touches the vertex on it's way past the vertex. We need the first "touch"
			float a, b, c; // Equation Parameters

						   // We can use the squared velocities length below when checking for collisions with the edges of the triangles
						   // to, so to keep things clear, we won't set a directly
			float velocityLengthSquared = XMVectorGetX(XMVector3Length(velocity));
			velocityLengthSquared *= velocityLengthSquared;

			// We'll start by setting 'a', since all 3 point equations use this 'a'
			a = velocityLengthSquared;

			// This is a temporary variable to hold the distance down the velocity vector that
			// the sphere will touch the vertex.
			float newT;

			// P0 - Collision test with sphere and p0
			b = 2.0f * (XMVectorGetX(XMVector3Dot(velocity, position - p0)));
			c = XMVectorGetX(XMVector3Length((p0 - position)));
			c = (c*c) - 1.0f;
			if (getLowestRoot(a, b, c, t, &newT)) {    // Check if the equation can be solved
													   // If the equation was solved, we can set a couple things. First we set t (distance
													   // down velocity vector the sphere first collides with vertex) to the temporary newT,
													   // Then we set collidingWithTri to be true so we know there was for sure a collision
													   // with the triangle, then we set the exact point the sphere collides with the triangle,
													   // which is the position of the vertex it collides with
				t = newT;
				collidingWithTri = true;
				collisionPoint = p0;
			}

			// P1 - Collision test with sphere and p1
			b = 2.0*(XMVectorGetX(XMVector3Dot(velocity, position - p1)));
			c = XMVectorGetX(XMVector3Length((p1 - position)));
			c = (c*c) - 1.0;
			if (getLowestRoot(a, b, c, t, &newT)) {
				t = newT;
				collidingWithTri = true;
				collisionPoint = p1;
			}

			// P2 - Collision test with sphere and p2
			b = 2.0*(XMVectorGetX(XMVector3Dot(velocity, position - p2)));
			c = XMVectorGetX(XMVector3Length((p2 - position)));
			c = (c*c) - 1.0;
			if (getLowestRoot(a, b, c, t, &newT)) {
				t = newT;
				collidingWithTri = true;
				collisionPoint = p2;
			}

			//////////////////////////////////////////////Sphere-Edge Collision Test//////////////////////////////////////////////
			// Even though there might have been a collision with a vertex, we will still check for a collision with an edge of the
			// triangle in case an edge was hit before the vertex. Again we will solve a quadratic equation to find where (and if)
			// the swept sphere's position is 1 unit away from the edge of the triangle. The equation parameters this time are a 
			// bit more complex: (still "Ax^2 + Bx + C = 0")
			// a = edgeLength^2 * -velocityLength^2 + (edge . velocity)^2
			// b = edgeLength^2 * 2(velocity . spherePositionToVertex) - 2((edge . velocity)(edge . spherePositionToVertex))
			// c =  edgeLength^2 * (1 - spherePositionToVertexLength^2) + (edge . spherePositionToVertex)^2
			// . denotes dot product

			// Edge (p0, p1):
			XMVECTOR edge = p1 - p0;
			XMVECTOR spherePositionToVertex = p0 - position;
			float edgeLengthSquared = XMVectorGetX(XMVector3Length(edge));
			edgeLengthSquared *= edgeLengthSquared;
			float edgeDotVelocity = XMVectorGetX(XMVector3Dot(edge, velocity));
			float edgeDotSpherePositionToVertex = XMVectorGetX(XMVector3Dot(edge, spherePositionToVertex));
			float spherePositionToVertexLengthSquared = XMVectorGetX(XMVector3Length(spherePositionToVertex));
			spherePositionToVertexLengthSquared = spherePositionToVertexLengthSquared * spherePositionToVertexLengthSquared;

			// Equation parameters
			a = edgeLengthSquared * -velocityLengthSquared + (edgeDotVelocity * edgeDotVelocity);
			b = edgeLengthSquared * (2.0f * XMVectorGetX(XMVector3Dot(velocity, spherePositionToVertex))) - (2.0f * edgeDotVelocity * edgeDotSpherePositionToVertex);
			c = edgeLengthSquared * (1.0f - spherePositionToVertexLengthSquared) + (edgeDotSpherePositionToVertex * edgeDotSpherePositionToVertex);

			// We start by finding if the swept sphere collides with the edges "infinite line"
			if (getLowestRoot(a, b, c, t, &newT)) {
				// Now we check to see if the collision happened between the two vertices that make up this edge
				// We can calculate where on the line the collision happens by doing this:
				// f = (edge . velocity)newT - (edge . spherePositionToVertex) / edgeLength^2
				// if f is between 0 and 1, then we know the collision happened between p0 and p1
				// If the collision happened at p0, the f = 0, if the collision happened at p1 then f = 1
				float f = (edgeDotVelocity * newT - edgeDotSpherePositionToVertex) / edgeLengthSquared;
				if (f >= 0.0f && f <= 1.0f) {
					// If the collision with the edge happened, we set the results
					t = newT;
					collidingWithTri = true;
					collisionPoint = p0 + f * edge;
				}
			}

			// Edge (p1, p2):
			edge = p2 - p1;
			spherePositionToVertex = p1 - position;
			edgeLengthSquared = XMVectorGetX(XMVector3Length(edge));
			edgeLengthSquared = edgeLengthSquared * edgeLengthSquared;
			edgeDotVelocity = XMVectorGetX(XMVector3Dot(edge, cP.e_Velocity));
			edgeDotSpherePositionToVertex = XMVectorGetX(XMVector3Dot(edge, spherePositionToVertex));
			spherePositionToVertexLengthSquared = XMVectorGetX(XMVector3Length(spherePositionToVertex));
			spherePositionToVertexLengthSquared = spherePositionToVertexLengthSquared * spherePositionToVertexLengthSquared;

			a = edgeLengthSquared * -velocityLengthSquared + (edgeDotVelocity * edgeDotVelocity);
			b = edgeLengthSquared * (2.0f * XMVectorGetX(XMVector3Dot(velocity, spherePositionToVertex))) - (2.0f * edgeDotVelocity * edgeDotSpherePositionToVertex);
			c = edgeLengthSquared * (1.0f - spherePositionToVertexLengthSquared) + (edgeDotSpherePositionToVertex * edgeDotSpherePositionToVertex);

			if (getLowestRoot(a, b, c, t, &newT)) {
				float f = (edgeDotVelocity * newT - edgeDotSpherePositionToVertex) / edgeLengthSquared;
				if (f >= 0.0f && f <= 1.0f) {
					t = newT;
					collidingWithTri = true;
					collisionPoint = p1 + f * edge;
				}
			}

			// Edge (p2, p0):
			edge = p0 - p2;
			spherePositionToVertex = p2 - position;
			edgeLengthSquared = XMVectorGetX(XMVector3Length(edge));
			edgeLengthSquared = edgeLengthSquared * edgeLengthSquared;
			edgeDotVelocity = XMVectorGetX(XMVector3Dot(edge, velocity));
			edgeDotSpherePositionToVertex = XMVectorGetX(XMVector3Dot(edge, spherePositionToVertex));
			spherePositionToVertexLengthSquared = XMVectorGetX(XMVector3Length(spherePositionToVertex));
			spherePositionToVertexLengthSquared = spherePositionToVertexLengthSquared * spherePositionToVertexLengthSquared;

			a = edgeLengthSquared * -velocityLengthSquared + (edgeDotVelocity * edgeDotVelocity);
			b = edgeLengthSquared * (2.0f * XMVectorGetX(XMVector3Dot(velocity, spherePositionToVertex))) - (2.0f * edgeDotVelocity * edgeDotSpherePositionToVertex);
			c = edgeLengthSquared * (1.0f - spherePositionToVertexLengthSquared) + (edgeDotSpherePositionToVertex * edgeDotSpherePositionToVertex);

			if (getLowestRoot(a, b, c, t, &newT)) {
				float f = (edgeDotVelocity * newT - edgeDotSpherePositionToVertex) / edgeLengthSquared;
				if (f >= 0.0f && f <= 1.0f) {
					t = newT;
					collidingWithTri = true;
					collisionPoint = p2 + f * edge;
				}
			}
		}

		// If we have found a collision, we will set the results of the collision here
		if (collidingWithTri == true)
		{
			// We find the distance to the collision using the time variable (t) times the length of the velocity vector
			float distToCollision = t * XMVectorGetX(XMVector3Length(velocity));

			// Now we check if this is the first triangle that has been collided with OR it is 
			// the closest triangle yet that was collided with
			if (cP.foundCollision == false || distToCollision < cP.nearestDistance) {

				// Collision response information (used for "sliding")
				cP.nearestDistance = distToCollision;
				cP.intersectionPoint = collisionPoint;

				// Make sure this is set to true if we've made it this far
				cP.foundCollision = true;
				return true;
			}
		}
	}
	return false;
}

bool checkPointInTriangle(const XMVECTOR& point, const XMVECTOR& triV1, const XMVECTOR& triV2, const XMVECTOR& triV3)
{
	//Denna funktion kolla om en point är i en triangle genom att
	XMVECTOR collisionPoint1 = XMVector3Cross((triV3 - triV2), (point - triV2));
	XMVECTOR collisionPoint2 = XMVector3Cross((triV3 - triV2), (triV1 - triV2));
	if (XMVectorGetX(XMVector3Dot(collisionPoint1, collisionPoint2)) >= 0)
	{
		collisionPoint1 = XMVector3Cross((triV3 - triV1), (point - triV1));
		collisionPoint2 = XMVector3Cross((triV3 - triV1), (triV2 - triV1));
		if (XMVectorGetX(XMVector3Dot(collisionPoint1, collisionPoint2)) >= 0)
		{
			collisionPoint1 = XMVector3Cross((triV2 - triV1), (point - triV1));
			collisionPoint2 = XMVector3Cross((triV2 - triV1), (triV3 - triV1));
			if (XMVectorGetX(XMVector3Dot(collisionPoint1, collisionPoint2)) >= 0)
			{
				return true;
			}
		}
	}
	return false;
}

// This function solves the quadratic eqation "At^2 + Bt + C = 0" and is found in Kasper Fauerby's paper on collision detection and response
bool getLowestRoot(float a, float b, float c, float maxR, float* root)
{
	// Vi börjar genom att kolla om det finns en lösning
	// Om determinanten är negativ så finns det inga lösningar och därför returnar vi false.
	float determinant = b*b - 4.0f*a*c;

	if (determinant < 0.0f) return false;
	// calculate the two roots: (if determinant == 0 then
	// x1==x2 but lets disregard that slight optimization)
	float sqrtD = sqrt(determinant);
	float r1 = (-b - sqrtD) / (2 * a);
	float r2 = (-b + sqrtD) / (2 * a);
	// Sort so x1 <= x2
	if (r1 > r2) {
		float temp = r2;
		r2 = r1;
		r1 = temp;
	}
	// Get lowest root:
	if (r1 > 0 && r1 < maxR) {
		*root = r1;
		return true;
	}
	// It is possible that we want x2 - this can happen
	// if x1 < 0
	if (r2 > 0 && r2 < maxR) {
		*root = r2;
		return true;
	}

	// No (valid) solutions
	return false;
}
#pragma endregion

#pragma region Picking

void calcRay(float mouseX, float mouseY, XMVECTOR& rayOrigin, XMVECTOR& rayDirection)
{
	XMVECTOR localRayDirection = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR LocalRayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	//XMVECTOR LocalRayOrigin		 = matrices.camPos;

	float X, Y, Z;
	XMFLOAT4X4 camProjection;
	XMStoreFloat4x4(&camProjection, matrices.Projection);

	//Transform 2D pick position on screen space to 3D ray in View space
	X = (((2.0f * mouseX) / WIN_WIDTH) - 1) / camProjection._11;
	Y = -(((2.0f * mouseY) / WIN_HEIGHT) - 1) / camProjection._22;
	Z = 1.0f;    //View space's Z direction ranges from 0 to 1, so we set 1 since the ray goes "into" the screen

				 //localRayDirection = XMVectorSet(X, Y, Z, 0.0f);
	localRayDirection = XMVectorSet(0.0f, 0.0f, Z, 0.0f);

	XMMATRIX inverseCamView;
	XMVECTOR det = { 1,1,1,1 };
	XMMATRIX temp = XMMatrixTranspose(matrices.camView);
	inverseCamView = XMMatrixInverse(&det, temp);

	rayOrigin = XMVector3TransformCoord(LocalRayOrigin, inverseCamView);
	//rayOrigin = matrices.camPos;
	rayDirection = XMVector3TransformNormal(localRayDirection, inverseCamView);
	rayDirection = XMVector3Normalize(rayDirection);
}

float Intersection(XMVECTOR rayOrigin, XMVECTOR rayDirection)
{
	for (int i = 0; i < obj.face_idxs.size() / 3; i++)
	{

		//Triangle's vertices, V1, V2, V3
		XMVECTOR tri1V1 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR tri1V2 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR tri1V3 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);


		//Temporary 3d floats for each vertex.
		XMFLOAT3 _tV1, _tV2, _tV3;

		_tV1.x = obj.finalVector[i * 3 + 0].x;
		_tV1.y = obj.finalVector[i * 3 + 0].y;
		_tV1.z = obj.finalVector[i * 3 + 0].z;

		_tV2.x = obj.finalVector[i * 3 + 1].x;
		_tV2.y = obj.finalVector[i * 3 + 1].y;
		_tV2.z = obj.finalVector[i * 3 + 1].z;

		_tV3.x = obj.finalVector[i * 3 + 2].x;
		_tV3.y = obj.finalVector[i * 3 + 2].y;
		_tV3.z = obj.finalVector[i * 3 + 2].z;


		tri1V1 = XMVectorSet(_tV1.x, _tV1.y, _tV1.z, 1.0f);
		tri1V2 = XMVectorSet(_tV2.x, _tV2.y, _tV2.z, 1.0f);
		tri1V3 = XMVectorSet(_tV3.x, _tV3.y, _tV3.z, 1.0f);

		tri1V1 = XMVector3TransformCoord(tri1V1, matrices.World);
		tri1V2 = XMVector3TransformCoord(tri1V2, matrices.World);
		tri1V3 = XMVector3TransformCoord(tri1V3, matrices.World);


		XMVECTOR U = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR V = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		XMVECTOR faceNormal = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		U = tri1V2 - tri1V1;
		V = tri1V3 - tri1V1;
		faceNormal = XMVector3Cross(U, V);
		faceNormal = XMVector3Normalize(faceNormal);

		XMVECTOR triPoint = tri1V1;

		//plane equation
		float tri1A = XMVectorGetX(faceNormal);
		float tri1B = XMVectorGetY(faceNormal);
		float tri1C = XMVectorGetZ(faceNormal);
		float tri1D = (-tri1A*XMVectorGetX(triPoint) - tri1B*XMVectorGetY(triPoint) - tri1C*XMVectorGetZ(triPoint));

		//now we find wher the ray intersects with the triangles plane,

		float ep1, ep2, t = 0.0f;

		float planeIntersectX, planeIntersectY, planeIntersectz = 0.0f;


		XMVECTOR pointInPlane = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		ep1 = (XMVectorGetX(rayOrigin)		*	tri1A) + (XMVectorGetY(rayOrigin)		*	tri1B) + (XMVectorGetZ(rayOrigin)		*	tri1C);
		ep2 = (XMVectorGetX(rayDirection)	*	tri1A) + (XMVectorGetY(rayDirection)	*	tri1B) + (XMVectorGetZ(rayDirection)	*	tri1C);

		if (ep2 != 0.0f)
		{
			t = -(ep1 + tri1D) / (ep2);
		}
		if (t > 0.0f)
		{
			planeIntersectX = XMVectorGetX(rayOrigin) + XMVectorGetX(rayDirection) * t;
			planeIntersectY = XMVectorGetY(rayOrigin) + XMVectorGetY(rayDirection) * t;
			planeIntersectz = XMVectorGetZ(rayOrigin) + XMVectorGetZ(rayDirection) * t;

			pointInPlane = XMVectorSet(planeIntersectX, planeIntersectY, planeIntersectz, 0.0f);

			if (PointInTriangle(tri1V1, tri1V2, tri1V3, pointInPlane))
			{
				return t / 2.0f;
			}
		}


	}
	return FLT_MAX;
}

bool PointInTriangle(XMVECTOR&triV1, XMVECTOR&triV2, XMVECTOR&triV3, XMVECTOR&point)
{
	XMVECTOR cp1 = XMVector3Cross((triV3 - triV2), (point - triV2));
	XMVECTOR cp2 = XMVector3Cross((triV3 - triV2), (triV1 - triV2));
	if (XMVectorGetX(XMVector3Dot(cp1, cp2)) >= 0)
	{
		cp1 = XMVector3Cross((triV3 - triV1), (point - triV1));
		cp2 = XMVector3Cross((triV3 - triV1), (triV2 - triV1));
		if (XMVectorGetX(XMVector3Dot(cp1, cp2)) >= 0)
		{
			cp1 = XMVector3Cross((triV2 - triV1), (point - triV1));
			cp2 = XMVector3Cross((triV2 - triV1), (triV3 - triV1));
			if (XMVectorGetX(XMVector3Dot(cp1, cp2)) >= 0)
			{
				return true;
			}
			else
				return false;
		}
		else
			return false;
	}
	return false;

}

#pragma endregion

void initParticle()
{
	//vertexList.push_back(VertexType{ 1.0f, 1.0f, 1.0f, 0.2f, 0.2f, 1.0f, 0.0f, 0.0f });

	D3D11_BUFFER_DESC bufferdesc;
	std::memset(&bufferdesc, 0, sizeof(bufferdesc));
	bufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferdesc.ByteWidth = sizeof(BillboardData) * maxParticles;
	bufferdesc.MiscFlags = 0;
	bufferdesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = vertexList.data();
	HRESULT hr = gDevice->CreateBuffer(&bufferdesc, &data, &gVertexBufferParticle);

}

void EmitParticles()
{
	particleDeviationX = 20.0f;
	particleDeviationY = 2.0f;
	particleDeviationZ = 20.0f;
	maxParticles = 2000;
	XMFLOAT3 wPos = { 0.0f,0.0f,0.0f };
	XMFLOAT3 dir = { 0.0f,1.0f,0.0f };
	XMFLOAT3 color = { 0.0f,0.0f,0.0f };
	
	float positionX, positionY, positionZ, red, green, blue;
	//int index, i, j;
	int i = 0;

	for (int i = 0; i < maxParticles; i++)
	{
		positionX = (((float)rand() - (float)rand()) / RAND_MAX) * particleDeviationX;
		positionY = (((float)rand() - (float)rand()) / RAND_MAX) * particleDeviationY;
		positionZ = (((float)rand() - (float)rand()) / RAND_MAX) * particleDeviationZ;

		vertexList.at(i).worldPos = wPos;
		vertexList.at(i).direction = dir;
		vertexList.at(i).color = color;
		vertexList.at(i).height = positionX;
		vertexList.at(i).width = 10.0f;
		vertexList.at(i).depth = positionZ;
			
		//vertexList.push_back(BillboardData{wPos.x,wPos.y,wPos.z,dir,color,positionX,10.0f,positionZ,color });

	}
	/*while (i < maxParticles)
	{
		//generate randomized particle properties.
		positionX = (((float)rand() - (float)rand()) / RAND_MAX) * particleDeviationX;
		positionY = (((float)rand() - (float)rand()) / RAND_MAX) * particleDeviationY;
		positionZ = (((float)rand() - (float)rand()) / RAND_MAX) * particleDeviationZ;

		//velocity = particleVelocity + (((float)rand() - (float)rand()) / RAND_MAX) * particleVelocityVariation;

		red = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		green = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		blue = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;

		
		velocity.push_back(((float)rand() / (RAND_MAX + 1) + 1 + (rand() % 3)) / 5.0f);
		i++;
	}*/


}

void UpdateBuffers()
{
	for (int i = 0; i < vertexList.size() - 1; i++)
	{

		if (vertexList[i].width >= 0)
		{
			vertexList[i].width = (vertexList[i].width - velocity[i]);

		}
		else
		{
			vertexList[i].width = 10;
		}
	}

	

}

void Update()
{

	gDeviceContext->UpdateSubresource(gConstantBuffer, 0, 0, &matrices, 0, 0);
	gDeviceContext->UpdateSubresource(gConstantLightBuffer, 0, 0, &Lights, 0, 0);

	gDeviceContext->GSSetConstantBuffers(0, 1, &gConstantBuffer);
	gDeviceContext->VSSetConstantBuffers(0, 1, &gConstantBuffer);
	gDeviceContext->PSSetConstantBuffers(0, 1, &gConstantBuffer);
	gDeviceContext->GSSetConstantBuffers(1, 1, &gConstantLightBuffer);
	gDeviceContext->VSSetConstantBuffers(1, 1, &gConstantLightBuffer);
	gDeviceContext->PSSetConstantBuffers(1, 1, &gConstantLightBuffer);
}

#pragma region Renders
void RenderShadow()
{
	SetViewportShadow();
	gDeviceContext->ClearDepthStencilView(gShadowDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	gDeviceContext->VSSetShader(gVertexShaderShadow, nullptr, 0);
	gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	gDeviceContext->DSSetShader(nullptr, nullptr, 0);
	gDeviceContext->GSSetShader(nullptr, nullptr, 0);
	gDeviceContext->PSSetShader(nullptr, nullptr, 0);

	UINT32 vertexSize = sizeof(obj.finalVector[0]);
	UINT32 vertexCount = obj.finalVector.size();
	UINT32 indexSize = obj.index_counter;
	UINT32 offset = 0;

	gDeviceContext->IASetVertexBuffers(0, 1, &gVertexBuffer, &vertexSize, &offset);
	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gDeviceContext->IASetInputLayout(GBuffer_VertexLayout);

	gDeviceContext->OMSetRenderTargets(0, NULL, gShadowDepthStencilView);

	gDeviceContext->Draw(vertexCount, 0);

	SetViewport();
	gDeviceContext->OMSetRenderTargets(1, &gBackBufferRTV, gDepthStencilView);
}

void RenderBillBoards()
{
	//gDeviceContext->ClearDepthStencilView(gDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	
	gDeviceContext->VSSetShader(gVertexShaderParticle, nullptr, 0);
	gDeviceContext->GSSetShader(gGeometryShaderParticle, nullptr, 0);
	gDeviceContext->PSSetShader(gPixelShaderParticle, nullptr, 0);

	D3D11_MAPPED_SUBRESOURCE gMappedResource;
	BillboardData* vtxPtr;
	HRESULT hr = gDeviceContext->Map(gVertexBufferParticle, 0, D3D11_MAP_WRITE_DISCARD, 0, &gMappedResource);

	vtxPtr = (BillboardData*)gMappedResource.pData;
	memcpy(vtxPtr, vertexList.data(), sizeof(BillboardData) * maxParticles);

	gDeviceContext->Unmap(gVertexBufferParticle, 0);


	/* KOLLAR ÖVER DENNA */
	UINT32 vertexSize = sizeof(BillboardData);
	UINT32 offset = 0;

	gDeviceContext->IASetVertexBuffers(0, 1, &gVertexBufferParticle, &vertexSize, &offset);
	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	gDeviceContext->Draw((vertexList.size() - 1), 0);
}

void RenderExplosion()
{
	gDeviceContext->VSSetShader(gVertexShaderParticle, nullptr, 0);
	gDeviceContext->GSSetShader(gGeometryShaderParticle, nullptr, 0);
	gDeviceContext->PSSetShader(gPixelShaderParticle, nullptr, 0);

	UINT32 vertexSize = sizeof(BillboardData);
	UINT32 offset = 0;

	gDeviceContext->IASetVertexBuffers(0, 1, &gVertexBufferExplosion, &vertexSize, &offset);
	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	gDeviceContext->Draw((explosionTest.size() - 1), 0);
	
	
}

void Render()
{
	gDeviceContext->ClearRenderTargetView(gBackBufferRTV, clearColor);
	gDeviceContext->ClearDepthStencilView(gDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	gDeviceContext->VSSetShader(gVertexShader, nullptr, 0);
	gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	gDeviceContext->DSSetShader(nullptr, nullptr, 0);
	gDeviceContext->GSSetShader(gGeometryShader, nullptr, 0);
	gDeviceContext->PSSetShader(gPixelShader, nullptr, 0);

	UINT32 vertexSize = sizeof(obj.finalVector[0]);
	UINT32 vertexCount = obj.finalVector.size();
	UINT32 indexSize = obj.index_counter;
	UINT32 offset = 0;

	gDeviceContext->IASetVertexBuffers(0, 1, &gVertexBuffer, &vertexSize, &offset);
	gDeviceContext->IASetIndexBuffer(gIndexBuffer, DXGI_FORMAT_R32_UINT, 0); // sets the index buffer

	gDeviceContext->IASetVertexBuffers(0, 1, &groundVertexBuffer, &groundVertexSize, &offset);

	gDeviceContext->IASetIndexBuffer(groundIndexBuffer, DXGI_FORMAT_R32_UINT, 0); // sets the index buffer

	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gDeviceContext->IASetInputLayout(gVertexLayout);

	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gDeviceContext->IASetInputLayout(gVertexLayout);

	gDeviceContext->PSSetShaderResources(1, 1, &ShadowDepthResource);
	//gDeviceContext->PSSetSamplers(0, 1, &texSamplerState);


	/************************************************************
	****************************DRAW****************************
	************************************************************/

	for (int i = 0; i < (obj.drawOffset.size() - 1); i++)
	{
		/*if (n < textureResources.size())
		{
		n++;
		}*/
		if (i < textureResources.size())
		{
			gDeviceContext->PSSetShaderResources(0, 1, &textureResources[i]);

		}

		if (i < normalResources.size())
		{
			gDeviceContext->PSSetShaderResources(2, 1, &normalResources[i]);
		}
		gDeviceContext->Draw((obj.drawOffset[(i + 1)] - obj.drawOffset[i]), obj.drawOffset[i]);

	}
	gDeviceContext->DrawIndexed(NumFaces * 3, 0, 0);

}

void RenderGBuffer2()
{
	ID3D11RenderTargetView* rtvsToSet[] = {
		textureRTVs[0],
		textureRTVs[1],
		textureRTVs[2],
		textureRTVs[3],
	};
	gDeviceContext->OMSetRenderTargets(numRTVs, rtvsToSet, GBufferDepthStencilView);

	gDeviceContext->VSSetShader(gVertexShader, nullptr, 0);
	gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	gDeviceContext->DSSetShader(nullptr, nullptr, 0);
	gDeviceContext->GSSetShader(nullptr, nullptr, 0);
	gDeviceContext->PSSetShader(gPixelShader, nullptr, 0);

	UINT offset2 = 0;
	UINT stride = sizeof(Vertex);

	gDeviceContext->IASetIndexBuffer(groundIndexBuffer, DXGI_FORMAT_R32_UINT, 0); // sets the index buffer
	gDeviceContext->IASetVertexBuffers(0, 1, &groundVertexBuffer, &stride, &offset2);

	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gDeviceContext->IASetInputLayout(GBuffer_VertexLayout);

	gDeviceContext->PSSetSamplers(0, 1, &pointSamplerState);

	gDeviceContext->PSSetShaderResources(2, 1, &ShadowDepthResource);
	gDeviceContext->PSSetShaderResources(0, 1, &heightResource);

	gDeviceContext->DrawIndexed(NumFaces * 3, 0, 0);
}

void RenderTriangle()
{
	ID3D11RenderTargetView* rtvsToSet[] = {
		textureRTVs[0],
		textureRTVs[1],
		textureRTVs[2],
		textureRTVs[3],
	};
	gDeviceContext->OMSetRenderTargets(numRTVs, rtvsToSet, GBufferDepthStencilView);

	gDeviceContext->VSSetShader(gVertexShader, nullptr, 0);
	gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	gDeviceContext->DSSetShader(nullptr, nullptr, 0);
	gDeviceContext->GSSetShader(gGeometryShader, nullptr, 0);
	gDeviceContext->PSSetShader(gPixelShader, nullptr, 0);

	UINT offset2 = 0;
	UINT vertexSize = sizeof(float) * 5;

	gDeviceContext->IASetVertexBuffers(0, 1, &triBuffer, &vertexSize, &offset2);
	gDeviceContext->IASetIndexBuffer(gIndexBuffer, DXGI_FORMAT_R32_UINT, 0); // sets the index buffer


	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gDeviceContext->IASetInputLayout(gVertexLayout);

	gDeviceContext->PSSetSamplers(0, 1, &pointSamplerState);

	gDeviceContext->Draw(6, 0);
}

void RenderGBuffer()
{
	for (int i = 0; i < numRTVs; i++)
		gDeviceContext->ClearRenderTargetView(textureRTVs[i], clearColor);
	gDeviceContext->ClearDepthStencilView(GBufferDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	ID3D11RenderTargetView* rtvsToSet[] = {
		textureRTVs[0],
		textureRTVs[1],
		textureRTVs[2],
		textureRTVs[3],
	};
	gDeviceContext->OMSetRenderTargets(numRTVs, rtvsToSet, GBufferDepthStencilView);

	gDeviceContext->VSSetShader(GBuffer_VS, nullptr, 0);
	gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	gDeviceContext->DSSetShader(nullptr, nullptr, 0);
	gDeviceContext->GSSetShader(GBuffer_GS, nullptr, 0);
	gDeviceContext->PSSetShader(GBuffer_PS, nullptr, 0);

	UINT32 vertexSize = sizeof(obj.finalVector[0]);
	UINT32 vertexCount = obj.finalVector.size();
	UINT32 indexSize = obj.index_counter;
	UINT32 offset = 0;

	gDeviceContext->IASetVertexBuffers(0, 1, &gVertexBuffer, &vertexSize, &offset);
	gDeviceContext->IASetIndexBuffer(gIndexBuffer, DXGI_FORMAT_R32_UINT, 0); // sets the index buffer

	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gDeviceContext->IASetInputLayout(GBuffer_VertexLayout);

	gDeviceContext->PSSetSamplers(0, 1, &linearSamplerState);


	gDeviceContext->PSSetShaderResources(2, 1, &ShadowDepthResource);
	/***************************DRAW****************************/

	for (int i = 0; i < (obj.drawOffset.size() - 1); i++)
	{
		if (i < textureResources.size())
		{
			gDeviceContext->PSSetShaderResources(0, 1, &textureResources[i]);

		}

		if (i < normalResources.size())
		{
			gDeviceContext->PSSetShaderResources(1, 1, &normalResources[i]);
		}


		gDeviceContext->Draw((obj.drawOffset[(i + 1)] - obj.drawOffset[i]), obj.drawOffset[i]);

	}

	//gDeviceContext->OMSetRenderTargets(1, &gBackBufferRTV, gDepthStencilView);
}

void RenderFinalPass()
{
	gDeviceContext->OMSetRenderTargets(1, &gBackBufferRTV, gDepthStencilView);

	gDeviceContext->ClearRenderTargetView(gBackBufferRTV, clearColor);
	gDeviceContext->ClearDepthStencilView(gDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);


	gDeviceContext->VSSetShader(FinalPass_VS, nullptr, 0);
	gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	gDeviceContext->DSSetShader(nullptr, nullptr, 0);
	gDeviceContext->GSSetShader(nullptr, nullptr, 0);
	gDeviceContext->PSSetShader(FinalPass_PS, nullptr, 0);

	UINT32 vertexSize = sizeof(float) * 5;
	UINT32 offset = 0;

	gDeviceContext->IASetVertexBuffers(0, 1, &gVertexBufferFinalPass, &vertexSize, &offset);
	gDeviceContext->IASetIndexBuffer(gIndexBuffer, DXGI_FORMAT_R32_UINT, 0); // sets the index buffer

	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gDeviceContext->IASetInputLayout(FinalPass_VertexLayout);


	gDeviceContext->PSSetShaderResources(0, numRTVs, shaderResourceViews);
	gDeviceContext->PSSetShaderResources(numRTVs + 1, 1, &ShadowDepthResource);

	//****************************DRAW****************************

	gDeviceContext->Draw(6, 0);

}
#pragma endregion

// handle of instance   commandline	  how the window is shown
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) // wWinMain Predefined main for directx
{
	//adding a console
	if (AllocConsole())
	{
		HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
		int hCrt = _open_osfhandle((long)handle_out, _O_TEXT);
		FILE* hf_out = _fdopen(hCrt, "w");
		setvbuf(hf_out, NULL, _IONBF, 1);
		*stdout = *hf_out;

		HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
		hCrt = _open_osfhandle((long)handle_in, _O_TEXT);
		FILE* hf_in = _fdopen(hCrt, "r");
		setvbuf(hf_in, NULL, _IONBF, 128);
		*stdin = *hf_in;
	}

	// use the console just like a normal one - printf(), getchar(), ...

	//Initialize window
	MSG msg = { 0 };
	wndHandle = InitWindow(hInstance);						// Skapar fönstret
															//window is valid
	if (!initDirectInput(hInstance))
	{
		MessageBox(0, L"DIRECT INPUT INITILIZATION - FAILED",
			L"ERROR", MB_OK);
		return 0;
	}
	if (wndHandle)
	{
		obj.read();

		CreateDirect3DContext(wndHandle); //2. Skapa och koppla SwapChain, Device och Device Context

		SetViewport();
		//myWindow.SetViewport(gDevice, gDevContext);

		CreateShaders();

		CreateDefferedShaders();
		CreateTextureViews();
		CreateFinalPassShaders();

		ConstantBuffer();
		finalPassQuadData();

		CreateTriangleForBackFace();

		createTriangle();

//		initExplosion();

//		EmitParticles();

		initParticle();

		createTextures();

		CreateHeightMap();

		//Shows the window
		ShowWindow(wndHandle, nCmdShow);

		//Main message loop
		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))	// Peekmessage checks for messages recieved by window and it translate and disbatch the messages and remove them from the stack
			{
				TranslateMessage(&msg);							// This function will translate any virtual keymessages to the window	
				DispatchMessage(&msg);							// Sends those messages out.
			}
			else
			{

				frameCount++;
				if (getTime() > 1.0f)
				{
					fps = frameCount;
					frameCount = 0;
					startTimer();
				}
				Update();

				//UpdateParticles(getFrameTime());
				UpdateBuffers();
				RenderShadow(); // Rendera
				//Render(); // Rendera
				RenderGBuffer(); // Rendera
				RenderGBuffer2(); // Rendera
				RenderTriangle();
				RenderBillBoards(); // Rendera

			/*	if (explosion)
				{
					RenderExplosion(); // Rendera
					updateExplosion();
					killExplosion();
				}*/
				RenderFinalPass();

				frameTime = getFrameTime();

				detectInput(frameTime);

				gSwapChain->Present(1, 0); // Växla front och back buffer
			}
		}

		gVertexBufferFinalPass->Release();
		gConstantLightBuffer->Release();
		gVertexBufferParticle->Release();
		gVertexBufferExplosion->Release();
		groundVertexBuffer->Release();
		groundIndexBuffer->Release();
		gVertexBuffer->Release();
		gIndexBuffer->Release();
		gConstantBuffer->Release();

		gPixelShader->Release();
		gGeometryShader->Release();
		gVertexShader->Release();
		
		GBuffer_GS->Release();
		GBuffer_VS->Release();
		GBuffer_PS->Release();
		FinalPass_PS->Release();
		FinalPass_VS->Release();

		for (size_t i = 0; i < numRTVs; i++)
		{
			textureRTVs[i]->Release();
		}

		triBuffer->Release();
		gRasterState->Release();
		gShadowDepthStencilView->Release();
		ShadowDepthResource->Release();
		gDepthStencilView->Release();
		pointSamplerState->Release();
		texSamplerState->Release();
		linearSamplerState->Release();
		GBufferDepthStencilView->Release();
		//shadowrenderpass
		gVertexShaderShadow->Release();

		gDevice->Release();
		gDeviceContext->Release();
		gSwapChain->Release();
		gBackBufferRTV->Release();
		//gShadowBackBuffer->Release();
		DestroyWindow(wndHandle);

		DIKeyboard->Unacquire();
		DIMouse->Unacquire();
		directInput->Release();
	}
	return (int)msg.wParam;
}

HWND InitWindow(HINSTANCE hInstance)
{
	//Creating the window class
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;					// when its resized in horizontal or vertical it redraw
	wcex.lpfnWndProc = WndProc;								//
	wcex.hInstance = hInstance;								    // assign handle of instance
	wcex.lpszClassName = L"BasicWindow";						// The classname of the window class
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);				// Sets the icon of window
	wcex.hCursor = LoadCursor(NULL, IDC_CROSS);					// Determines cursor of window
	wcex.lpszMenuName = (NULL);									// Menuname null , no menu name
	wcex.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

	//Register window class
	if (!RegisterClassEx(&wcex))
		return false;

	RECT rc = { 0, 0,1280, 720 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	HWND handle = CreateWindow(
		L"BasicWindow",										// Same as the classname on window
		L"GAMING ENGINE",								// Name on top of the window
		WS_OVERLAPPEDWINDOW,								// Contains a bunch of diferent window styles.
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rc.right - rc.left,
		rc.bottom - rc.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	if (!handle)
	{
		return false;
	}

	return handle;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)//Message reciever function
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0); // If pressing cross closing the application
		break;

	case WM_CHAR:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0); // If pressing escape, close the application
			return 0;

		}

	}
	return DefWindowProc(hWnd, message, wParam, lParam); // completing message procedure function.

}

HRESULT CreateDirect3DContext(HWND windowHandle)
{
	DXGI_SWAP_CHAIN_DESC SCD; //Create a struct to hold information about the swap chain

	ZeroMemory(&SCD, sizeof(DXGI_SWAP_CHAIN_DESC)); // clear out the struct for use

	HRESULT WINAPI DirectInput8Create(
		HINSTANCE hinst,							// is the handle to the instance of our application
		DWORD dwVersion,							// this is the version of the direct input we want to use
		REFIID riidltf,								// This is an indentifier to the interface of direct input we want to use
		LPVOID *ppvOut,								// This is the returned pointer to our direct input object
		LPUNKNOWN punkOuter);						// This is used for COM aggregation

													// Fill the swap chain description struct

	SCD.BufferCount = 1;								// One back buffer
	SCD.BufferDesc.RefreshRate.Numerator = 60;			// FPS-cap
	SCD.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Use 32 bit color
	SCD.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  // How swap chain is to be used
	SCD.OutputWindow = windowHandle;						// The window to be used
	SCD.SampleDesc.Count = 4;							// How many multisamples
	SCD.Windowed = false;
	SCD.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	SCD.Windowed = TRUE;

	//Create a device, device context and swap chain using the information in the SCD struct
	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&SCD,
		&gSwapChain,
		&gDevice,
		NULL,
		&gDeviceContext);

	if (SUCCEEDED(hr))
	{
		//Get the adress of the backbuffer
		gSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&gBackBuffer);

		//gSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&gShadowBackBuffer);

		createDepthStencil();

		createLightDepthStencil();

		//use the back buffer adress to create the render target
		gDevice->CreateRenderTargetView(gBackBuffer, NULL, &gBackBufferRTV);
		gBackBuffer->Release();

		//TESTING SHADOWRENDEERTARGET
		//gDevice->CreateRenderTargetView(gBackBuffer, NULL, &gShadowRenderTarget);
		//gShadowBackBuffer->Release();

		//Set the render target as the back buffer
		gDeviceContext->OMSetRenderTargets(1, &gBackBufferRTV, gDepthStencilView);
	}
	return hr;
}