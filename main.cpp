
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

float Intersection(XMVECTOR rayOrigin, XMVECTOR rayDirection,XMMATRIX& worldSpace);
void RenderExplosion();
bool PointInTriangle(XMVECTOR&triV1, XMVECTOR&triV2, XMVECTOR&triV3, XMVECTOR&point);
void Picking(float mouseX, float mouseY, XMVECTOR& pickRayVectorWorldSpacePos, XMVECTOR&pickRayVectorWorldSpaceDir);
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
#pragma endregion

#pragma region camVectors

XMVECTOR camPosition = XMVectorSet(0, 1, -5, 0);
XMVECTOR camTarget = XMVectorSet(0, 0, 0, 0);
XMVECTOR camUp = XMVectorSet(0, 1, 0, 0);

XMVECTOR lightPosition = XMVectorSet(10, 5, -5, 1);
XMVECTOR lightDir = XMVectorSet(0, 0, 0, 0);
XMVECTOR lightUp = XMVectorSet(0, 1, 0, 0);


#pragma endregion

#pragma region init Structs

struct MatrixBuffer {
	XMMATRIX World;
	XMMATRIX camView;
	XMMATRIX Projection;
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

struct VertexType
{
	float x,y,z;
	float h,w;
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

ID3D11Texture2D* gBackBuffer = nullptr;
//ID3D11Texture2D* gShadowBackBuffer = nullptr;

ID3D11RenderTargetView* gBackBufferRTV = nullptr;

//ID3D11RenderTargetView* gShadowRenderTarget = nullptr; //egen

ID3D11DepthStencilView* gDepthStencilView = nullptr;

ID3D11DepthStencilView* gShadowDepthStencilView = nullptr;
ID3D11ShaderResourceView* ShadowDepthResource = nullptr;

D3D11_RASTERIZER_DESC rasterDesc;
ID3D11RasterizerState* gRasterState = nullptr;

#pragma endregion


// INITIALIZE SHADER THINGS

vector<ID3D11ShaderResourceView*> textureResources;
vector<ID3D11ShaderResourceView*> normalResources;

ID3D11SamplerState* texSamplerState;

Importer obj;

// GLOBALS FOR FIRST PERSON CAMERA *********************************

XMVECTOR defaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
XMVECTOR defaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
XMVECTOR camForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
XMVECTOR camRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

XMVECTOR defaultUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
XMVECTOR camUpDown = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

XMMATRIX camRotationMatrix;
XMMATRIX groundWorld;

float moveLeftRight = 0.0f;		// Used to move along the camFoward and camRight vectors
float moveBackForward = 0.0f;   // Used to move along the camFoward and camRight vectors
float moveupDown = 0.0f;

float camYaw = 0.0f;
float camPitch = 0.0f;

float particleSize;

int maxParticles;

vector<VertexType> explosionTest;

vector<VertexType> vertexList;

vector<StartingPos> explosionStartingPos;

vector<float> velocity;

int maxExplosionParticles;
float expDeviationX, expDeviationY, expDeviationZ;
bool explosion = false;
XMVECTOR origin;

float particleDeviationX, particleDeviationY, particleDeviationZ;

//GLOBALS FOR INPUT ************************************************

HWND wndHandle = NULL;


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

int NumFaces = 0; 
int NumVertices = 0; 

//Height map
struct HeightMapInfo {
	int terrainWidth;
	int terrainHeight;
	XMFLOAT3 * heightMap;
};

//bool HeightMapLoad(char* filename, HeightMapInfo &hminfo);

//TEXT
bool beginCheck;
// TIME GLOBALS ****************************************************

double countsPerSecond = 0.0;
__int64 counterStart = 0;

int frameCount = 0;
int fps = 0;

__int64 frameTimeOld = 0;
double frameTime;

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
		{ "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 3, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * 5, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		
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
	
}

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
	float nearZ = 0.01;
	float farZ = 50.0;

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
	matrices.Projection = XMMatrixTranspose(matrices.Projection); // Transposing the projection and view matrices.
	matrices.World = XMMatrixIdentity();              // Setting the world matrix as a identity matrix

	// = matrices.World * matrices.camView * matrices.Projection;

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
	camTarget = XMVector3TransformCoord(defaultForward, camRotationMatrix); // sets the camera target vector by rotating the defaultforward vector with the
																			// rotation matrix we created
	camTarget = XMVector3Normalize(camTarget); // normalizing the camtarget vector

	XMMATRIX RotateYTempMatrix;
	RotateYTempMatrix = XMMatrixRotationY(camYaw); // Finding the new right and forward directions of the camera by  using a rotation matrix 
												   //which will be rotated on the Y-axis, since its a first perosn camera we need to keep our cam forward and right pointing only in x and z axis

												   // transforming the cameras right up and forwards vectors using the matrix just defined.
												   // also rotating the default right up and default foward vectors and set the result in the right up and foward vectors.
	/**/ camRight = XMVector3TransformCoord(defaultRight, RotateYTempMatrix);
	/**/ camUpDown = XMVector3TransformCoord(defaultUp, RotateYTempMatrix);
	/**/ camForward = XMVector3TransformCoord(defaultForward, RotateYTempMatrix);

	camPosition += moveLeftRight* camRight;
	camPosition += moveBackForward* camForward;
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

	float speed = 15.0f * time;

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

		// reading mouse


		/*int mouseX = 0;
		int mouseY= 0;
		mouseX += mouseCurrentState.IX;
		mouseY += mouseCurrentState.IY;

		if (mouseX < 0 ){mouseX = 0; }
		if (mouseY < 0) { mouseY = 0; }

		if (mouseX > WIN_WIDTH) { mouseX = WIN_WIDTH; }
		if (mouseY > WIN_HEIGHT) { mouseY = WIN_HEIGHT; }*/

		POINT mousePos;

		GetCursorPos(&mousePos);
		ScreenToClient(hwnd, &mousePos);

		int mouseX = mousePos.x;
		int mouseY = mousePos.y;

		float tempDist;
		float closestDist = FLT_MAX;

		XMVECTOR pwrsPos, pwrsDir;

		Picking(mouseX, mouseY, pwrsPos, pwrsDir);





		//Picking(mouseX, mouseY);
		
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

void Picking(float mouseX, float mouseY,XMVECTOR& pickRayVectorWorldSpacePos, XMVECTOR&pickRayVectorWorldSpaceDir)
{
	/*POINT mouse;
	

	GetCursorPos(&mouse);

	float pointX, pointY;

	float mousePosX = mouse.x;
	float mousePosY = mouse.y;

	if (GetCursorPos(&mouse))
	{
		ScreenToClient(hwnd, &mouse);
		float mousePosX = mouse.x;
		float mousePosY = mouse.y;

	}*/
	ShowCursor(TRUE);

	XMVECTOR pickRayInViewSpaceDir = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR pickRayInViewSpacePos = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	float PRVecX, PRVecY, PRVecZ;

	XMVECTOR matInvDeter;
	XMMATRIX V = matrices.camView;

	XMMATRIX InversV; 

	InversV = XMMatrixInverse(&matInvDeter, V);

	//transform 2D picking pos.
	XMFLOAT4X4 camProjection;

	XMStoreFloat4x4(&camProjection, matrices.Projection);
	
	PRVecX = (2.0f * mouseX / WIN_WIDTH - 1) / camProjection(0, 0);
	PRVecY = (-2.0f * mouseY / WIN_HEIGHT + 1) / camProjection(1, 1);
	PRVecZ = 1.0f;


	pickRayInViewSpaceDir = XMVectorSet(PRVecX, PRVecY, PRVecZ, 0.0f);


	XMMATRIX pickRayToWorldSpaceMatrix;
	//XMVECTOR matInvDeter;

	pickRayToWorldSpaceMatrix = XMMatrixInverse(&matInvDeter, matrices.camView);

	pickRayVectorWorldSpacePos = XMVector3TransformCoord(pickRayInViewSpacePos, pickRayToWorldSpaceMatrix);
	pickRayVectorWorldSpaceDir = XMVector3TransformNormal(pickRayInViewSpaceDir, pickRayToWorldSpaceMatrix);

	Intersection(pickRayVectorWorldSpacePos, pickRayVectorWorldSpaceDir,matrices.World);
	//XMMATRIX projectionMatrix;
	//XMMATRIX inverseViewMatrix;
	//XMMATRIX translateMatrix;
	//XMMATRIX inverseWorldMatrix;
	//XMMATRIX viewMatrix;
	//XMMATRIX worldMatrix;

	//XMVECTOR pickrayInViewSpaceDir = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	//XMVECTOR pickRayInViewSpacePos = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	//XMVECTOR direction = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	//XMVECTOR origin = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	//XMVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	//XMVECTOR rayDirection = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	///*XMFLOAT3 direction;
	//XMFLOAT3 origin;
	//XMFLOAT3 rayOrigin;
	//XMFLOAT3 rayDirection;*/

	//bool intersect, result;
	//XMFLOAT4 _direction;//so we can get the x,y,z values
	//XMFLOAT4X4 _projectionMatrix;
	//XMFLOAT4X4 _camView;

	//pointX = ((2.0f * (float)mousePosX) / (float)WIN_WIDTH) - 1.0f;
	//pointY = (((2.0f *(float)mousePosY) / (float)WIN_HEIGHT) -1.0f)*-1.0f;
	//projectionMatrix = matrices.Projection;
	//XMStoreFloat4x4(&_projectionMatrix, projectionMatrix);

	//pointX = pointX / _projectionMatrix._11;
	//pointY = pointY / _projectionMatrix._22;

	////inverse of the view matrix.
	//viewMatrix = matrices.camView;
	//XMStoreFloat4x4(&_camView, viewMatrix);
	//XMVECTOR matINVdeter;
	//inverseViewMatrix = XMMatrixInverse(&matINVdeter, viewMatrix);
	////XMMatrixInverse(nullptr, &viewMatrix);
	////inverseViewMatrix = XMMatrixInverse(nullptr, matrices.camView);
	//XMFLOAT4X4 viewInverseMatrix; //making a new variable of the XMFLOAT4x4 type so we can store it.
	//XMStoreFloat4x4(&viewInverseMatrix, inverseViewMatrix);
	//
	////XMStoreFloat3(&_direction, direction);
	//XMStoreFloat4(&_direction, direction);
	//
	////direction XMVectorGetX 
	////XMVectorGetX;
	//_direction.x = (pointX * viewInverseMatrix._11) + (pointY * viewInverseMatrix._21) + viewInverseMatrix._31;
	//_direction.y = (pointX * viewInverseMatrix._12) + (pointY * viewInverseMatrix._22) + viewInverseMatrix._32;
	//_direction.z = (pointX * viewInverseMatrix._13) + (pointY * viewInverseMatrix._23) + viewInverseMatrix._33;

	//direction = XMLoadFloat4(&_direction);
	////direction = _direction;
	//origin = camPosition;
	////XMLoadFloat3(&origin);
	//worldMatrix = matrices.World;

	//translateMatrix = XMMatrixTranslation(-5.0f, 1.0f, 5.0f);
	//worldMatrix = XMMatrixMultiply(worldMatrix, translateMatrix);
	//inverseWorldMatrix = XMMatrixInverse(&matINVdeter, worldMatrix);

	//rayOrigin = XMVector3TransformCoord(origin, inverseWorldMatrix); // pickrayinworldspacepos
	//rayDirection = XMVector3TransformNormal(direction, inverseWorldMatrix); // pickrayinworldspacedir
	//rayDirection = XMVector3Normalize(rayDirection);

	//intersect = Intersection(rayOrigin, rayDirection);


	//if (intersect == true)
	//{
	//	RenderExplosion();
	//}

	////XMVECTOR v0, v1, v2;
	/*for (int i = 0; i < obj.index_counter; i++)
	{
		 obj.vertices.at(i).x;
		 obj.vertices.at(i).y;
		 obj.vertices.at(i).z;

		 float t;

	}*/

	return;
}

//bool HeightMapLoad(char * filename, HeightMapInfo & hminfo)
//{
//
//	FILE *filePtr;
//
//	BITMAPFILEHEADER bitmapFileHeader;
//	BITMAPINFOHEADER bitmapInfoHeader;
//
//	int imageSize, index;
//
//	unsigned char height;
//
//	filePtr = fopen(filename, "rb");
//
//	if (filePtr == NULL)
//	{
//		return 0; 
//	}
//
//	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
//
//	fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
//
//	hminfo.terrainWidth = bitmapInfoHeader.biWidth;
//	hminfo.terrainHeight = bitmapInfoHeader.biHeight;
//
//	imageSize = hminfo.terrainWidth * hminfo.terrainHeight * 3;
//
//	unsigned char* bitmapImage = new unsigned char[imageSize];
//
//	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);
//
//	fread(bitmapImage, 1, imageSize, filePtr);
//
//	fclose(filePtr);
//
//	hminfo.heightMap = new XMFLOAT3[hminfo.terrainWidth* hminfo.terrainHeight];
//
//	int k = 0;
//
//	float heightfactor = 10.0f;
//
//	for (int j = 0; j < hminfo.terrainHeight; j++)
//	{
//		for (int i = 0; i < hminfo.terrainWidth; i++)
//		{
//			height = bitmapImage[k];
//
//			index = (hminfo.terrainHeight * j) + i;
//
//			hminfo.heightMap[index].x = (float)i;
//			hminfo.heightMap[index].y = (float)height / heightfactor;
//			hminfo.heightMap[index].z = (float)j;
//
//			k += 3;
//		}
//	}
//	delete[]bitmapImage;
//	bitmapImage = 0;
//	return true;
//}

float Intersection(XMVECTOR rayOrigin, XMVECTOR rayDirection,XMMATRIX& worldSpace)
{
	for (int i = 0; i < obj.face_idxs.size()/ 3; i++)
	{

		//Triangle's vertices, V1, V2, V3
		XMVECTOR tri1V1 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR tri1V2 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR tri1V3 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);


		//Temporary 3d floats for each vertex.
		XMFLOAT3 _tV1, _tV2, _tV3;
		//XMVECTOR tV1, tV2, tV3;
		// Komma åt triangle.
	
		_tV1.x = obj.finalVector[obj.face_idxs[i * 3 + 0].face_pos].x;
		_tV1.y = obj.finalVector[obj.face_idxs[i * 3 + 0].face_pos].y;
		_tV1.z = obj.finalVector[obj.face_idxs[i * 3 + 0].face_pos].z;

		_tV2.x = obj.finalVector[obj.face_idxs[i * 3 + 1].face_pos].x;
		_tV2.y = obj.finalVector[obj.face_idxs[i * 3 + 1].face_pos].y;
		_tV2.z = obj.finalVector[obj.face_idxs[i * 3 + 1].face_pos].z;
									
		_tV3.x = obj.finalVector[obj.face_idxs[i * 3 + 2].face_pos].x;
		_tV3.y = obj.finalVector[obj.face_idxs[i * 3 + 2].face_pos].y;
		_tV3.z = obj.finalVector[obj.face_idxs[i * 3 + 2].face_pos].z;


		tri1V1 = XMVectorSet(_tV1.x, _tV1.y, _tV1.z,0.0f);
		tri1V2 = XMVectorSet(_tV2.x, _tV2.y, _tV2.z, 0.0f);
		tri1V3 = XMVectorSet(_tV3.x, _tV3.y, _tV3.z, 0.0f);

		tri1V2 = XMVector3TransformCoord(tri1V2, worldSpace);
		tri1V3 = XMVector3TransformCoord(tri1V3, worldSpace);
		tri1V1 = XMVector3TransformCoord(tri1V1, worldSpace);


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

		ep1 = (XMVectorGetX(rayOrigin)*tri1A)    + (XMVectorGetY(rayOrigin)*tri1B)    + (XMVectorGetZ(rayOrigin)*tri1C);
		ep2 = (XMVectorGetX(rayDirection)*tri1A) + (XMVectorGetY(rayDirection)*tri1B) + (XMVectorGetZ(rayDirection)*tri1C);

		if (ep2 != 0.0f)
		{
			t = -(ep1 + tri1D) / (ep2);
		}
		if (t > 0.0f)
		{
			planeIntersectX = XMVectorGetX(rayOrigin) + XMVectorGetX(rayDirection);
			planeIntersectY = XMVectorGetY(rayOrigin) + XMVectorGetY(rayDirection);
			planeIntersectz = XMVectorGetZ(rayOrigin) + XMVectorGetZ(rayDirection);

			pointInPlane = XMVectorSet(planeIntersectX,planeIntersectY,planeIntersectz,0.0f);

			if (PointInTriangle(tri1V1, tri1V2, tri1V3, pointInPlane))
			{
				
				RenderExplosion();
				return t / 2.0f;
				
			}
		}
	

	}
	return FLT_MAX;
	
}
bool PointInTriangle(XMVECTOR&triV1, XMVECTOR&triV2, XMVECTOR&triV3, XMVECTOR&point)
{
	//find if point is inside the triangle

	/*XMVECTOR cp1 = XMVector3Cross((triV3-triV2),(point-triV2));
	XMVECTOR cp2 = XMVector3Cross((triV3 - triV2),(triV1-triV2));

	if (XMVectorGetX(XMVector3Dot(cp1, cp2)) >= 0)
	{
		cp1 = XMVector3Cross((triV3 - triV1), (point - triV1));
		cp2 = XMVector3Cross((triV3 - triV2), (triV1 - triV2));

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
		{
			return false;
		}
	}
	return false;*/
	//To find out if the point is inside the triangle, we will check to see if the point
		//is on the correct side of each of the triangles edges.

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
	while (i < maxExplosionParticles)
	{
		//generate randomized particle properties.
		positionX = ((float)rand() / (RAND_MAX) + (rand() % 1))-0.5f;
		positionY = ((float)rand() / (RAND_MAX) + (rand() % 1))-0.5f;
		positionZ = ((float)rand() / (RAND_MAX) + (rand() % 1))-0.5f;

		//saving the starting position for all particles for reset;
		explosionStartingPos.push_back(StartingPos{ positionX,positionY,positionZ });

		explosionTest.push_back(VertexType{ positionX, positionY, positionZ, 0.02f, 0.02f, 1.0f, 0.0f, 0.0f });
		
		i++;
	}

	D3D11_BUFFER_DESC bufferdesc;
	std::memset(&bufferdesc, 0, sizeof(bufferdesc));
	bufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferdesc.ByteWidth = sizeof(VertexType) * maxExplosionParticles;
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
		XMVECTOR temp = { explosionTest[i].x,explosionTest[i].y,explosionTest[i].z };
		travelDir = temp - origin;
		x = XMVectorGetX(travelDir);
		y = XMVectorGetY(travelDir);
		z = XMVectorGetZ(travelDir);

		explosionTest[i].x = explosionTest[i].x + x / 10.0f;
		explosionTest[i].y = explosionTest[i].y + y / 10.0f;
		explosionTest[i].z = explosionTest[i].z + z / 10.0f;
	}

	D3D11_MAPPED_SUBRESOURCE gMappedResource;
	VertexType* vtxPtr;
	HRESULT hr = gDeviceContext->Map(gVertexBufferExplosion, 0, D3D11_MAP_WRITE_DISCARD, 0, &gMappedResource);

	vtxPtr = (VertexType*)gMappedResource.pData;
	memcpy(vtxPtr, explosionTest.data(), sizeof(VertexType) * maxExplosionParticles);

	gDeviceContext->Unmap(gVertexBufferExplosion, 0);
}

void killExplosion()
{
	if(explosion)
	{
		if (explosionTest[0].x>=5|| explosionTest[0].y >= 5|| explosionTest[0].z >= 5)
		{
			for (int i = 0; i < maxExplosionParticles - 1; i++)
			{
				explosionTest[i].x = explosionStartingPos[i].x;
				explosionTest[i].y = explosionStartingPos[i].y;
				explosionTest[i].z = explosionStartingPos[i].z;
			}
			explosion = false;
		}
	}
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
	bufferdesc.ByteWidth = sizeof(VertexType) * maxParticles;
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

	float positionX, positionY, positionZ, red, green, blue;
	//int index, i, j;
	int i = 0;
	while (i < maxParticles)
	{
		//generate randomized particle properties.
		positionX = (((float)rand() - (float)rand()) / RAND_MAX) * particleDeviationX;
		positionY = (((float)rand() - (float)rand()) / RAND_MAX) * particleDeviationY;
		positionZ = (((float)rand() - (float)rand()) / RAND_MAX) * particleDeviationZ;

		//velocity = particleVelocity + (((float)rand() - (float)rand()) / RAND_MAX) * particleVelocityVariation;

		red = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		green = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		blue = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;

		vertexList.push_back(VertexType{ positionX, 10, positionZ, 0.002f, 0.5f, 0.7f, 0.7f, 1.0f });
		velocity.push_back(((float)rand() / (RAND_MAX + 1) + 1 + (rand() % 3)) / 5.0f);
		i++;
	}


}																		
																				
void UpdateBuffers()
{
	
	for (int i = 0; i < vertexList.size() -1; i++)
	{
		
		if (vertexList[i].y >= 0)
		{
			vertexList[i].y = (vertexList[i].y - velocity[i]);
			
		}
		else
		{
			vertexList[i].y = 10;
		}
	}

	D3D11_MAPPED_SUBRESOURCE gMappedResource;
	VertexType* vtxPtr;
	HRESULT hr = gDeviceContext->Map(gVertexBufferParticle, 0, D3D11_MAP_WRITE_DISCARD, 0, &gMappedResource);

	vtxPtr = (VertexType*)gMappedResource.pData;
	memcpy(vtxPtr, vertexList.data(), sizeof(VertexType) * maxParticles);

	gDeviceContext->Unmap(gVertexBufferParticle, 0);


	

	//matrices.World = XMMatrixTranslation(1.0f, 2.0f, 0.0f);
	//gDeviceContext->UpdateSubresource(gVertexBufferParticle, 0, 0, vertexList.data(), 0, 0);

	//gDeviceContext->GSSetConstantBuffers(0, 1, &gConstantBuffer);

}

void Update()
{
	static float angle = 0.0f;

	angle -= 0.0001f;

	//matrices.World = XMMatrixRotationY(angle);

	gDeviceContext->UpdateSubresource(gConstantBuffer, 0, 0, &matrices, 0, 0);
	gDeviceContext->UpdateSubresource(gConstantLightBuffer, 0, 0, &Lights, 0, 0);

	gDeviceContext->GSSetConstantBuffers(0, 1, &gConstantBuffer);
	gDeviceContext->VSSetConstantBuffers(0, 1, &gConstantBuffer);
	gDeviceContext->PSSetConstantBuffers(0, 1, &gConstantBuffer);
	gDeviceContext->GSSetConstantBuffers(1, 1, &gConstantLightBuffer);
	gDeviceContext->VSSetConstantBuffers(1, 1, &gConstantLightBuffer);
	gDeviceContext->PSSetConstantBuffers(1, 1, &gConstantLightBuffer);
}

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
	gDeviceContext->IASetInputLayout(gVertexLayout);

	gDeviceContext->OMSetRenderTargets(0, NULL, gShadowDepthStencilView);

	gDeviceContext->Draw(vertexCount, 0);

	SetViewport();
	gDeviceContext->OMSetRenderTargets(1, &gBackBufferRTV, gDepthStencilView);
}

void RenderParticles()
{
	//gDeviceContext->ClearDepthStencilView(gDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	
	gDeviceContext->VSSetShader(gVertexShaderParticle, nullptr, 0);
	gDeviceContext->GSSetShader(gGeometryShaderParticle, nullptr, 0);
	gDeviceContext->PSSetShader(gPixelShaderParticle, nullptr, 0);

	UINT32 vertexSize = sizeof(VertexType);
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

	UINT32 vertexSize = sizeof(VertexType);
	UINT32 offset = 0;

	gDeviceContext->IASetVertexBuffers(0, 1, &gVertexBufferExplosion, &vertexSize, &offset);
	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	gDeviceContext->Draw((explosionTest.size() - 1), 0);
}

void Render()
{

	

	float clearColor[] = { 0, 0, 0, 1 };
	gDeviceContext->ClearRenderTargetView(gBackBufferRTV, clearColor);
	gDeviceContext->ClearDepthStencilView(gDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

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
	gDeviceContext->IASetIndexBuffer(gIndexBuffer, DXGI_FORMAT_R32_UINT , 0); // sets the index buffer

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
}

// handle of instance                      commandline		 how the window is shown
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

		ConstantBuffer();

		createTriangle();

		initExplosion();

		EmitParticles();

		initParticle();

		createTextures();
		
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
				Render(); // Rendera
				RenderParticles(); // Rendera

				if (explosion)
				{
					RenderExplosion(); // Rendera
					updateExplosion();
					killExplosion();
				}
				

				

				frameTime = getFrameTime();

				detectInput(frameTime);

				gSwapChain->Present(1, 0); // Växla front och back buffer
			}
		}

		gVertexBuffer->Release();
		gIndexBuffer->Release();
		gConstantBuffer->Release();
		gPixelShader->Release();
		gGeometryShader->Release();
		gVertexShader->Release();

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
		NULL,
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
