
#include <windows.h>

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include <string>
using namespace DirectX;

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

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
XMVECTOR cameraPosVector = { 0, 0, -2};
XMVECTOR lookAtVector = { 0, 0, 0 };
XMVECTOR upVector = { 0, 1, 0 };
// INITIALIZE BUFFERS ***********************************************
ID3D11Buffer* gVertexBuffer = nullptr;
ID3D11Buffer* gIndexBuffer = nullptr;

ID3D11Buffer* gConstantBuffer = nullptr;

ID3D11Texture2D* gBackBuffer = nullptr;

ID3D11RenderTargetView* gBackBufferRTV = nullptr;
ID3D11DepthStencilView* gDepthStencilView = nullptr;

// INITIALIZE STRUCTS ***********************************************
struct MatrixBuffer {
	XMMATRIX World;
	XMMATRIX View;
	XMMATRIX Projection;
};
MatrixBuffer matrices;
// FUNCTIONS********************************************************

void CreateShaders()
{
	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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
}

void createTriangle()
{
	struct TriangleVertex2
	{
		float x, y, z;
		float r, g, b;
	};

	TriangleVertex2 triangleVertices[] =
	{
		-1.0f,-1.0f,-1.0f,
		0,   0,   0,

		-1.0f,+1.0f,-1.0f,
		0,   0,   1 

		+ 1.0f,+1.0f,-1.0f,
		0,   1,   0,

		+1.0f,-1.0f,-1.0f,
		0,   1,   1,

		-1.0f,-1.0f,+1.0f,
		1,   0,   0,
		
		-1.0f,+1.0f,+1.0f,
		1,   0,   1,

		+1.0f,+1.0f,+1.0f,
		1,   1,   0,

		+1.0f,-1.0f,+1.0f,
		1,   1,   1,

	};

	D3D11_BUFFER_DESC bufferdesc;
	memset(&bufferdesc, 0, sizeof(bufferdesc));
	bufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferdesc.Usage = D3D11_USAGE_DEFAULT;
	bufferdesc.ByteWidth = sizeof(triangleVertices) * 8;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = triangleVertices;
	HRESULT hr = gDevice->CreateBuffer(&bufferdesc, &data, &gVertexBuffer);

	//int indices[] = {
	//	0,2,1, // -x
	//	1,2,3,

	//	4,5,6, // +x
	//	5,7,6,

	//	0,1,5, // -y
	//	0,5,4,

	//	2,6,7, // +y
	//	2,7,3,

	//	0,4,6, // -z
	//	0,6,2,

	//	1,3,7, // +z
	//	1,7,5,
	//};


	//D3D11_BUFFER_DESC bufferDesc2;
	//bufferDesc2.ByteWidth = sizeof(indices);
	//bufferDesc2.Usage = D3D11_USAGE_DEFAULT;
	//bufferDesc2.BindFlags = D3D11_BIND_INDEX_BUFFER;

	//D3D11_SUBRESOURCE_DATA initData;
	//ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
	//initData.pSysMem = indices;
	//initData.SysMemPitch = 0;
	//initData.SysMemSlicePitch = 0;

	//hr = gDevice->CreateBuffer(&bufferDesc2, &initData, &gIndexBuffer);
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
	matrices.View = XMMatrixLookAtLH((cameraPosVector), (lookAtVector),(upVector));
	matrices.Projection = XMMatrixPerspectiveFovLH((XM_PI * 0.45), (640.0 / 480.0), (0.5), (20));
	//                                           FOV(Field of view)| Windowsize |Near plane | Far plane

	matrices.View = XMMatrixTranspose(matrices.View);
	matrices.Projection = XMMatrixTranspose(matrices.Projection); // Transposing the projection and view matrices.
	matrices.World = XMMatrixIdentity();              // Setting the world matrix as a identity matrix

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

void Update()
{
	/*static float angle = 0.0f;

	angle -= 0.01f;

	matrices.World = XMMatrixRotationY(XMConvertToRadians(angle));*/
	
	gDeviceContext->UpdateSubresource(gConstantBuffer, 0, 0, &matrices, 0, 0);

	gDeviceContext->GSSetConstantBuffers(0, 1, &gConstantBuffer);
}

void Render()
{

	float clearColor[] = { 0, 0, 0, 1 };
	gDeviceContext->ClearRenderTargetView(gBackBufferRTV, clearColor);
	gDeviceContext->ClearDepthStencilView(gDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);


	gDeviceContext->VSSetShader(gVertexShader, nullptr, 0);
	gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	gDeviceContext->DSSetShader(nullptr, nullptr, 0);
	gDeviceContext->GSSetShader(gGeometryShader, nullptr, 0);
	gDeviceContext->PSSetShader(gPixelShader, nullptr, 0);

	UINT32 vertexSize = sizeof(float)* 6;
	UINT32 offset = 0;

	gDeviceContext->IASetVertexBuffers(0, 1, &gVertexBuffer, &vertexSize, &offset);
	gDeviceContext->IASetIndexBuffer(gIndexBuffer, DXGI_FORMAT_R16_UINT , 0); // sets the index buffer

	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gDeviceContext->IASetInputLayout(gVertexLayout);

	gDeviceContext->Draw(8, 0);
}

// handle of instance                      commandline		 how the window is shown
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) // wWinMain Predefined main for directx
{
	//Initialize window
	MSG msg = { 0 };
	HWND wndHandle = InitWindow(hInstance);						// Skapar f�nstret
												//window is valid
	if (wndHandle)
	{
		CreateDirect3DContext(wndHandle); //2. Skapa och koppla SwapChain, Device och Device Context

		SetViewport();
		//myWindow.SetViewport(gDevice, gDevContext);

		CreateShaders();

		ConstantBuffer();

		createTriangle();
		
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
				Update();

				Render(); // Rendera

				gSwapChain->Present(0, 0); // V�xla front och back buffer
			}
		}

		gVertexBuffer->Release();
		//gIndexBuffer->Release();
		gConstantBuffer->Release();
		gPixelShader->Release();
		gGeometryShader->Release();
		gVertexShader->Release();

		gDevice->Release();
		gDeviceContext->Release();
		gSwapChain->Release();
		gBackBufferRTV->Release();
		DestroyWindow(wndHandle);
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

													// Fill the swap chain description struct

	SCD.BufferCount = 1;								// One back buffer
	SCD.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Use 32 bit color
	SCD.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  // How swap chain is to be used
	SCD.OutputWindow = windowHandle;						// The window to be used
	SCD.SampleDesc.Count = 4;							// How many multisamples
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





