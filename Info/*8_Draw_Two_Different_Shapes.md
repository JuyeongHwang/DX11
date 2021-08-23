

## main.cpp

~~~

#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <fstream>
using namespace std;

/////////////
// LINKING //
/////////////
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
using namespace DirectX;

//*** 전역변수 ***//
const int Width = 800;
const int Height = 600;
IDXGISwapChain* swapChain = nullptr;
ID3D11Device* device = nullptr;
ID3D11DeviceContext* deviceContext = nullptr;
ID3D11RenderTargetView* renderTargetView = nullptr;

ID3D11Buffer* squareVertexBuffer = nullptr;
ID3D11Buffer* triangleVertexBuffer = nullptr;
ID3D11Buffer* squareIndexBuffer = nullptr;
ID3D11Buffer* triangleIndexBuffer = nullptr;
ID3D11VertexShader* vertexShader;
ID3D11PixelShader* pixelShader;
ID3D11InputLayout* layout;

//정점 배열 수
float vertexCount = 0;
float indexCount = 0;

LPCTSTR WndClassName = L"firstWindow";
HWND hwnd = NULL;


// **** shader **** //
bool InitializeShader();

D3D11_INPUT_ELEMENT_DESC Playout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};
unsigned int numElements = ARRAYSIZE(Playout);

// *** model *** ///

struct VertexType {
	VertexType() {} //기본 생성자. 없으면 오류난다.
	VertexType(float x, float y, float z,
		float r, float g, float b, float a)
		: pos(x, y, z), color(r, g, b, a) {}

	XMFLOAT3 pos;
	XMFLOAT4 color;
};


// *** direct3D *** ///
bool InitializeDirect3d11App(HINSTANCE hInstance);
void ReleaseObjects();

void UpdateScene();
void DrawScene();
void shaderPipeline();

// *** window *** ///
bool InitializeWindow(HINSTANCE hInstance, int ShowWnd, int width, int height, bool windowed);
int messageloop();
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lParam);



///****< Main Windows Fuction >****///
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

	if (!InitializeWindow(hInstance, nShowCmd, Width, Height, true)) {

		MessageBox(0, L"Window Initialization - Failed", L"Error", MB_OK);
		return 0;
	}

	if (!InitializeDirect3d11App(hInstance)) {
		MessageBox(0, L"Direct3d Initialization - Failed", L"Error", MB_OK);
		return 0;
	}

	if (!InitializeShader()) {
		MessageBox(hwnd, L"Could not initialize the shader object.", L"Error", MB_OK);
		return false;
	}

	//loop
	messageloop();

	ReleaseObjects();

	return 0;
}


////////// ********************************************** ////////////////
bool InitializeTriangle() {
	VertexType vertices2[] = {
		VertexType(-1.f, 0.f, 0.0f,
		1.0f, 0.f, 0.f, 1.f), //bottm left
		VertexType(-1.0f, 0.5f, 0.0f,
		0.f, 1.f, 0.f, 1.f), //top left
		VertexType(-0.7f, 0.5f, 0.0f,
		0.f, 0.f, 1.f, 1.f), //bottom right
		VertexType(0.5 * 2.f, 0.5 * 2.f, 0.0f,
		1.f, 1.f, 1.f, 1.f) //Top right
	};

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource 구조에 정점 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA vertexData;
	ZeroMemory(&vertexData, sizeof(vertexData));
	vertexData.pSysMem = vertices2;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	// 이제 정점 버퍼를 만듭니다.
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &triangleVertexBuffer)))
	{
		return false;
	}

	DWORD indiceTriangle[] = {
		0,1,2
	};

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 인덱스 데이터를 가리키는 보조 리소스 구조체를 작성합니다.
	D3D11_SUBRESOURCE_DATA indexData;
	ZeroMemory(&indexData, sizeof(D3D11_SUBRESOURCE_DATA));
	indexData.pSysMem = indiceTriangle;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼를 생성합니다.
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &triangleIndexBuffer)))
	{
		return false;
	}

}

void drawTriangle() {

	UINT stride = sizeof(VertexType);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &triangleVertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(triangleIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->DrawIndexed(indexCount, 0, 0);
}
bool InitializeSquare() {
	vertexCount = 4;
	indexCount = 6;

	VertexType vertices[] = {
		VertexType(-0.5f, -0.5f, 0.0f,
		1.0f, 0.f, 0.f, 1.f), //bottm left
		VertexType(-0.5f, 0.5f, 0.0f,
		0.f, 1.f, 0.f, 1.f), //top left
		VertexType(0.5f, -0.5f, 0.0f,
		0.f, 0.f, 1.f, 1.f), //bottom right
		VertexType(0.5f, 0.5f, 0.0f,
		1.f, 1.f, 1.f, 1.f) //Top right
	};

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource 구조에 정점 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	// 이제 정점 버퍼를 만듭니다.
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &squareVertexBuffer)))
	{
		return false;
	}


	unsigned long* indices = new unsigned long[indexCount];
	if (!indices) {
		MessageBox(0, L"creaet indices - Failed", L"Error", MB_OK);
		return false;
	}

	// 인덱스 배열에 값을 설정합니다.
	indices[0] = 0;  // Bottom left.
	indices[1] = 1;  // Top left.
	indices[2] = 2;  // Bottom right.

	indices[3] = 3; //Top right
	indices[4] = 2; //bottom right
	indices[5] = 1; //Top left

	// 정적 인덱스 버퍼의 구조체를 설정합니다.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 인덱스 데이터를 가리키는 보조 리소스 구조체를 작성합니다.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼를 생성합니다.
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &squareIndexBuffer)))
	{
		return false;
	}

	// 생성되고 값이 할당된 정점 버퍼와 인덱스 버퍼를 해제합니다.

	delete[] indices;
	indices = 0;
}
void drawSquare() {
	UINT stride = sizeof(VertexType);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &squareVertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(squareIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->DrawIndexed(indexCount, 0, 0);

}
bool InitializeShader() {

	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer; //from hlsl
	ID3D10Blob* pixelShaderBuffer; //from hlsl
	
	InitializeSquare();
	InitializeTriangle();
	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// Compile the vertex shader code.
	D3DCompileFromFile(L"effects.fx", NULL, NULL, "VS", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&vertexShaderBuffer, &errorMessage);

	D3DCompileFromFile(L"effects.fx", NULL, NULL, "PS", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errorMessage);


	// Create the vertex shader from the buffer.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &vertexShader);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not create the vertex shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the pixel shader from the buffer.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &pixelShader);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not create the pixel shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the vertex input layout.
	result = device->CreateInputLayout(Playout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &layout);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not create the layout object.", L"Error", MB_OK);
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;
	

	return true;

}


void shaderPipeline() {

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	deviceContext->IASetInputLayout(layout);
	deviceContext->VSSetShader(vertexShader, NULL, 0);
	deviceContext->PSSetShader(pixelShader, NULL, 0);

}

bool InitializeDirect3d11App(HINSTANCE hInstance) {

	HRESULT hr;

	//m_vsync_enabled = true;

	//버퍼 DESCRIBE for backBuffer
	DXGI_MODE_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

	bufferDesc.Width = Width;
	bufferDesc.Height = Height; //resolution
	bufferDesc.RefreshRate.Numerator = 60;
	bufferDesc.RefreshRate.Denominator = 1; //Hz. 1/60
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//스캔 라인 순서 및 크기를 지정하지 않음.
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//스왑체인 DESCRIBE
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDesc.BufferDesc = bufferDesc; //위에서 정의
	swapChainDesc.BufferCount = 1; //백버퍼 하나만 사용.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //백버퍼의 사용 용도 지정
	swapChainDesc.OutputWindow = hwnd; //사용될 윈도우 핸들

	//멀티샘플링
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; //출력된 다음 백버퍼를 비우도록 지정.
	swapChainDesc.Flags = 0; //추가 옵션 플래그 사용안함.

	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL,
		NULL, NULL, NULL, D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &device, NULL, &deviceContext);
	if (FAILED(hr)) {
		MessageBox(NULL, L"Create SwapChain failed", L"D3D11CreateDeviceAndSwapChain", MB_OK);
		return false;
	}

	//백버퍼 생성
	ID3D11Texture2D* BackBufferPtr = nullptr;
	hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBufferPtr);
	if (FAILED(hr)) {
		MessageBox(NULL, L"Create BackBuffer failed", L"swapChain->GetBuffer", MB_OK);
		return false;
	}

	//백버퍼포인터로 렌더타겟 뷰 생성
	hr = device->CreateRenderTargetView(BackBufferPtr, NULL, &renderTargetView);
	if (FAILED(hr)) {
		MessageBox(NULL, L"Create rendertargetview failed", L"device->CreateRenderTargetView", MB_OK);
		return false;
	}

	//필요 없어진 백버퍼 포인터 해제
	BackBufferPtr->Release();
	BackBufferPtr = 0;

	deviceContext->OMSetRenderTargets(1, &renderTargetView, NULL);

	//렌더링을 위한 뷰포트
	D3D11_VIEWPORT viewport;
	viewport.Width = (float)Width;
	viewport.Height = (float)Height;
	viewport.MinDepth = 0.f;
	viewport.MaxDepth = 1.f;
	viewport.TopLeftX = 0.f;
	viewport.TopLeftY = 0.f;

	deviceContext->RSSetViewports(1, &viewport);


	return true;
}
void ReleaseObjects() {
	swapChain->Release();
	renderTargetView->Release();
	device->Release();
	deviceContext->Release();

	//model
	squareVertexBuffer->Release();
	triangleVertexBuffer->Release();
	squareIndexBuffer->Release();
	triangleIndexBuffer->Release();


	vertexShader->Release();
	pixelShader->Release();
	layout->Release();

}

//화면 프레임
void UpdateScene() {

	float bgColor[] = { 1.0f,1.0f,1.0f,1.0f };
	
	deviceContext->ClearRenderTargetView(renderTargetView, bgColor);
	shaderPipeline();
	
	drawTriangle();
	drawSquare();
	
	
	
}

void DrawScene() {

	swapChain->Present(0, 0);

}

//// **********  시스템 클래스 ********** ////
bool InitializeWindow(HINSTANCE hInstance, int ShowWnd, int width, int height, bool windowed) {

	//1. WNDCLASSEX 만들기
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = NULL;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = WndClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

	//2. 등록
	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, L"Error registering class", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	//3. 만들기. This is the part where we create our window!!

	hwnd = CreateWindowEx(NULL, WndClassName, L"Geometry Dash", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		width, height, NULL, NULL, hInstance, NULL);


	if (!hwnd) {
		MessageBox(NULL, L"Error creating window", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	//4.창을 보여주고 업데이트
	ShowWindow(hwnd, ShowWnd);
	UpdateWindow(hwnd);

	return true;
}

int messageloop() {

	//1. MSG 구조체 객체 만들기. This will hold the message's information.
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));


	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) {
				break;
			}
			TranslateMessage(&msg); //키보드의 virtual key를 characters로 변환하는 것같은 역할
			DispatchMessage(&msg); //보낸다 메세지를. 우리의 window precedure에게.(WndProc)
		}
		else {
			UpdateScene();
			DrawScene();
		}
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			if (MessageBox(0, L"Are you sure you want to exit?", L"Really?", MB_YESNO | MB_ICONQUESTION) == IDYES)
				DestroyWindow(hwnd);
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}
~~~

## effects.fx

~~~
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
};

VS_OUTPUT VS(float4 inPos : POSITION, float4 inColor : COLOR)
{
    VS_OUTPUT output;

    output.Pos = inPos;
    output.Color = inColor;

    return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    return input.Color;
}
~~~