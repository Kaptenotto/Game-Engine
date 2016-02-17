
#include <windows.h>

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include <string>
#include <dinput.h>
#include <vector>

#include <stdio.h>
#include <io.h>
#include <fcntl.h>

#include "importer.h"
#include "Wic.h"



#include <Wincodec.h>



using namespace DirectX;
using namespace std;

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

HWND InitWindow(HINSTANCE hInstance);

// message procedure, HWND = handle to window.
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT CreateDirect3DContext(HWND wndHandle);
IDXGISwapChain* gSwapChain = nullptr;



// DEVICE **********************************************************
ID3D11Device* gDevice = nullptr;
ID3D11DeviceContext* gDeviceContext = nullptr;
// INITIALIZE LAYOUTS **********************************************
ID3D11InputLayout* gVertexLayout = nullptr;
 // INITIALIZE SHADERS *********************************************
ID3D11VertexShader* gVertexShader = nullptr;

ID3D11PixelShader* gPixelShader = nullptr;

ID3D11GeometryShader* gGeometryShader = nullptr;
//INITIALIZE VECTORS ***********************************************

XMVECTOR camPosition = { 0, 0, -5 ,0 };
XMVECTOR camTarget = { 0, 0, 0, 0 };
XMVECTOR camUp = { 0, 1, 0, 0};


// INITIALIZE BUFFERS ***********************************************
ID3D11Buffer* gVertexBuffer = nullptr;
ID3D11Buffer* gIndexBuffer = nullptr;
ID3D11Buffer* gGroundVertexBuffer = nullptr;
ID3D11Buffer* gGroundIndexBuffer = nullptr;

ID3D11Buffer* gConstantBuffer = nullptr;

ID3D11Texture2D* gBackBuffer = nullptr;

ID3D11RenderTargetView* gBackBufferRTV = nullptr;
ID3D11DepthStencilView* gDepthStencilView = nullptr;

// INITIALIZE SHADER THINGS *****************************************

//ID3D11ShaderResourceView* textureResource;
//ID3D11ShaderResourceView* textureResource2;

vector<ID3D11ShaderResourceView*> textureResources;

ID3D11SamplerState* texSamplerState;

//IWICImagingFactory* imgFac;


//static IWICImagingFactory * _GetWIC()
//{
//	static IWICImagingFactory* s_Factory = nullptr;
//
//	if (s_Factory)
//		return s_Factory;
//
//	HRESULT hr = CoCreateInstance(
//		CLSID_WICImagingFactory1,
//		nullptr,
//		CLSCTX_INPROC_SERVER,
//		__uuidof(IWICImagingFactory),
//		(LPVOID*)&s_Factory
//		);
//
//	if (FAILED(hr))
//	{
//		s_Factory = nullptr;
//		return nullptr;
//	}
//
//	return s_Factory;
//}

//CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, __uuidof(IWICImagingFactory), (void**)&imgFac);


// INITIALIZE OBJ-IMPORTER ******************************************
Importer obj;


// INITIALIZE STRUCTS ***********************************************

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

struct MatrixBuffer {
	XMMATRIX World;
	XMMATRIX camView;
	XMMATRIX Projection;
	
};
MatrixBuffer matrices;

typedef struct DIMOUSESTATES
{
	LONG IX;
	LONG IY;
	LONG IZ;
	BYTE rgbButtons[4];
};

// GLOBALS FOR FIRST PERSON CAMERA *********************************

XMVECTOR defaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
XMVECTOR defaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
XMVECTOR camForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
XMVECTOR camRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

XMMATRIX camRotationMatrix;
XMMATRIX groundWorld;

float moveLeftRight = 0.0f;		// Used to move along the camFoward and camRight vectors
float moveBackForward = 0.0f;   // Used to move along the camFoward and camRight vectors

float camYaw = 0.0f;
float camPitch = 0.0f;


XMMATRIX Translation;
XMMATRIX Scale;

//GLOBALS FOR INPUT ************************************************

HWND hWnd = NULL;

IDirectInputDevice8* DIKeyboard;
IDirectInputDevice8* DIMouse;

DIMOUSESTATES mouseLastState;
LPDIRECTINPUT8 directInput;

float rotx = 0;
float rotz = 0;
float scaleX = 1.0f;
float scaleY = 1.0f;

XMMATRIX rotationX;
XMMATRIX rotationY;

// TIME GLOBALS ****************************************************

double countsPerSecond = 0.0;
__int64 counterStart = 0;

int frameCount = 0;
int fps = 0;

__int64 frameTimeOld = 0;
double frameTime;

// LIGHT************************************************************
//ID3D11Buffer* cbPerFrameBuffer;
//
//struct cbPerObject {
//	XMMATRIX WVP;
//	XMMATRIX World;
//};
//
//struct Light
//{
//	Light()
//	{
//		ZeroMemory(this, sizeof(Light));
//	}
//	XMFLOAT3 dir;
//	float pad;
//	XMFLOAT4 ambient;
//	XMFLOAT4 diffuse;
//}light;
//
//struct cbPerFrame
//{
//	Light light;
//};
//
//cbPerFrame constbuffPerFrame;


// FUNCTIONS********************************************************

void createGround()
{
	//Create the vertex buffer
	Vertex v[] =
	{
		// Bottom Face
		Vertex(-1.0f, -1.0f, -1.0f, 100.0f, 100.0f, 0.0f, 1.0f, 0.0f),
		Vertex(1.0f, -1.0f, -1.0f,   0.0f, 100.0f, 0.0f, 1.0f, 0.0f),
		Vertex(1.0f, -1.0f,  1.0f,   0.0f,   0.0f, 0.0f, 1.0f, 0.0f),
		Vertex(-1.0f, -1.0f,  1.0f, 100.0f,   0.0f, 0.0f, 1.0f, 0.0f),
	};

	DWORD indices[] = {
		0,  1,  2,
		0,  2,  3,
	};

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * 2 * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;

	iinitData.pSysMem = indices;
	gDevice->CreateBuffer(&indexBufferDesc, &iinitData, &gGroundIndexBuffer);

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * 4;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;

	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = v;
	HRESULT hr = gDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &gGroundVertexBuffer);
}

void CreateShaders()
{
	

	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 3 , D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * 5, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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

	//Reads obj-File
	

	


}

void createTextures()
{
	HRESULT hr;

	ID3D11Resource* texResource;

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
		
	}
	texResource->Release();
}

void objVertexBuffer()
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

	//UINT indices[] = {
	//	0,1,2, // front face
	//	0,2,3,
	//
	//	4,6,5, // back face
	//	4,7,6,
	//
	//	4,5,1, // left
	//	4,1,0,
	//
	//	3,2,6, // right
	//	3,6,7,
	//
	//	1,5,6, // top face
	//	1,6,2,
	//
	//	4,0,3, // bot face
	//	4,3,7,
	//};


	D3D11_BUFFER_DESC bufferDesc2;
	bufferDesc2.ByteWidth = sizeof(obj.face_idxs[0]) * obj.face_idxs.size();
	bufferDesc2.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc2.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc2.CPUAccessFlags = 0;
	bufferDesc2.MiscFlags = 0;
	bufferDesc2.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initData;
	//ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
	initData.pSysMem = &obj.face_idxs[0];
	hr = gDevice->CreateBuffer(&bufferDesc2, &initData, &gIndexBuffer);
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
	float aspectRatio = 640.0 / 480.0;
	float nearZ = 0.5;
	float farZ = 1000.0;

	matrices.camView = XMMatrixLookAtLH(
		(camPosition),
		(camTarget),
		(camUp));

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





}

void SetViewport()
{
	D3D11_VIEWPORT vP;
	vP.Width = (float)640;
	vP.Height = (float)480;
	vP.MinDepth = 0.0f;
	vP.MaxDepth = 1.0f;
	vP.TopLeftX = 0;
	vP.TopLeftY = 0;
	gDeviceContext->RSSetViewports(1, &vP);
}

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
	hr = DIKeyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	hr = DIMouse->SetDataFormat(&c_dfDIMouse);
	hr = DIMouse->SetCooperativeLevel(hWnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);

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
	/**/ camUp = XMVector3TransformCoord(camUp, RotateYTempMatrix);
	/**/ camForward = XMVector3TransformCoord(defaultForward, RotateYTempMatrix);

	camPosition += moveLeftRight* camRight;
	camPosition += moveBackForward* camForward;

	moveLeftRight = 0.0f;
	moveBackForward = 0.0f;

	camTarget = camPosition + camTarget;

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
		PostMessage(hWnd, WM_DESTROY, 0, 0);
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
	if ((mouseCurrentState.IX != mouseLastState.IX) || (mouseCurrentState.IY != mouseLastState.IY))
	{
		camYaw += mouseLastState.IX * 0.001f;

		camPitch += mouseCurrentState.IY * 0.001f;

		mouseLastState = mouseCurrentState;
	}
	updateCamera();

	return;
}

//TIME FUNCTIONS*********************************************************

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

void Update()
{
	gDeviceContext->UpdateSubresource(gConstantBuffer, 0, 0, &matrices, 0, 0);

	gDeviceContext->GSSetConstantBuffers(0, 1, &gConstantBuffer);
}

//void updateGround()
//{
//	groundWorld = XMMatrixIdentity();
//
//	Scale = XMMatrixScaling(500.0f, 10.0f, 500.0f);
//	Translation = XMMatrixTranslation(0.0f, 10.0f, 0.0f);
//
//	groundWorld = Scale * Translation;
//
//	gDeviceContext->UpdateSubresource(gConstantBuffer, 0, 0, &matrices, 0, 0);
//
//	gDeviceContext->GSSetConstantBuffers(0, 1, &gConstantBuffer);
//}

//void RenderGround()
//{
//	gDeviceContext->VSSetShader(gVertexShader, nullptr, 0);
//	gDeviceContext->HSSetShader(nullptr, nullptr, 0);
//	gDeviceContext->DSSetShader(nullptr, nullptr, 0);
//	gDeviceContext->GSSetShader(gGeometryShader, nullptr, 0);
//	gDeviceContext->PSSetShader(gPixelShader, nullptr, 0);
//
//	UINT32 vertexSize = sizeof(obj.finalVector[0]);
//	UINT32 vertexCount = obj.finalVector.size();
//	UINT32 indexSize = obj.index_counter;
//	UINT32 offset = 0;
//
//	gDeviceContext->IASetVertexBuffers(0, 1, &gGroundVertexBuffer, &vertexSize, &offset);
//	gDeviceContext->IASetIndexBuffer(gGroundIndexBuffer, DXGI_FORMAT_R32_UINT, 0); // sets the index buffer
//
//	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//	gDeviceContext->IASetInputLayout(gVertexLayout);
//}

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


	/************************************************************
	 ****************************DRAW****************************
     ************************************************************/
	int n = 0;

	for (int i = 0; i < (obj.objCounter); i++)
	{ 
		/*if (n < textureResources.size())
		{
			n++;
		}*/
		gDeviceContext->PSSetShaderResources(0, 1, &textureResources[i]);

		gDeviceContext->Draw((obj.drawOffset[(i+1)] - obj.drawOffset[i]), obj.drawOffset[i]);
		
	}
}

// handle of instance                      commandline		 how the window is shown
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) // wWinMain Predefined main for directx
{


	//adding a console
	/*if (AllocConsole())
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
	}*/



	// use the console just like a normal one - printf(), getchar(), ...

	//Initialize window
	MSG msg = { 0 };
	hWnd = InitWindow(hInstance);						// Skapar fönstret
												//window is valid
	if (!initDirectInput(hInstance))
	{
		MessageBox(0, L"DIRECT INPUT INITILIZATION - FAILED",
			L"ERROR", MB_OK);
		return 0;
	}

	if (hWnd)
	{
		obj.read();

		CreateDirect3DContext(hWnd); //2. Skapa och koppla SwapChain, Device och Device Context

		SetViewport();
		//myWindow.SetViewport(gDevice, gDevContext);

		CreateShaders();
		
		ConstantBuffer();

	/*	createGround();*/

		objVertexBuffer();

		createTextures();
		
		//Shows the window
		ShowWindow(hWnd, nCmdShow);

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
				Update();
				
				/*updateGround();

				RenderGround();*/

				Render(); // Rendera

				frameCount++;
				if (getTime() > 1.0f)
				{
					fps = frameCount;
					frameCount = 0;
					startTimer();
				}

				frameTime = getFrameTime();

				detectInput(frameTime);

				gSwapChain->Present(0, 0); // Växla front och back buffer
			}
		}

		gVertexBuffer->Release();
		gIndexBuffer->Release();
		gConstantBuffer->Release();
		gPixelShader->Release();
		gGeometryShader->Release();
		gVertexShader->Release();

		gDevice->Release();
		gDeviceContext->Release();
		gSwapChain->Release();
		gBackBufferRTV->Release();
		DestroyWindow(hWnd);

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

	RECT rc = { 0, 0, 640, 480 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	hWnd = CreateWindow(
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

	if (!hWnd)
	{
		return false;
	}

	return hWnd;
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

		createDepthStencil();

		//use the back buffer adress to create the render target
		gDevice->CreateRenderTargetView(gBackBuffer, NULL, &gBackBufferRTV);
		gBackBuffer->Release();

		//Set the render target as the back buffer
		gDeviceContext->OMSetRenderTargets(1, &gBackBufferRTV, gDepthStencilView);
	}
	return hr;
}





