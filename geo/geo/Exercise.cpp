#include "pch.h"

ID3D11BlendState* Transparency;
ID3D11RasterizerState* CCWcullMode;
ID3D11RasterizerState* CWcullMode;
ID3D11RasterizerState* noCull;


void InitD2DScreenTexture();
void RenderText(wstring text, float left, float top, float right, float bottom);
//*** 전역변수 ***//
const int Width = 1024;
const int Height = 720;


ID3D11Buffer* CubeVertexBuffer = nullptr;
ID3D11Buffer* triangleVertexBuffer = nullptr;
ID3D11Buffer* CubeIndexBuffer = nullptr;
ID3D11Buffer* triangleIndexBuffer = nullptr;
ID3D11Buffer* backgroundVertexBuffer = nullptr;
ID3D11Buffer* backgroundIndexBuffer = nullptr;
ID3D11Buffer* circleVertexBuffer = nullptr;
ID3D11Buffer* circleIndexBuffer = nullptr;

ID3D11VertexShader* vertexShader;
ID3D11PixelShader* pixelShader;
ID3D11InputLayout* layout;

// *** frame *** //
double countsPerSecond = 0.0;
__int64 CounterStart = 0;

int frameCount = 0;
int fps = 0;

__int64 frameTimeOld = 0;
double frameTime;

void StartTimer();
double GetTime();
double GetFrameTime();
// *********** 
ID3D11Buffer* cbPerObjectBuffer;
XMMATRIX WVP;
XMMATRIX cubeWorld; 
XMMATRIX bgWorld;
XMMATRIX triangleWorld;
XMMATRIX triangleWorld2;
XMMATRIX circleWorld;
XMMATRIX camView;
XMMATRIX camProjection;



struct cbPerObject
{
	XMMATRIX  WVP;
};
cbPerObject cbPerObj;

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
	//{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};
unsigned int numElements = ARRAYSIZE(Playout);

// *** model *** ///

struct VertexType {
	VertexType() {} //기본 생성자. 없으면 오류난다.
	VertexType(float x, float y, float z,
		float u, float v)
		: pos(x, y, z), texCoord(u, v){}

	XMFLOAT3 pos;
	XMFLOAT2 texCoord;
};

// *** camera *** //

XMFLOAT3 camPosition = XMFLOAT3(0.0f, 0.f, -8.0f);
XMFLOAT3 camRotation = XMFLOAT3(0.0f, 0.0f, 0.0f);

XMMATRIX Rotation;
XMMATRIX Scale;
XMMATRIX Translation;

// **** texture - shader**** //
int texheight = 0;
int texwidth = 0;
unsigned char* targaData = 0;

struct TargaHeader
{
	unsigned char data1[12];
	unsigned short width;
	unsigned short height;
	unsigned char bpp;
	unsigned char data2;
};

bool LoadTarga(char* name);
ID3D11Texture2D* cubeTexture = 0;
ID3D11ShaderResourceView* cubetextureView = 0;
ID3D11SamplerState* CubesTexSamplerState = 0;

ID3D11Texture2D* bgTexture = 0;
ID3D11ShaderResourceView* bgtextureView = 0;
ID3D11SamplerState* BgTexSamplerState = 0; 

// *** direct3D *** ///
bool InitializeDirect3d11App(HINSTANCE hInstance);
void ReleaseObjects();

void UpdateScene(double time);
void DrawScene();
void shaderPipeline();

// *** window *** ///
bool InitializeWindow(HINSTANCE hInstance, int ShowWnd, int width, int height, bool windowed);
int messageloop();
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lParam);


#include "DirectClass.h"
DirectClass* dx = new DirectClass;
///****< Main Windows Fuction >****///
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

	
	if (!InitializeWindow(hInstance, nShowCmd, Width, Height, true)) {

		MessageBox(0, L"Window Initialization - Failed", L"Error", MB_OK);
		return 0;
	}

	if (!dx->InitializeDirect(hwnd, Width, Height)) {
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

bool InitializeTriangle() {

	VertexType vertices2[] = {
		VertexType(0.5f, 1.0f, -0.5f, 0.1f, 0.1f),
		VertexType(1.0f, +.0f, -1.0f, 0.1f, 0.1f),
		VertexType(+0.0f, +0.0f, -1.0f,0.1f, 0.1f),

		VertexType(+.0f, .0f, .0f ,0.1f, 0.1f),
		VertexType(1.0f, .0f, .0f ,0.1f, 0.1f),

		VertexType(-1.0f, +1.0f, +1.0f,0.1f, 0.1f),
		VertexType(+1.0f, +1.0f, +1.0f,0.1f, 0.1f),
		VertexType(+1.0f, -1.0f, +1.0f,0.1f, 0.1f)
		
	};
	int vertexCount = sizeof(vertices2) / sizeof(VertexType);
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount; //here
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource 구조에 정점 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA vertexData;
	ZeroMemory(&vertexData, sizeof(vertexData));
	vertexData.pSysMem = vertices2; //here
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	// 이제 정점 버퍼를 만듭니다.
	
	if (FAILED(dx->getDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &triangleVertexBuffer)))
	{
		return false;
	}

	DWORD indiceTriangle[] = {
		// front face
		0, 1, 2,

		// left face
		0, 2, 3,
		// back face
		0,3, 4,

		// right face
		0,4,1,

		// bottom face
		1,3,2,
		1,4,3
		
	};

	int indexCount = sizeof(indiceTriangle) / sizeof(DWORD);
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * indexCount; //here
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 인덱스 데이터를 가리키는 보조 리소스 구조체를 작성합니다.
	D3D11_SUBRESOURCE_DATA indexData;
	ZeroMemory(&indexData, sizeof(D3D11_SUBRESOURCE_DATA));
	indexData.pSysMem = indiceTriangle; //here
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼를 생성합니다.
	if (FAILED(dx->getDevice()->CreateBuffer(&indexBufferDesc, &indexData, &triangleIndexBuffer)))
	{
		return false;
	}

}

float total_game_time = 0.0f;
float triangleX = 4.0f;
void updateTriangle() {

	XMVECTOR rotaxis = XMVectorSet(0.0f, .0f, 1.0f, 0.0f);

	Rotation = XMMatrixRotationAxis(rotaxis, 0.0f);

	triangleWorld = XMMatrixIdentity();
	Translation = XMMatrixTranslation(triangleX, -0.7f, 0.f);
	Scale = XMMatrixTranslation(0.5f, 0.3f, 0.5f);
	triangleWorld = Rotation*Scale*Translation;

	triangleWorld2 = XMMatrixIdentity();
	Translation = XMMatrixTranslation(15.0f, -0.7f,-0.f);
	Scale = XMMatrixTranslation(0.5f, 0.3f, 0.5f);
	triangleWorld2 = Rotation * Scale * Translation;
}
void drawTriangle() {
	updateTriangle();
	UINT stride = sizeof(VertexType);
	UINT offset = 0;
	
	
	//move
	dx->getDevContext()->IASetVertexBuffers(0, 1, &triangleVertexBuffer, &stride, &offset);
	dx->getDevContext()->IASetIndexBuffer(triangleIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	WVP = triangleWorld * camView * camProjection;
	cbPerObj.WVP = XMMatrixTranspose(WVP);
	dx->getDevContext()->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
	dx->getDevContext()->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
	dx->getDevContext()->DrawIndexed(18, 0, 0); //here


	WVP = triangleWorld2 * camView * camProjection;
	cbPerObj.WVP = XMMatrixTranspose(WVP);
	dx->getDevContext()->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
	dx->getDevContext()->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
	dx->getDevContext()->DrawIndexed(18, 0, 0); //here
}

bool InitializeCube() {

	VertexType vertices[] = {
		// Front Face
		VertexType(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
		VertexType(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f),
		VertexType(1.0f,  1.0f, -1.0f, 1.0f, 0.0f),
		VertexType(1.0f, -1.0f, -1.0f, 1.0f, 1.0f),

		// Back Face
		VertexType(-1.0f, -1.0f, 1.0f, 1.0f, 1.0f),
		VertexType(1.0f, -1.0f, 1.0f, 0.0f, 1.0f),
		VertexType(1.0f,  1.0f, 1.0f, 0.0f, 0.0f),
		VertexType(-1.0f,  1.0f, 1.0f, 1.0f, 0.0f),

		// Top Face
		VertexType(-1.0f, 1.0f, -1.0f, 1.f, 1.f),
		VertexType(-1.0f, 1.0f,  1.0f, 0.0f, 1.f),
		VertexType(1.0f, 1.0f,  1.0f, 0.0f, 0.0f),
		VertexType(1.0f, 1.0f, -1.0f, 1.f, 1.f),

		// Bottom Face
		VertexType(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f),
		VertexType(1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
		VertexType(1.0f, -1.0f,  1.0f, 0.0f,0.0f),
		VertexType(-1.0f, -1.0f,  1.0f, 1.0f, 1.f),

		// Left Face
		VertexType(-1.0f, -1.0f,  1.0f, 0.0f, 1.0f),
		VertexType(-1.0f,  1.0f,  1.0f, 0.0f, 0.0f),
		VertexType(-1.0f,  1.0f, -1.0f, 1.0f, 0.0f),
		VertexType(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f),

		// Right Face
		VertexType(1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
		VertexType(1.0f,  1.0f, -1.0f, 0.0f, 0.0f),
		VertexType(1.0f,  1.0f,  1.0f, 1.0f, 0.0f),
		VertexType(1.0f, -1.0f,  1.0f, 1.0f, 1.0f),

	};
	vertexCount = sizeof(vertices)/sizeof(VertexType);
	

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
	if (FAILED(dx->getDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &CubeVertexBuffer)))
	{
		return false;
	}

	DWORD indices[] = {
		// Front Face
		0,  1,  2,
		0,  2,  3,

		// Back Face
		4,  5,  6,
		4,  6,  7,

		// Top Face
		8,  9, 10,
		8, 10, 11,

		// Bottom Face
		12, 13, 14,
		12, 14, 15,

		// Left Face
		16, 17, 18,
		16, 18, 19,

		// Right Face
		20, 21, 22,
		20, 22, 23

	};
	indexCount = sizeof(indices)/sizeof(DWORD);

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
	if (FAILED(dx->getDevice()->CreateBuffer(&indexBufferDesc, &indexData, &CubeIndexBuffer)))
	{
		return false;
	}	

}

int i = 0;
bool isRotate = false;
float cubeX = -5.0f;
bool isSpace = false;
float rot = 0.0f;
float jumpty = 0.0f;
bool isStart = false;
void settingCamera();

void updateCube(double time) {

	if (!isStart) {
		return;
	}
	camPosition.y = 3.f;
	camPosition.z = -15.f;
	
	cubeX += 7.f * time;
	settingCamera();
	XMVECTOR rotaxis = XMVectorSet(0.0f, .0f, 1.0f, 0.0f);
	///
	camPosition.x = cubeX;
	
	if (isRotate) {
		rot += 1.5708f * time * 4.f;
		if (rot >= 1.5708f * (i + 1)) {
			rot = 1.5708f * (i + 1);
			isRotate = false;
			i++;
		}
	}
	if (isSpace) {

		jumpty += 15.f * time;
		//cubeX += 7.f * time;
		//Define cube2's world space matrix
		Rotation = XMMatrixRotationAxis(rotaxis, -rot);
	}
	else {
		Rotation = XMMatrixRotationAxis(rotaxis, -rot);
		jumpty -= 15.f * time;

		if (jumpty <= 0.0f) {
			jumpty = 0.f;
		}
		//else { cubeX += 7.f * time; }
	}
	if (jumpty > 2.f) {
		isSpace = false;
	}
	

	cubeWorld = XMMatrixIdentity();
	Scale = XMMatrixScaling(0.4f, 0.4f, 0.4f);
	Translation = XMMatrixTranslation(cubeX, jumpty, 0.f);

	cubeWorld = Rotation * Scale * Translation ;

}
void drawCube() {

	UINT stride = sizeof(VertexType);
	UINT offset = 0;
	dx->getDevContext()->IASetVertexBuffers(0, 1, &CubeVertexBuffer, &stride, &offset);
	dx->getDevContext()->IASetIndexBuffer(CubeIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	dx->getDevContext()->PSSetShaderResources(0, 1, &cubetextureView);
	dx->getDevContext()->PSSetSamplers(0, 1, &CubesTexSamplerState);


	WVP = cubeWorld * camView * camProjection;
	cbPerObj.WVP = XMMatrixTranspose(WVP);

	dx->getDevContext()->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
	dx->getDevContext()->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);


	dx->getDevContext()->RSSetState(CWcullMode);
	dx->getDevContext()->DrawIndexed(indexCount, 0, 0);

}

bool InitializeCircle() {
	VertexType vertices[] = {
		// Front Face
		VertexType(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
		VertexType(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f),
		VertexType(1.0f,  1.0f, -1.0f, 1.0f, 0.0f),
		VertexType(1.0f, -1.0f, -1.0f, 1.0f, 1.0f),

	};
	int bgvertexCount = sizeof(vertices) / sizeof(VertexType);


	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * bgvertexCount;
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
	if (FAILED(dx->getDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &circleVertexBuffer)))
	{
		return false;
	}

	DWORD indices[] = {
		// Front Face
		0,  1,  2,
		0,  2,  3,
	};
	int bgindexCount = sizeof(indices) / sizeof(DWORD);

	// 정적 인덱스 버퍼의 구조체를 설정합니다.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * bgindexCount;
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
	if (FAILED(dx->getDevice()->CreateBuffer(&indexBufferDesc, &indexData, &circleIndexBuffer)))
	{
		return false;
	}
	return true;
}
int a = 0;
void updateCircle() {
	a += 3.f;
	XMVECTOR rotaxis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	circleWorld = XMMatrixIdentity();
	Rotation = XMMatrixRotationAxis(rotaxis, 0);
	Scale = XMMatrixScaling(Width / 90, 1.8f, 0.4f);
	Translation = XMMatrixTranslation(cubeX, -3.f, 4.f);

	circleWorld = Rotation * Scale * Translation;
}
void drawCircle() {
	updateCircle();

	UINT stride = sizeof(VertexType);
	UINT offset = 0;
	dx->getDevContext()->IASetVertexBuffers(0, 1, &circleVertexBuffer, &stride, &offset);
	dx->getDevContext()->IASetIndexBuffer(circleIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	dx->getDevContext()->PSSetShaderResources(0u, 1u, &bgtextureView);
	dx->getDevContext()->PSSetSamplers(0, 1, &BgTexSamplerState);

	WVP = bgWorld * camView * camProjection;
	cbPerObj.WVP = XMMatrixTranspose(WVP);

	dx->getDevContext()->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
	dx->getDevContext()->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);

	dx->getDevContext()->RSSetState(noCull);
	dx->getDevContext()->DrawIndexed(6, 0, 0);
}


bool InitializeBg() {
	VertexType vertices[] = {
		// Front Face
		VertexType(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
		VertexType(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f),
		VertexType(1.0f,  1.0f, -1.0f, 1.0f, 0.0f),
		VertexType(1.0f, -1.0f, -1.0f, 1.0f, 1.0f),

	};
	int bgvertexCount = sizeof(vertices) / sizeof(VertexType);


	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * bgvertexCount;
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
	if (FAILED(dx->getDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &backgroundVertexBuffer)))
	{
		return false;
	}

	DWORD indices[] = {
		// Front Face
		0,  1,  2,
		0,  2,  3,
	};
	int bgindexCount = sizeof(indices) / sizeof(DWORD);

	// 정적 인덱스 버퍼의 구조체를 설정합니다.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * bgindexCount;
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
	if (FAILED(dx->getDevice()->CreateBuffer(&indexBufferDesc, &indexData, &backgroundIndexBuffer)))
	{
		return false;
	}
	return true;
}
int aa = 0;
void updateBg() {
	aa += 3.f;
	XMVECTOR rotaxis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	bgWorld = XMMatrixIdentity();
	Rotation = XMMatrixRotationAxis(rotaxis, 0);
	Scale = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	Translation = XMMatrixTranslation(cubeX + 3.0f, 0.f, 0.f);

	bgWorld = Rotation * Scale * Translation;
}
void drawBg() {
	updateBg();
	
	UINT stride = sizeof(VertexType);
	UINT offset = 0;
	dx->getDevContext()->IASetVertexBuffers(0, 1, &backgroundVertexBuffer, &stride, &offset);
	dx->getDevContext()->IASetIndexBuffer(backgroundIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	dx->getDevContext()->PSSetShaderResources(0u, 1u, &bgtextureView);
	dx->getDevContext()->PSSetSamplers(0, 1, &BgTexSamplerState);

	WVP = bgWorld * camView * camProjection;
	cbPerObj.WVP = XMMatrixTranspose(WVP);

	dx->getDevContext()->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
	dx->getDevContext()->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);

	dx->getDevContext()->RSSetState(noCull);
	dx->getDevContext()->DrawIndexed(6, 0, 0);
}


bool LoadTarga(char* name) {

	FILE* filePtr;
	TargaHeader targaFileHeader;

	int error = fopen_s(&filePtr, name, "rb");
	if (error != 0) { return false; }

	int count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1) { return false; }


	texheight = (int)targaFileHeader.height;
	texwidth = (int)targaFileHeader.width;
	int bpp = (int)targaFileHeader.bpp;

	if (bpp != 32) { return false; }


	int imageSize = texwidth * texheight * 4;
	//unsigned char* targaImage = new unsigned char[imageSize];
	targaData = new unsigned char[imageSize];
	if (!targaData) { return false; }

	count = (unsigned int)fread(targaData, 1, imageSize, filePtr);
	if (count != imageSize) { return false; }

	// Close the file.
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	/*
	wstring ws = to_wstring(count);
	LPCWSTR result = ws.c_str();
	MessageBox(hwnd, result, L"Error", MB_OK);
*/

	return true;
}

#include "DDSTextureLoader11.h"

ID3D11Resource* skytexture;
ID3D11ShaderResourceView* skyRV;
const wchar_t* fileName = (wchar_t*)"skymap.dds";

bool loadDDS() {
	HRESULT hr;
	hr = CreateDDSTextureFromFile(dx->getDevice(),fileName, nullptr, &skyRV);
	return true;
}
bool InitializeTextrue() {
	ID3D11Texture2D* SMTexture = 0;
	wstring ws = to_wstring(texheight);
	LPCWSTR lp = ws.c_str();
	if (!loadDDS()) {
		return false;
	}

	return true;
}
bool InitializeTexture() {
	//InitializeTextrue();
	D3D11_TEXTURE2D_DESC textureDesc;
	unsigned int rowPitch;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	if (!LoadTarga((char*)"toppng.tga")) {
		return false;
	}


	// Setup the description of the texture.
	textureDesc.Height = texheight;
	textureDesc.Width = texwidth;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	D3D11_SUBRESOURCE_DATA srd;

	// Create the empty texture.
	HRESULT hResult = dx->getDevice()->CreateTexture2D(&textureDesc, NULL, &cubeTexture);
	if (FAILED(hResult))
	{
		return false;
	}


	// Set the row pitch of the targa image data.
	rowPitch = (texwidth * 4) * sizeof(unsigned char);

	// Copy the targa image data into the texture.
	dx->getDevContext()->UpdateSubresource(cubeTexture, 0, NULL, targaData, rowPitch, 0);

	// Setup the shader resource view description.
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;



	// Create the shader resource view for the texture.
	hResult = dx->getDevice()->CreateShaderResourceView(cubeTexture, &srvDesc, &cubetextureView);
	if (FAILED(hResult))
	{
		return false;
	}

	// Generate mipmaps for this texture.
	dx->getDevContext()->GenerateMips(cubetextureView);

	// Release the targa image data now that the image data has been loaded into the texture.
	delete[] targaData;

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//Create the Sample State
	dx->getDevice()->CreateSamplerState(&sampDesc, &CubesTexSamplerState);

	// ******* background ********** //
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	rowPitch = NULL;
	ZeroMemory(&srvDesc, sizeof(srvDesc));

	if (!LoadTarga((char*)"backGround.tga")) {
		return false;
	}

	// Setup the description of the texture.
	textureDesc.Height = texheight;
	textureDesc.Width = texwidth;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// Create the empty texture.
	hResult = dx->getDevice()->CreateTexture2D(&textureDesc, NULL, &bgTexture);
	if (FAILED(hResult))
	{
		return false;
	}
	// Set the row pitch of the targa image data.
	rowPitch = (texwidth * 4) * sizeof(unsigned char);

	// Copy the targa image data into the texture.
	dx->getDevContext()->UpdateSubresource(bgTexture, 0, NULL, targaData, rowPitch, 0);

	// Setup the shader resource view description.
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view for the texture.
	hResult = dx->getDevice()->CreateShaderResourceView(bgTexture, &srvDesc, &bgtextureView);
	if (FAILED(hResult))
	{
		return false;
	}

	// Generate mipmaps for this texture.
	dx->getDevContext()->GenerateMips(bgtextureView);

	// Release the targa image data now that the image data has been loaded into the texture.
	delete[] targaData;
	targaData = 0;


	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//Create the Sample State
	dx->getDevice()->CreateSamplerState(&sampDesc, &BgTexSamplerState);
}

bool InitializeShader() {
	

	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer; //from hlsl
	ID3D10Blob* pixelShaderBuffer; //from hlsl
	
	InitializeCube();
	InitializeTriangle();
	InitializeBg();
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
	result = dx->getDevice()->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &vertexShader);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not create the vertex shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the pixel shader from the buffer.
	result = dx->getDevice()->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &pixelShader);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not create the pixel shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the vertex input layout.
	result = dx->getDevice()->CreateInputLayout(Playout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &layout);
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
	

	D3D11_BUFFER_DESC cbbd;
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));
	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(cbPerObject);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	result = dx->getDevice()->CreateBuffer(&cbbd, NULL, &cbPerObjectBuffer);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not create the cbPerObjectBuffer object.", L"Error", MB_OK);
		return false;
	}

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory(&rtbd, sizeof(rtbd));

	rtbd.BlendEnable = false;
	rtbd.SrcBlend = D3D11_BLEND_SRC_COLOR;
	rtbd.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = true;
	blendDesc.RenderTarget[0] = rtbd;

	dx->getDevice()->CreateBlendState(&blendDesc, &Transparency);

	D3D11_RASTERIZER_DESC cmdesc;
	ZeroMemory(&cmdesc, sizeof(cmdesc));

	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_BACK;

	cmdesc.FrontCounterClockwise = true;
	dx->getDevice()->CreateRasterizerState(&cmdesc, &CCWcullMode);

	cmdesc.FrontCounterClockwise = false;
	dx->getDevice()->CreateRasterizerState(&cmdesc, &CWcullMode);

	D3D11_RASTERIZER_DESC rastDesc;
	ZeroMemory(&rastDesc, sizeof(rastDesc));
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.CullMode = D3D11_CULL_NONE;

	dx->getDevice()->CreateRasterizerState(&rastDesc, &noCull);
	
	InitializeTexture();
	InitD2DScreenTexture();


	return true;

}

void settingCamera() {

	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;


	// 위쪽을 가리키는 벡터를 설정합니다.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// XMVECTOR 구조체에 로드한다.
	upVector = XMLoadFloat3(&up);

	// 3D월드에서 카메라의 위치를 ​​설정합니다.
	position = camPosition;

	// XMVECTOR 구조체에 로드한다.
	positionVector = XMLoadFloat3(&position);

	// 기본적으로 카메라가 찾고있는 위치를 설정합니다.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// XMVECTOR 구조체에 로드한다.
	lookAtVector = XMLoadFloat3(&lookAt);

	// yaw (Y 축), pitch (X 축) 및 roll (Z 축)의 회전값을 라디안 단위로 설정합니다.
	pitch = camRotation.x * 0.0174532925f;
	yaw = camRotation.y * 0.0174532925f;
	roll = camRotation.z * 0.0174532925f;

	//  yaw, pitch, roll 값을 통해 회전 행렬을 만듭니다.
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// lookAt 및 up 벡터를 회전 행렬로 변형하여 뷰가 원점에서 올바르게 회전되도록 합니다.
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	// 회전 된 카메라 위치를 뷰어 위치로 변환합니다.
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	// 마지막으로 세 개의 업데이트 된 벡터에서 뷰 행렬을 만듭니다.
	camView = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);


	//Set the Projection matrix
	camProjection = XMMatrixPerspectiveFovLH(XM_PI / 4.0f,
		(float)Width / Height, 0.1f, 1000.0f);

}


bool InitializeDirect3d11App(HINSTANCE hInstance) {

	HRESULT hr;


	//m_vsync_enabled = true;

	//Direct2D, Direct3D 10.1, DirectWrite 초기화

	

	return true;
}
void ReleaseObjects() {
	
	//d3d
	/*
	swapChain->Release();
	renderTargetView->Release();
	device->Release();
	deviceContext->Release();
	depthStencilView->Release();
	depthStencilBuffer->Release();
		//text
	d3d101Device->Release();
	keyedMutex11->Release();
	keyedMutex10->Release();
	D2DRenderTarget->Release();
	Brush->Release();
	//BackBuffer11->Release();
	sharedTex11->Release();
	d2dVertBuffer->Release();
	d2dIndexBuffer->Release();
	d2dTexture->Release();
	DWriteFactory->Release();
	TextFormat->Release();
	*/
	Transparency->Release();
	CCWcullMode->Release();
	CWcullMode->Release();
	noCull->Release();


	//model
	CubeVertexBuffer->Release();
	triangleVertexBuffer->Release();
	CubeIndexBuffer->Release();
	triangleIndexBuffer->Release();


	// shader
	vertexShader->Release();
	pixelShader->Release();
	layout->Release();

	cubeTexture->Release();
	cubetextureView->Release();
	CubesTexSamplerState->Release();

	cbPerObjectBuffer->Release();


}


//화면 프레임
void UpdateScene(double time) {
	updateCube(time);

}

void DrawScene() {

	dx->DXBeginScene();
	

	dx->getDevContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	dx->getDevContext()->IASetInputLayout(layout);
	dx->getDevContext()->VSSetShader(vertexShader, NULL, 0);
	dx->getDevContext()->PSSetShader(pixelShader, NULL, 0);

	
	float blendFactor[] = { 0.4f, 0.4f, 0.4f, 0.4f }; //??

	//Set the default blend state(no blending) for opaque objects
	dx->getDevContext()->OMSetBlendState(0, 0, 0xffffffff);
	//Render Opaque objects
	dx->getDevContext()->OMSetBlendState(Transparency, blendFactor, 0xffffffff);

	drawBg();
	drawCube();
	drawTriangle();
	drawCircle();

	float time2 = total_game_time * 0.001f;
	wstring ws = to_wstring(time2);
	
	dx->RenderText(ws, 80, 10, 250, 250);
	//Set the blend state for D2D render target texture objects
	dx->getDevContext()->OMSetBlendState(Transparency, NULL, 0xffffffff);
	WVP = XMMatrixIdentity();
	cbPerObj.WVP = XMMatrixTranspose(WVP);
	dx->getDevContext()->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
	dx->getDevContext()->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
	dx->getDevContext()->PSSetSamplers(0, 1, &CubesTexSamplerState);
	dx->getDevContext()->RSSetState(CWcullMode);
	//Draw the second cube
	dx->getDevContext()->DrawIndexed(6, 0, 0);
	
	dx->RenderText(L" Time : ", 0, 10, 100, 100);
	dx->getDevContext()->OMSetBlendState(Transparency, NULL, 0xffffffff);
	WVP = XMMatrixIdentity();
	cbPerObj.WVP = XMMatrixTranspose(WVP);
	dx->getDevContext()->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
	dx->getDevContext()->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
	dx->getDevContext()->PSSetSamplers(0, 1, &CubesTexSamplerState);
	dx->getDevContext()->RSSetState(CWcullMode);
	//Draw the second cube
	dx->getDevContext()->DrawIndexed(6, 0, 0);

	dx->RenderText(L"Shift : Reset Camera Focus ", 700, 10, 1024, 100);
	//Set the blend state for D2D render target texture objects
	dx->getDevContext()->OMSetBlendState(Transparency, NULL, 0xffffffff);
	WVP = XMMatrixIdentity();
	cbPerObj.WVP = XMMatrixTranspose(WVP);
	dx->getDevContext()->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
	dx->getDevContext()->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
	dx->getDevContext()->PSSetSamplers(0, 1, &CubesTexSamplerState);
	dx->getDevContext()->RSSetState(CWcullMode);
	//Draw the second cube
	dx->getDevContext()->DrawIndexed(6, 0, 0);

	
	dx->DXEndScene();
	

}

/*
	VertexType v[] =
	{
		// Front Face
		VertexType(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
		VertexType(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f),
		VertexType(1.0f,  1.0f, -1.0f, 1.0f, 0.0f),
		VertexType(1.0f, -1.0f, -1.0f, 1.0f, 1.0f),
	};
	*/
void InitD2DScreenTexture()
{

}


void RenderText(std::wstring text, float left, float top, float right, float bottom)
{
	
}



void StartTimer()
{
	LARGE_INTEGER frequencyCount;
	QueryPerformanceFrequency(&frequencyCount);

	countsPerSecond = double(frequencyCount.QuadPart);

	QueryPerformanceCounter(&frequencyCount);
	CounterStart = frequencyCount.QuadPart;
}

double GetTime()
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	return double(currentTime.QuadPart - CounterStart) / countsPerSecond;
}

double GetFrameTime()
{
	LARGE_INTEGER currentTime;
	__int64 tickCount;
	QueryPerformanceCounter(&currentTime);

	tickCount = currentTime.QuadPart - frameTimeOld;
	frameTimeOld = currentTime.QuadPart;

	if (tickCount < 0.0f)
		tickCount = 0.0f;

	return float(tickCount) / countsPerSecond;
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
			frameCount++;
			if (GetTime() > 1.0f) {
				fps = frameCount;
				frameCount = 0;
				StartTimer();
			}
			frameTime = GetFrameTime();
			UpdateScene(frameTime);
			DrawScene();
		}
		if (isStart) {
			total_game_time++;
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
		else if (wParam == VK_SPACE && jumpty == 0) {
			isSpace = true;
			isRotate = true;
		}
		else if (wParam == VK_SHIFT) {
			camPosition.x = cubeX + 7.0f;;
		}
		else if (wParam == VK_TAB) {
			isStart = true;
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}