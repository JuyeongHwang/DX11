#include "pch.h"



//FOR FONT
// 
ID3D10Device1* d3d101Device;
IDXGIKeyedMutex* keyedMutex11;
IDXGIKeyedMutex* keyedMutex10;
ID2D1RenderTarget* D2DRenderTarget;
ID2D1SolidColorBrush* Brush;
ID3D11Texture2D* BackBuffer11;
ID3D11Texture2D* sharedTex11;
ID3D11Buffer* d2dVertBuffer;
ID3D11Buffer* d2dIndexBuffer;
ID3D11ShaderResourceView* d2dTexture;
IDWriteFactory* DWriteFactory;
IDWriteTextFormat* TextFormat;


ID3D11BlendState* Transparency;
ID3D11RasterizerState* CCWcullMode;
ID3D11RasterizerState* CWcullMode;
ID3D11RasterizerState* noCull;

bool InitD2D_D3D101_DWrite(IDXGIAdapter1* Adapter);
void InitD2DScreenTexture();
void RenderText(wstring text, float left, float top, float right, float bottom);
//*** 전역변수 ***//
const int Width = 1024;
const int Height = 720;

IDXGISwapChain* swapChain = nullptr;
ID3D11Device* device = nullptr;
ID3D11DeviceContext* deviceContext = nullptr;
ID3D11RenderTargetView* renderTargetView = nullptr;
ID3D11DepthStencilView* depthStencilView;
ID3D11Texture2D* depthStencilBuffer;

ID3D11Buffer* CubeVertexBuffer = nullptr;
ID3D11Buffer* triangleVertexBuffer = nullptr;
ID3D11Buffer* CubeIndexBuffer = nullptr;
ID3D11Buffer* triangleIndexBuffer = nullptr;
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
XMMATRIX triangleWorld;
XMMATRIX triangleWorld2;
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

// **** texture **** //
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

ID3D11Texture2D* cubeTexture = 0;
ID3D11ShaderResourceView* cubetextureView = 0;
ID3D11SamplerState* CubesTexSamplerState = 0; //d3d


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
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &triangleVertexBuffer)))
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
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &triangleIndexBuffer)))
	{
		return false;
	}

}

void updateTriangle() {
	XMVECTOR rotaxis = XMVectorSet(0.0f, .0f, 1.0f, 0.0f);

	Rotation = XMMatrixRotationAxis(rotaxis, 0.0f);

	triangleWorld = XMMatrixIdentity();
	Translation = XMMatrixTranslation(4.f, -0.7f, 0.f);
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
	
	deviceContext->IASetVertexBuffers(0, 1, &triangleVertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(triangleIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	WVP = triangleWorld * camView * camProjection;
	cbPerObj.WVP = XMMatrixTranspose(WVP);
	deviceContext->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
	deviceContext->DrawIndexed(18, 0, 0); //here


	WVP = triangleWorld2 * camView * camProjection;
	cbPerObj.WVP = XMMatrixTranspose(WVP);
	deviceContext->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
	deviceContext->DrawIndexed(18, 0, 0); //here
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
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &CubeVertexBuffer)))
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
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &CubeIndexBuffer)))
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
	deviceContext->IASetVertexBuffers(0, 1, &CubeVertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(CubeIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->PSSetShaderResources(0, 1, &cubetextureView);
	deviceContext->PSSetSamplers(0, 1, &CubesTexSamplerState);


	WVP = cubeWorld * camView * camProjection;
	cbPerObj.WVP = XMMatrixTranspose(WVP);

	deviceContext->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);


	deviceContext->RSSetState(CWcullMode);
	deviceContext->DrawIndexed(indexCount, 0, 0);

}


bool LoadTarga() {

	FILE* filePtr;
	TargaHeader targaFileHeader;

	int error = fopen_s(&filePtr, "toppng.tga", "rb");
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

	/*
	wstring ws = to_wstring(count);
	LPCWSTR result = ws.c_str();
	MessageBox(hwnd, result, L"Error", MB_OK);
*/

	error = fclose(filePtr);
	if (error != 0) { return false; }

	return true;
}


bool InitializeTexture() {
	D3D11_TEXTURE2D_DESC textureDesc;
	unsigned int rowPitch;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	if (!LoadTarga()) {
		return false;
	}

	// Setup the description of the texture.
	textureDesc.Height = texheight;
	textureDesc.Width = texwidth;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// Create the empty texture.
	HRESULT hResult = device->CreateTexture2D(&textureDesc, NULL, &cubeTexture);
	if (FAILED(hResult))
	{
		return false;
	}


	// Set the row pitch of the targa image data.
	rowPitch = (texwidth * 4) * sizeof(unsigned char);

	// Copy the targa image data into the texture.
	deviceContext->UpdateSubresource(cubeTexture, 0, NULL, targaData, rowPitch, 0);

	// Setup the shader resource view description.
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	// Create the shader resource view for the texture.
	hResult = device->CreateShaderResourceView(cubeTexture, &srvDesc, &cubetextureView);
	if (FAILED(hResult))
	{
		return false;
	}

	// Generate mipmaps for this texture.
	deviceContext->GenerateMips(cubetextureView);

	// Release the targa image data now that the image data has been loaded into the texture.
	delete[] targaData;
	targaData = 0;


	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//Create the Sample State
	device->CreateSamplerState(&sampDesc, &CubesTexSamplerState);

}

bool InitializeShader() {
	

	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer; //from hlsl
	ID3D10Blob* pixelShaderBuffer; //from hlsl
	
	InitializeCube();
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
	

	D3D11_BUFFER_DESC cbbd;
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));
	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(cbPerObject);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	result = device->CreateBuffer(&cbbd, NULL, &cbPerObjectBuffer);
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

	device->CreateBlendState(&blendDesc, &Transparency);

	D3D11_RASTERIZER_DESC cmdesc;
	ZeroMemory(&cmdesc, sizeof(cmdesc));

	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_BACK;

	cmdesc.FrontCounterClockwise = true;
	device->CreateRasterizerState(&cmdesc, &CCWcullMode);

	cmdesc.FrontCounterClockwise = false;
	device->CreateRasterizerState(&cmdesc, &CWcullMode);

	D3D11_RASTERIZER_DESC rastDesc;
	ZeroMemory(&rastDesc, sizeof(rastDesc));
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.CullMode = D3D11_CULL_NONE;

	device->CreateRasterizerState(&rastDesc, &noCull);
	
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

	IDXGIFactory1* DXGIFactory;
	CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&DXGIFactory);

	IDXGIAdapter1* Adapter;
	DXGIFactory->EnumAdapters1(0, &Adapter);

	DXGIFactory->Release();

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

	
		//Create our SwapChain
	hr = D3D11CreateDeviceAndSwapChain(Adapter, D3D_DRIVER_TYPE_UNKNOWN, NULL,
		D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT, NULL, NULL,
		D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &device, NULL, &deviceContext);

	//Direct2D, Direct3D 10.1, DirectWrite 초기화
	InitD2D_D3D101_DWrite(Adapter);
	Adapter->Release();
	//Create our BackBuffer
	ID3D11Texture2D* BackBuffer;
	hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBuffer);

	//Create our Render Target
	hr = device->CreateRenderTargetView(BackBuffer, NULL, &renderTargetView);
	BackBuffer->Release();

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

	//Describe our Depth/Stencil Buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = Width;
	depthStencilDesc.Height = Height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	//Create the Depth/Stencil View
	device->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer);
	device->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView);

	//Set our Render Target
	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

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
	depthStencilView->Release();
	depthStencilBuffer->Release();
	
	//model
	CubeVertexBuffer->Release();
	triangleVertexBuffer->Release();
	CubeIndexBuffer->Release();
	triangleIndexBuffer->Release();


	vertexShader->Release();
	pixelShader->Release();
	layout->Release();

	cubeTexture->Release();
	cubetextureView->Release();
	CubesTexSamplerState->Release();

	cbPerObjectBuffer->Release();

}

float total_game_time = 0.0f;
//화면 프레임
void UpdateScene(double time) {
	updateCube(time);

}

void DrawScene() {

	
	float bgColor[] = { 0.0f,0.0f,0.0f,1.0f };

	deviceContext->ClearRenderTargetView(renderTargetView, bgColor);
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	deviceContext->IASetInputLayout(layout);
	deviceContext->VSSetShader(vertexShader, NULL, 0);
	deviceContext->PSSetShader(pixelShader, NULL, 0);

	
	float blendFactor[] = { 0.4f, 0.4f, 0.4f, 0.4f }; //??

	//Set the default blend state(no blending) for opaque objects
	deviceContext->OMSetBlendState(0, 0, 0xffffffff);
	//Render Opaque objects
	deviceContext->OMSetBlendState(Transparency, blendFactor, 0xffffffff);

	drawCube();
	drawTriangle();

	float time2 = total_game_time * 0.001f;
	wstring ws = to_wstring(time2);
	RenderText(ws, 80, 10, 250, 250);
	RenderText(L" Time : ", 0, 10, 100, 100);
	RenderText(L"Shift : Reset Camera Focus ", 700, 10, 1024, 100);

	swapChain->Present(0, 0);

	

}

bool InitD2D_D3D101_DWrite(IDXGIAdapter1* Adapter)
{
	HRESULT hr;
	//Create our Direc3D 10.1 Device///////////////////////////////////////////////////////////////////////////////////////
	hr = D3D10CreateDevice1(Adapter, D3D10_DRIVER_TYPE_HARDWARE, NULL, D3D10_CREATE_DEVICE_DEBUG | D3D10_CREATE_DEVICE_BGRA_SUPPORT,
		D3D10_FEATURE_LEVEL_9_3, D3D10_1_SDK_VERSION, &d3d101Device);

	//Create Shared Texture that Direct3D 10.1 will render on//////////////////////////////////////////////////////////////
	D3D11_TEXTURE2D_DESC sharedTexDesc;

	ZeroMemory(&sharedTexDesc, sizeof(sharedTexDesc));

	sharedTexDesc.Width = Width;
	sharedTexDesc.Height = Height;
	sharedTexDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sharedTexDesc.MipLevels = 1;
	sharedTexDesc.ArraySize = 1;
	sharedTexDesc.SampleDesc.Count = 1;
	sharedTexDesc.Usage = D3D11_USAGE_DEFAULT;
	sharedTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	sharedTexDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;

	hr = device->CreateTexture2D(&sharedTexDesc, NULL, &sharedTex11);

	// Get the keyed mutex for the shared texture (for D3D11)///////////////////////////////////////////////////////////////
	hr = sharedTex11->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)&keyedMutex11);

	// Get the shared handle needed to open the shared texture in D3D10.1///////////////////////////////////////////////////
	IDXGIResource* sharedResource10;
	HANDLE sharedHandle10;

	hr = sharedTex11->QueryInterface(__uuidof(IDXGIResource), (void**)&sharedResource10);

	hr = sharedResource10->GetSharedHandle(&sharedHandle10);

	sharedResource10->Release();

	// Open the surface for the shared texture in D3D10.1///////////////////////////////////////////////////////////////////
	IDXGISurface1* sharedSurface10;

	hr = d3d101Device->OpenSharedResource(sharedHandle10, __uuidof(IDXGISurface1), (void**)(&sharedSurface10));

	hr = sharedSurface10->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)&keyedMutex10);

	// Create D2D factory///////////////////////////////////////////////////////////////////////////////////////////////////
	ID2D1Factory* D2DFactory;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), (void**)&D2DFactory);

	D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties;

	ZeroMemory(&renderTargetProperties, sizeof(renderTargetProperties));

	renderTargetProperties.type = D2D1_RENDER_TARGET_TYPE_HARDWARE;
	renderTargetProperties.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED);

	hr = D2DFactory->CreateDxgiSurfaceRenderTarget(sharedSurface10, &renderTargetProperties, &D2DRenderTarget);

	sharedSurface10->Release();
	D2DFactory->Release();

	// Create a solid color brush to draw something with        
	hr = D2DRenderTarget->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 0.0f, 1.0f), &Brush);

	//DirectWrite///////////////////////////////////////////////////////////////////////////////////////////////////////////
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&DWriteFactory));

	hr = DWriteFactory->CreateTextFormat(
		L"Script",
		NULL,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		24.0f,
		L"en-us",
		&TextFormat
	);

	hr = TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	hr = TextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

	d3d101Device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
	return true;
}
void InitD2DScreenTexture()
{
	//Create the vertex buffer
	VertexType v[] =
	{
		// Front Face
		VertexType(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
		VertexType(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f),
		VertexType(1.0f,  1.0f, -1.0f, 1.0f, 0.0f),
		VertexType(1.0f, -1.0f, -1.0f, 1.0f, 1.0f),
	};

	DWORD indices[] = {
		// Front Face
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
	device->CreateBuffer(&indexBufferDesc, &iinitData, &d2dIndexBuffer);


	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * 4;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;

	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = v;
	device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &d2dVertBuffer);

	//Create A shader resource view from the texture D2D will render to,
	//So we can use it to texture a square which overlays our scene
	device->CreateShaderResourceView(sharedTex11, NULL, &d2dTexture);
}

wstring printText;
void RenderText(std::wstring text, float left, float top, float right, float bottom)
{
	//Release the D3D 11 Device
	keyedMutex11->ReleaseSync(0);

	//Use D3D10.1 device
	keyedMutex10->AcquireSync(0, 5);

	//Draw D2D content        
	D2DRenderTarget->BeginDraw();

	//Clear D2D Background
	D2DRenderTarget->Clear(D2D1::ColorF(0.0f, 1.0f, 1.0f, 0.0f));

	//Create our string
	std::wostringstream printString;
	printString << text;
	printText = printString.str();

	//Set the Font Color
	D2D1_COLOR_F FontColor = D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f);

	//Set the brush color D2D will use to draw with
	Brush->SetColor(FontColor);

	//Create the D2D Render Area
	D2D1_RECT_F layoutRect = D2D1::RectF(left, top, right, bottom);

	//Draw the Text
	D2DRenderTarget->DrawText(
		printText.c_str(),
		wcslen(printText.c_str()),
		TextFormat,
		layoutRect,
		Brush
	);

	D2DRenderTarget->EndDraw();

	//Release the D3D10.1 Device
	keyedMutex10->ReleaseSync(1);

	//Use the D3D11 Device
	keyedMutex11->AcquireSync(1, 5);

	//Use the shader resource representing the direct2d render target
	//to texture a square which is rendered in screen space so it
	//overlays on top of our entire scene. We use alpha blending so
	//that the entire background of the D2D render target is "invisible",
	//And only the stuff we draw with D2D will be visible (the text)

	//Set the blend state for D2D render target texture objects
	deviceContext->OMSetBlendState(Transparency, NULL, 0xffffffff);

	//Set the d2d Index buffer
	deviceContext->IASetIndexBuffer(d2dIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	//Set the d2d vertex buffer
	UINT stride = sizeof(VertexType);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &d2dVertBuffer, &stride, &offset);

	WVP = XMMatrixIdentity();
	cbPerObj.WVP = XMMatrixTranspose(WVP);
	deviceContext->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
	deviceContext->PSSetShaderResources(0, 1, &d2dTexture);
	deviceContext->PSSetSamplers(0, 1, &CubesTexSamplerState);

	deviceContext->RSSetState(CWcullMode);
	//Draw the second cube
	deviceContext->DrawIndexed(6, 0, 0);
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