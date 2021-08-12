#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
// Windows ��� ����:
#include <windows.h>

// C ��Ÿ�� ��� �����Դϴ�.
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

IDXGISwapChain* SwapChain;
ID3D11Device* device;
ID3D11DeviceContext* deviceContext;
ID3D11RenderTargetView* renderTargetView;

ID3D11Buffer* triangleVertBuffer;
ID3D11VertexShader* VS;
ID3D11PixelShader* PS;
ID3D10Blob* errorMessage;
ID3D10Blob* VS_Buffer;
ID3D10Blob* PS_Buffer;
ID3D11InputLayout* vertLayout;

float red = 0.0f;
float green = 0.0f;
float blue = 0.0f;
int colormodr = 1;
int colormodg = 1;
int colormodb = 1;

LPCTSTR WndClassName = L"firstWindow";
HWND hwnd = NULL;

const int Width = 800;
const int Height = 600;

//direct3d�� �ʱ�ȭ�� �� ���
bool InitializeDirect3d11App(HINSTANCE hInstance);
//�޸𸮸� ����
void ReleaseObjects();
bool InitScene();
//change our scene on a per-frame basis
void UpdateScene();
void DrawScene();

void CleanUp();

//window ����� ���� : true, �ƴϸ� false�� ��ȯ�� ����.
bool InitializeWindow(HINSTANCE hInstance, int ShowWnd, int width, int height, bool windowed);

//���α׷��� ���ư� ��.
int messageloop();

// window callback procedure�� �����������.
// �������� �޼����� ó���ϴ� ���Դϴ�
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lParam);



struct Vertex {
	Vertex() {}
	Vertex(float x, float y, float z , float r, float g, float b, float a) : pos(x, y, z), color(r,g,b,a) {}

	XMFLOAT3 pos;
	XMFLOAT4 color;
};

D3D11_INPUT_ELEMENT_DESC layout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA,0},
};
UINT numElements = ARRAYSIZE(layout);

/*
ID3D11Device::CreateInputLayout()
ID3D11Device::CreateBuffer()
ID3D11DeviceContext::IASetVertexBuffers()
ID3D11DeviceContext::IASetInputLayout()

ID3D11DeviceContext::IASetPrimitiveTopology()
ID3D11DeviceContext::Draw()
*/


///****< Main Windows Fuction >****///

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

	if (!InitializeWindow(hInstance, nShowCmd, Width, Height, true)) {

		MessageBox(0, L"Window Initialization - Failed", L"Error", MB_OK);

		return 0;
	}


	if (!InitializeDirect3d11App(hInstance)) {
		MessageBox(0, L"Direct3D Initialization - Failed", L"Error", MB_OK);
		return 0;
	}

	if (!InitScene()) //Initialize out scene
	{
		MessageBox(0, L"Scene Initialization - Failed", L"Error", MB_OK);
		return 0;
	}

	messageloop();

	CleanUp();

	return 0;
}

/*
Direct3D�� �����ϱ� ���� ù��° COM �������̽� ������Ʈ�� �ٷ� SwapChain!
����ü���� ����ۿ� ����Ʈ���۸� ���� �ٲٴµ� �̿���.
���� ���۸��̶�� �Ҹ���.
�츮�� ���� ȭ�鿡 �����Ҷ� �츮�� ����۸� �������ϰ��ִ´�. �� ���� �� �׸��� ����۸� ����Ϳ� �����ش�.
�ٸ� ���δ�, �츮�� ��ĵ���ε��� �������̴�.

�� ���� Interface�� �ٷ� �츮�� �ϵ���� ����̽� GPU ���� �ǰڴ�.
���� directX11 ���ʹ� ���ο� �������̽��� �����. ID3D11Device��� �����ε�,
�̴� ��Ƽ�������� ���� �ΰ��� ������. �츮�� ID3D11DeviceContext �������̽� ������Ʈ��
��� ������ �޼ҵ带 �θ� �� ����� ���̴�. ID3D11Device�� �������� �ʿ���� ������ �޼ҵ���� �θ� �� �̿��� ����.

DirectX11�� ���ο� ��Ƽ������ Ư¡�� ������, ���ø����̼��� �ӵ��� ������ �ϰ��ߴ�.
�ʰ� � �޸𸮸� �ε��ϰų�, � ���̳� ������Ʈ�� ���� �� �ʴ� ID3D11Device object�� �θ����̴�.
������Ʈ�� ���� �ҷ����ų� ����������� �ʴ� ����̽����ؽ�Ʈ �������̽��� �ҷ�, ���� ���� ��ü�� ��� ������ �ǵ��� �� ���̴�.

�׷��� ���� �츮�� ���� �ٸ� ���� Ÿ�� �� �������̽� ������Ʈ�� ������.
�Ϲ������� �츮�� 2d textrue�� backbuffer�� ��ũ���� ���������� ���� �ʰ� render target view�� ��� �׸���.
�׸��� �� �ؽ��Ĵ� ����Ÿ���� ���������ο��� �ƿ�ǲ ���� ���������� �������� ���⼭ �׸��� �� ���̴�.
*/

bool InitializeDirect3d11App(HINSTANCE hInstance)
{
	HRESULT herror;
	//Describe our Buffer
	DXGI_MODE_DESC bufferDesc;

	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

	bufferDesc.Width = Width;
	bufferDesc.Height = Height;
	bufferDesc.RefreshRate.Numerator = 60;
	bufferDesc.RefreshRate.Denominator = 1;
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//Describe our SwapChain
	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDesc.BufferDesc = bufferDesc;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;


	//Create our SwapChain
	herror = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL,
		D3D11_SDK_VERSION, &swapChainDesc, &SwapChain, &device, NULL, &deviceContext);
	if (FAILED(herror)) {
		MessageBox(NULL, L"Create SwapChain failed", L"D3D11CreateDeviceAndSwapChain", MB_OK);
		return 0;
	}


	//Create our BackBuffer
	ID3D11Texture2D* BackBuffer;
	herror = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBuffer);
	if (FAILED(herror)) {
		MessageBox(NULL, L"Create Back failed", L"SwapChain->GetBuffer", MB_OK);
		return 0;
	}
	//Create our Render Target
	herror = device->CreateRenderTargetView(BackBuffer, NULL, &renderTargetView);
	BackBuffer->Release();
	if (FAILED(herror)) {
		MessageBox(NULL, L"Create Render Target failed", L"device->CreateRenderTargetView", MB_OK);
		return 0;
	}
	//Set our Render Target
	deviceContext->OMSetRenderTargets(1, &renderTargetView, NULL);

	return true;
}

/*
bool InitializeDirect3d11App(HINSTANCE hInstance)
{
	//���� üũ�� ���� ����
	//Describing the BackBuffer.***********]
	DXGI_MODE_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

	bufferDesc.Width = Width; //resolution
	bufferDesc.Height = Height; //resolution
	bufferDesc.RefreshRate.Numerator = 60; //hz ����
	bufferDesc.RefreshRate.Denominator = 1; //hz ����. 60/1
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //32bit unsigned integer.
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; //meaning the order in which the render takes place on a surface does not matter.
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; //, which will stretch the image to the monitors resolution.


	//Describing the SwapChain***********]
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	swapChainDesc.BufferDesc = bufferDesc; //����۸� �����ϴ� �κ�. ���� �����Ѱ� ��������
	swapChainDesc.SampleDesc.Count = 1; //��Ƽ���ø��� �����ϴ� ��.������ �ε巯�� ����? ������� �ȼ��� ������ �۰�.. �ȼ��� ���� ��ϰ��⶧���� (choppiness) ��ǻ�� �밢���� ������ ��������, ���� ���� �� �ִ�.
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //cpu�� ����ۿ� �����ϴ� ��� ����. �츮�� ����Ÿ������ ������ �Ұſ��� ���ɷ� ������
	swapChainDesc.BufferCount = 1; //�츮�� ����� ������� ��. ���� 1���� ���۸� �־ �����Ÿ��°� ����. 2�ؼ� Ʈ���� ���۸��̳� �� ���� �ص� ��������
	swapChainDesc.OutputWindow = hwnd; 
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; //
	//swapChainDesc.Flags =  //����� �ʿ����. ������ & Ǯ��ũ��ó�� ����͵��� �ػ󵵰� ���� �� DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH ���?

	//Creating the Device and SwapChain***********]
	//direct3d�� �߿��Լ��� D3D11CreateDeviceAndSwapChain()�� ����, ��ġ/��ġ����/����ü�� �����~~

	D3D11CreateDeviceAndSwapChain(
		NULL, //video adapter to use. ����Ʈ �Ѱ� ����Ұű⶧���� NULL�� ����
		D3D_DRIVER_TYPE_HARDWARE,// ���� �����Ұ���;; direct3d�� gpu�� ���� ������ ���̴�. ��� ��
		NULL,// HMODULE �ڵ��� ����Ͽ� ����Ʈ���� �����Ͷ���¡�� ������ DLL�� �����մϴ�.
		NULL,// 
		NULL, //Set NULL to use the highest features available.
		NULL,//
		D3D11_SDK_VERSION,//
		&swapChainDesc, //
		&SwapChain,//
		&device, //
		NULL, //
		&deviceContext //
		);

	//Creating the BackBuffer***********]
	//���� Ÿ�� �並 ���� �� ���� ����۸� �������. 
	ID3D11Texture2D* BackBuffer;
	SwapChain->GetBuffer(
		0, //UINT Buffer; swapChainDesc���� SwapEffect�� DXGI_SWAP_EFFECT_DISCARD�� ����. �츮�� ù��°���ۿ��� �����ϸ� �ȴ�.
		__uuidof(ID3D11Texture2D), // �ڷ� ���۸� �����ϱ� ���� �������̽� ������ ���� ���� ID�Դϴ�. �츮�� 2d �ؽ��� ���.
		(void**)&BackBuffer //�̰��� ���� �ۼ��� BackBuffer�� ���� �������Դϴ�. �� �����ʹ� �������� ǥ���Դϴ�.
	);

	//Creating the Render Target***********]

	//�츮�� �����
	//Ÿ�ٺ� ��Ʈ����.We set NULL to create a view that accesses all of the subresources in mipmap level 0.
	//This is a pointer to our ID3D11RenderTargetView interface, renderTargetView.

	device->CreateRenderTargetView(BackBuffer, NULL, &renderTargetView); 
	BackBuffer->Release();

	//���� Ÿ���� �����ϱ� ���� ��.
	//����Ÿ�ٺ並 �����ϱ� ���� �迭
	//depth stencil buffer�� ���� ������.
	deviceContext->OMSetRenderTargets(1, &renderTargetView, NULL);

	return true;
}
*/

void ReleaseObjects()
{
	//Release the COM objects we created.
	SwapChain->Release();
	device->Release();
	deviceContext->Release();
}

bool InitScene()
{

	HRESULT hr;
	//���̴� ������
	//Effects.fx�κ��� ���̴� ������. D3DX11CompileFromFile()�� ���� �� �� ����.
	
	hr = D3DCompileFromFile(L"Effects.fx", 0, 0, "VS", "vs_5_0", 0, 0, &VS_Buffer, &errorMessage);
	hr = D3DCompileFromFile(L"Effects.fx", 0, 0, "PS", "ps_5_0", 0, 0, &PS_Buffer, &errorMessage);

	hr = device->CreateVertexShader(VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), NULL, &VS);
	hr = device->CreatePixelShader(PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), NULL, &PS);
	
	
	//setting the shader
	deviceContext->VSSetShader(VS, 0, 0);
	deviceContext->PSSetShader(PS, 0, 0);

	//���߿� �𵨷� �� �κ�
	//Create the vertex buffer
	Vertex v[] =
	{
		Vertex(-0.5f, 0.5f, 1.f, 1.f,1.f,1.f,1.f),
		Vertex(0.5f, -0.5f, 1.f , 1.f,0.f,0.f,1.f),
		Vertex(-0.5f, -0.5f, 1.f , 0.f, 1.f,1.f, 1.f),
		
		Vertex(0.5f, 0.5f, 1.f , 1.f,0.f,0.f,1.f),
		Vertex(0.5f, -0.5f, 1.f , 1.f,0.f,0.f,1.f),
		Vertex(-0.5f, 0.5f, 1.f, 1.f,1.f,1.f,1.f),
	};



	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * 3;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	ZeroMemory(&vertexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	vertexBufferData.pSysMem = v;
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	hr = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &triangleVertBuffer);

	//setting the vertex buffer
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &triangleVertBuffer, &stride, &offset);

	//Creating the Input (Vertex) Layout
	hr = device->CreateInputLayout(layout, numElements, VS_Buffer->GetBufferPointer(),
		VS_Buffer->GetBufferSize(), &vertLayout);

	//setting the input vertex layout
	deviceContext->IASetInputLayout(vertLayout);

	//Setting the Primitive Topology ??
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	//Creating viewport

	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = Width;
	viewport.Height = Height;

	deviceContext->RSSetViewports(1, &viewport);
	
	return true;
}

void UpdateScene()
{
	red += colormodr * 0.00005f;
	green += colormodg * 0.000002f;
	blue += colormodb * 0.000001f;

	if (red >= 1.0f || red <= 0.0f)
		colormodr *= -1;
	if (green >= 1.0f || green <= 0.0f)
		colormodg *= -1;
	if (blue >= 1.0f || blue <= 0.0f)
		colormodb *= -1;
}

void DrawScene()
{
	//Clear our backbuffer to the updated color
	float color[4] = { red, green, blue , 1.0f };

	deviceContext->ClearRenderTargetView(renderTargetView,color);
	deviceContext->Draw(6,0);

	//Present the backbuffer to the screen
	SwapChain->Present(0, 0);
}

void CleanUp()
{
	//Release the COM objects we created
	SwapChain->Release();
	device->Release();
	deviceContext->Release();
	renderTargetView->Release();
	triangleVertBuffer->Release();
	VS->Release();
	PS->Release();
	VS_Buffer->Release();
	PS_Buffer->Release();
	vertLayout->Release();
	
}

/// <summary>
/// 
/// </summary>
/// <param name="hInstance"> This is the handle to our current application</param>
/// <param name="ShowWnd"> How the window should be displayed. Some common commands are SW_SHOWMAXIMIZED, SW_SHOW, SW_SHOWMINIMIZED.</param>
/// <param name="width"> Width of the window in pixels</param>
/// <param name="height"> Height of the window in pixels</param>
/// <param name="windowed"> False if the window is fullscreen and true if the window is windowed </param>
/// <returns></returns>
bool InitializeWindow(HINSTANCE hInstance, int ShowWnd, int width, int height, bool windowed) {


	//1. WNDCLASSEX �����
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);

	//CS_HREDRAW : ����Ŭ������ ������ ���� ����
	//CS_VREDRAW : �������� ���̰� ���ϰų� �������� �������� �ٽ� �׷���
	wc.style = CS_HREDRAW | CS_VREDRAW;

	wc.lpfnWndProc = WndProc;

	//WNDCLASSEX ���� �Ŀ� �߰����� �޸� �Ҵ� ��
	wc.cbClsExtra = NULL;
	//window instance �Ŀ� �߰����� �޸� �Ҵ� ��
	wc.cbWndExtra = NULL;

	//GetModuleHandle()
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = WndClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

	//2. ���
	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, L"Error registering class", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}
	
	//3. �����. This is the part where we create our window!!

	hwnd = CreateWindowEx(
		NULL,
		WndClassName, 
		L"Geometry Dash",
		WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT, CW_USEDEFAULT, 
		width, 
		height, 
		NULL, 
		NULL, 
		hInstance,
		NULL);


	if (!hwnd) {
		MessageBox(NULL, L"Error creating window", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	//4.â�� �����ְ� ������Ʈ
	ShowWindow(hwnd, ShowWnd);
	UpdateWindow(hwnd);
	
	return true;
}

int messageloop() {

	//1. MSG ����ü ��ü �����. This will hold the message's information.
	MSG msg;

	ZeroMemory(&msg, sizeof(MSG)); 

	/*
LPMSG lpMsg ; msg ������
HWND hwnd ; �޼����� ������ �������� �ڵ�.
UINT wMsgFilterMin ; �޼��� ���� ��(message quere ����ϴ� ��) ù �޼����� ���� ����Ǿ����� Ȯ���ϰ�,
wMsgFilterMin�� wMsgFilterMax�� �Ѵ� 0�̶��, PeekMessage�� ��� �޼����� ������ ���̴�.
UINT wMsgFilterMax ; �޼��� ť �� ������ ���� Ȯ���Ѵ�.
UINT wRemoveMsg ; �޼����� ��� �����Ǵ����� ���� ����. �츮�� PM_REMOVE�� �����ϸ� �޼����� ���� ��, ������ ���̴�.
*/

	//while there is a message...
	while (true) 
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
		{
			if (msg.message == WM_QUIT) {
				break;
			}

			TranslateMessage(&msg); //Ű������ virtual key�� characters�� ��ȯ�ϴ� �Ͱ��� ����
			DispatchMessage(&msg); //������ �޼�����. �츮�� window precedure����.(WndProc)
			
		}

		else {
			//run game code

			UpdateScene();
			DrawScene();
		}
	}

	return (int)msg.wParam;
}

/// <summary>
/// 
/// </summary>
/// <param name="hwnd">
/// the handle to the window that got the message.
/// </param>
/// <param name="msg">
/// the contents of the message.
/// </param>
/// <param name="wParam", "lParam">
/// wParam and lParam are extra information about the message.
/// we will be using wParam to detect keyboard input.
/// </param>
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			if(MessageBox(0, L"Are you sure you want to exit?", L"Really?",MB_YESNO | MB_ICONQUESTION) == IDYES) 
				DestroyWindow(hwnd);
		}
	return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
	return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}


/*
* 
* <PipeLine> *
* 
1. Input Assembler (IA) Stage
2. Vertex Shader (VS) Stage //Programmable
3. Hull Shader (HS) Stage	//Programmable
4. Tesselator Shader (TS) Stage
5. Domain Shader (DS) Stage	//Programmable
6. Geometry Shader (GS) Stage	//Programmable
7. Stream Output (SO) Stage
8. Rasterizer (RS) Stage
9. Pixel Shader (PS) Stage	//Programmable
10. Output Merger (OM) Stage


<1. IA stage >
- fixed function stage. �ڵ��� �� ����
- ������Ʈ�� ������, ������ indices(�ѱ���� ������)�� �д´�. 
- �� �����͸� �̿��� primitives ������ �׸���. 

IA�� ���𰡸� ������ ��, ���۸����/Primitive Topology(??) �����ϱ�, Input Layout, and active buffer �� �۾��� �ʿ�.

�ΰ��� ���۸� ����Ŵ�. vertex & index ����. ���۸� �����, D3D11_BUFFER_DESC�� ä������ ����.

���۳� ���۵��� ����� �� ��, �츮�� **input-layout object**�� ������ �Ѵ�. 
vertex ������ �����ϰ� �ִ°�, �׸��� �츮�� ���ؽ� ������ �ִ� �� ��Ҹ� �����ִ� ����.

D3D11_INPUT_ELEMENT_DESC�� ���� �������. ��ǲ��� �ϳ��� ���ؽ� ���� ��ü�� ���� ����.

���� ���,,

//The vertex Structure
struct Vertex {
	XMFLOAT3 pos;
	XMFLOAT4 color;
};

D3D11_INPUT_ELEMENT_DESC layout[] = {
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR", 0 , DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA,0}
};
*/

/*
<2. VS Stage >
-���α׷��� ����
-VS Stage�� ��� ������ IA�� ������ ������ �� ��ġ�� �����Դϴ�.
-�׷��� ��� ������ VS�� ����Ѵ�.
-VS�� �̵�, ũ�� ����, ����, �ؽ��� ��� �Ҽ�����
-���α׷��� ������ ������ �ʿ䰡 ���� ��쿡�� ������������ �۵��Ϸ��� �׻� ���� ���̴��� �����ؾ� �մϴ�.
-HLSL ���� �׷�����.
-

https://www.braynzarsoft.net/viewtutorial/q16390-4-begin-drawing
���ϴٰ� �Ӹ� ������. �̾ �ؾ���;;
*/


