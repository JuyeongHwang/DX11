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

//direct3d를 초기화할 때 사용
bool InitializeDirect3d11App(HINSTANCE hInstance);
//메모리릭 방지
void ReleaseObjects();
bool InitScene();
//change our scene on a per-frame basis
void UpdateScene();
void DrawScene();

void CleanUp();

//window 만들기 성공 : true, 아니면 false를 반환할 것임.
bool InitializeWindow(HINSTANCE hInstance, int ShowWnd, int width, int height, bool windowed);

//프로그램이 돌아갈 곳.
int messageloop();

// window callback procedure도 설정해줘야함.
// 윈도우의 메세지를 처리하는 곳입니다
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
Direct3D를 정의하기 위한 첫번째 COM 인터페이스 오브젝트는 바로 SwapChain!
스왑체인은 백버퍼와 프론트버퍼를 서로 바꾸는데 이용함.
더블 버퍼링이라고 불린다.
우리의 씬을 화면에 렌더할때 우리는 백버퍼를 렌더링하고있는다. 그 다음 다 그리면 백버퍼를 모니터에 보여준다.
다른 말로는, 우리는 스캔라인들을 가질것이다.

그 다음 Interface는 바로 우리의 하드웨어 디바이스 GPU 씨가 되겠다.
이제 directX11 부터는 새로운 인터페이스가 생겼다. ID3D11Device라는 아이인데,
이는 멀티스레딩을 위해 두개로 나뉜다. 우리는 ID3D11DeviceContext 인터페이스 오브젝트를
모든 렌더링 메소드를 부를 때 사용할 것이다. ID3D11Device는 렌더링에 필요없는 나머지 메소드들을 부를 때 이용할 것임.

DirectX11은 새로운 멀티스레딩 특징을 가지고, 어플리케이션의 속도를 빠르게 하곤했다.
너가 어떤 메모리를 로딩하거나, 어떤 모델이나 오브젝트를 만들 때 너는 ID3D11Device object를 부를것이다.
오브젝트나 모델이 불러지거나 만들어질때는 너는 디바이스콘텍스트 인터페이스를 불러, 너의 씬에 물체가 계속 렌더링 되도록 할 것이다.

그래서 이제 우리는 서로 다른 렌더 타겟 뷰 인터페이스 오브젝트를 가진다.
일반적으로 우리는 2d textrue나 backbuffer를 스크린에 직접적으로 쓰지 않고 render target view에 대신 그린다.
그리고 이 텍스쳐는 렌더타겟의 파이프라인에서 아웃풋 멀절 스테이지로 보내지고 여기서 그리게 될 것이다.
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
	//에러 체크를 위한 변수
	//Describing the BackBuffer.***********]
	DXGI_MODE_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

	bufferDesc.Width = Width; //resolution
	bufferDesc.Height = Height; //resolution
	bufferDesc.RefreshRate.Numerator = 60; //hz 설정
	bufferDesc.RefreshRate.Denominator = 1; //hz 설정. 60/1
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //32bit unsigned integer.
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; //meaning the order in which the render takes place on a surface does not matter.
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; //, which will stretch the image to the monitors resolution.


	//Describing the SwapChain***********]
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	swapChainDesc.BufferDesc = bufferDesc; //백버퍼를 묘사하는 부분. 위에 정의한거 가져오기
	swapChainDesc.SampleDesc.Count = 1; //멀티샘플링을 묘사하는 곳.엣지의 부드러움 정도? 모니터의 픽셀을 무한히 작게.. 픽셀은 작은 블록같기때문에 (choppiness) 컴퓨터 대각선의 엣지는 지글지글, 고르지 못할 수 있다.
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //cpu가 백버퍼에 접근하는 방식 결정. 우리는 렌더타겟으로 렌더링 할거여서 저걸로 쓸거임
	swapChainDesc.BufferCount = 1; //우리가 사용할 백버퍼의 수. 보통 1더블 버퍼만 있어도 버벅거리는거 없음. 2해서 트리플 버퍼링이나 더 많이 해도 ㄱㅊㄱㅊ
	swapChainDesc.OutputWindow = hwnd; 
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; //
	//swapChainDesc.Flags =  //현재는 필요없음. 윈도우 & 풀스크린처럼 모니터들의 해상도가 변할 때 DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH 사용?

	//Creating the Device and SwapChain***********]
	//direct3d의 중요함수인 D3D11CreateDeviceAndSwapChain()를 통해, 장치/장치문맥/스왑체인 만들거~~

	D3D11CreateDeviceAndSwapChain(
		NULL, //video adapter to use. 디폴트 한개 사용할거기때문에 NULL로 세팅
		D3D_DRIVER_TYPE_HARDWARE,// 뭘로 구현할건지;; direct3d는 gpu에 의해 구현될 것이다. 라는 말
		NULL,// HMODULE 핸들을 사용하여 소프트웨어 래스터라이징을 구현할 DLL에 연결합니다.
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
	//렌더 타겟 뷰를 만들 때 사용될 백버퍼를 만들거임. 
	ID3D11Texture2D* BackBuffer;
	SwapChain->GetBuffer(
		0, //UINT Buffer; swapChainDesc에서 SwapEffect를 DXGI_SWAP_EFFECT_DISCARD로 세팅. 우리는 첫번째버퍼에만 접근하면 된다.
		__uuidof(ID3D11Texture2D), // 뒤로 버퍼를 변경하기 위한 인터페이스 유형에 대한 참조 ID입니다. 우리는 2d 텍스쳐 사용.
		(void**)&BackBuffer //이것은 위에 작성한 BackBuffer에 대한 포인터입니다. 이 포인터는 렌더링할 표면입니다.
	);

	//Creating the Render Target***********]

	//우리의 백버퍼
	//타겟뷰 스트럭쳐.We set NULL to create a view that accesses all of the subresources in mipmap level 0.
	//This is a pointer to our ID3D11RenderTargetView interface, renderTargetView.

	device->CreateRenderTargetView(BackBuffer, NULL, &renderTargetView); 
	BackBuffer->Release();

	//렌더 타겟을 연결하기 위한 수.
	//렌더타겟뷰를 연결하기 위한 배열
	//depth stencil buffer에 대한 포인터.
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
	//셰이더 컴파일
	//Effects.fx로부터 셰이더 컴파일. D3DX11CompileFromFile()을 통해 할 수 있음.
	
	hr = D3DCompileFromFile(L"Effects.fx", 0, 0, "VS", "vs_5_0", 0, 0, &VS_Buffer, &errorMessage);
	hr = D3DCompileFromFile(L"Effects.fx", 0, 0, "PS", "ps_5_0", 0, 0, &PS_Buffer, &errorMessage);

	hr = device->CreateVertexShader(VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), NULL, &VS);
	hr = device->CreatePixelShader(PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), NULL, &PS);
	
	
	//setting the shader
	deviceContext->VSSetShader(VS, 0, 0);
	deviceContext->PSSetShader(PS, 0, 0);

	//나중에 모델로 뺄 부분
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


	//1. WNDCLASSEX 만들기
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);

	//CS_HREDRAW : 더블클릭으로 윈도우 조절 가능
	//CS_VREDRAW : 윈도우의 높이가 변하거나 수직으로 움직여도 다시 그려줌
	wc.style = CS_HREDRAW | CS_VREDRAW;

	wc.lpfnWndProc = WndProc;

	//WNDCLASSEX 생성 후에 추가적인 메모리 할당 수
	wc.cbClsExtra = NULL;
	//window instance 후에 추가적인 메모리 할당 수
	wc.cbWndExtra = NULL;

	//GetModuleHandle()
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

	//4.창을 보여주고 업데이트
	ShowWindow(hwnd, ShowWnd);
	UpdateWindow(hwnd);
	
	return true;
}

int messageloop() {

	//1. MSG 구조체 객체 만들기. This will hold the message's information.
	MSG msg;

	ZeroMemory(&msg, sizeof(MSG)); 

	/*
LPMSG lpMsg ; msg 포인터
HWND hwnd ; 메세지를 보내는 윈도우의 핸들.
UINT wMsgFilterMin ; 메세지 범위 중(message quere 얘기하는 듯) 첫 메세지의 값이 조사되었는지 확인하고,
wMsgFilterMin과 wMsgFilterMax가 둘다 0이라면, PeekMessage는 모든 메세지를 조사할 것이다.
UINT wMsgFilterMax ; 메세지 큐 중 마지막 값을 확인한다.
UINT wRemoveMsg ; 메세지가 어떻게 관리되는지에 관한 변수. 우리가 PM_REMOVE로 세팅하면 메세지는 읽힌 후, 삭제될 것이다.
*/

	//while there is a message...
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
- fixed function stage. 코딩할 수 없음
- 지오매트릭 데이터, 정점과 indices(한국어로 뭐더라)를 읽는다. 
- 이 데이터를 이용해 primitives 도형을 그린다. 

IA에 무언가를 보내기 전, 버퍼만들기/Primitive Topology(??) 세팅하기, Input Layout, and active buffer 등 작업이 필요.

두가지 버퍼를 만들거다. vertex & index 버퍼. 버퍼를 만들고, D3D11_BUFFER_DESC를 채워넣을 거임.

버퍼나 버퍼들을 만들고 난 후, 우리는 **input-layout object**를 만들어야 한다. 
vertex 구조가 구성하고 있는것, 그리고 우리의 버텍스 구조에 있는 각 요소를 말해주는 역할.

D3D11_INPUT_ELEMENT_DESC을 통해 만들고임. 인풋어레이 하나에 버텍스 구조 전체가 들어가는 구조.

예를 들어,,

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
-프로그래밍 가능
-VS Stage는 모든 정점이 IA에 원형이 조립된 후 거치는 과정입니다.
-그려진 모든 정점은 VS에 통과한다.
-VS로 이동, 크기 조정, 조명, 텍스쳐 등등 할수있음
-프로그램의 정점을 수정할 필요가 없는 경우에도 파이프라인이 작동하려면 항상 정점 셰이더를 구현해야 합니다.
-HLSL 언어로 그려진다.
-

https://www.braynzarsoft.net/viewtutorial/q16390-4-begin-drawing
더하다간 머리 터질듯. 이어서 해야지;;
*/


