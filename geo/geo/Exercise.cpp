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
XMMATRIX cubeFloorWorld;
XMMATRIX bgWorld;
XMMATRIX triangleWorld;
XMMATRIX triangleWorld2;
XMMATRIX circleWorld;
XMMATRIX camView;
XMMATRIX camProjection;



//*****************
#include <vector>

ID3D11Buffer* sphereIndexBuffer;
ID3D11Buffer* sphereVertBuffer;

ID3D11VertexShader* SKYMAP_VS;
ID3D11PixelShader* SKYMAP_PS;
ID3D10Blob* SKYMAP_VS_Buffer;
ID3D10Blob* SKYMAP_PS_Buffer;

ID3D11ShaderResourceView* smrv;

ID3D11DepthStencilState* DSLessEqual;
ID3D11RasterizerState* RSCullNone;

int NumSphereVertices;
int NumSphereFaces;

XMMATRIX sphereWorld;


float cubeBoundingSphere = 0.0f;
std::vector<XMFLOAT3> cubeBoundingBoxVertPosArray;
std::vector<DWORD> cubeBoundingBoxVertIndexArray;
XMVECTOR cubeCenterOffset;

void CreateBoundingVolumes(std::vector<XMFLOAT3>& vertPosArray,    // The array containing our models vertex positions
	std::vector<XMFLOAT3>& boundingBoxVerts,                            // Array we want to store the bounding box's vertex positions
	std::vector<DWORD>& boundingBoxIndex,                            // This is our bounding box's index array
	float& boundingSphere,                                            // The float containing the radius of our bounding sphere
	XMVECTOR& objectCenterOffset);                                    // A vector containing the distance between the models actual center and (0, 0, 0) in model space


void CreateSphere(int LatLines, int LongLines);



struct cbPerObject
{
	XMMATRIX  WVP;
	XMMATRIX World;
};
cbPerObject cbPerObj;

ID3D11Buffer* cbPerFrameBuffer;

struct Light
{
	Light()
	{
		ZeroMemory(this, sizeof(Light));
	}
	XMFLOAT3 dir;
	float pad;
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
};

Light light;

struct cbPerFrame
{
	Light  light;
};

cbPerFrame constbuffPerFrame;
void UpdateScene(double time);
ID3D11PixelShader* D2D_PS;
ID3D10Blob* D2D_PS_Buffer;

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
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{"NORMAL", 0,DXGI_FORMAT_R32G32B32_FLOAT,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,0}
};
unsigned int numElements = ARRAYSIZE(Playout);

// *** model *** ///

struct VertexType {
	VertexType() {} //기본 생성자. 없으면 오류난다.
	VertexType(float x, float y, float z,
		float u, float v,
		float nx, float ny, float nz)
		: pos(x, y, z), texCoord(u, v), normal(nx, ny, nz){}

	XMFLOAT3 pos;
	XMFLOAT2 texCoord;
	XMFLOAT3 normal;
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



void CreateBoundingVolumes(std::vector<XMFLOAT3>& vertPosArray,
	std::vector<XMFLOAT3>& boundingBoxVerts,
	std::vector<DWORD>& boundingBoxIndex,
	float& boundingSphere,
	XMVECTOR& objectCenterOffset) {

	XMFLOAT3 minVertex = XMFLOAT3(FLT_MAX, FLT_MAX, FLT_MAX);
	XMFLOAT3 maxVertex = XMFLOAT3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for (UINT i = 0; i < vertPosArray.size(); i++)
	{
		// The minVertex and maxVertex will most likely not be actual vertices in the model, but vertices
		// that use the smallest and largest x, y, and z values from the model to be sure ALL vertices are
		// covered by the bounding volume

		//Get the smallest vertex 
		minVertex.x = min(minVertex.x, vertPosArray[i].x);    // Find smallest x value in model
		minVertex.y = min(minVertex.y, vertPosArray[i].y);    // Find smallest y value in model
		minVertex.z = min(minVertex.z, vertPosArray[i].z);    // Find smallest z value in model

		//Get the largest vertex 
		maxVertex.x = max(maxVertex.x, vertPosArray[i].x);    // Find largest x value in model
		maxVertex.y = max(maxVertex.y, vertPosArray[i].y);    // Find largest y value in model
		maxVertex.z = max(maxVertex.z, vertPosArray[i].z);    // Find largest z value in model
	}

	// Compute distance between maxVertex and minVertex
	float distX = (maxVertex.x - minVertex.x) / 2.0f;
	float distY = (maxVertex.y - minVertex.y) / 2.0f;
	float distZ = (maxVertex.z - minVertex.z) / 2.0f;

	// Now store the distance between (0, 0, 0) in model space to the models real center
	objectCenterOffset = XMVectorSet(maxVertex.x - distX, maxVertex.y - distY, maxVertex.z - distZ, 0.0f);

	// Compute bounding sphere (distance between min and max bounding box vertices)
	// boundingSphere = sqrt(distX*distX + distY*distY + distZ*distZ) / 2.0f;
	boundingSphere = XMVectorGetX(XMVector3Length(XMVectorSet(distX, distY, distZ, 0.0f)));

	// Create bounding box    
	// Front Vertices
	boundingBoxVerts.push_back(XMFLOAT3(minVertex.x, minVertex.y, minVertex.z));
	boundingBoxVerts.push_back(XMFLOAT3(minVertex.x, maxVertex.y, minVertex.z));
	boundingBoxVerts.push_back(XMFLOAT3(maxVertex.x, maxVertex.y, minVertex.z));
	boundingBoxVerts.push_back(XMFLOAT3(maxVertex.x, minVertex.y, minVertex.z));

	// Back Vertices
	boundingBoxVerts.push_back(XMFLOAT3(minVertex.x, minVertex.y, maxVertex.z));
	boundingBoxVerts.push_back(XMFLOAT3(maxVertex.x, minVertex.y, maxVertex.z));
	boundingBoxVerts.push_back(XMFLOAT3(maxVertex.x, maxVertex.y, maxVertex.z));
	boundingBoxVerts.push_back(XMFLOAT3(minVertex.x, maxVertex.y, maxVertex.z));

	DWORD* i = new DWORD[36];

	// Front Face
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// Back Face
	i[6] = 4; i[7] = 5; i[8] = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// Top Face
	i[12] = 1; i[13] = 7; i[14] = 6;
	i[15] = 1; i[16] = 6; i[17] = 2;

	// Bottom Face
	i[18] = 0; i[19] = 4; i[20] = 5;
	i[21] = 0; i[22] = 5; i[23] = 3;

	// Left Face
	i[24] = 4; i[25] = 7; i[26] = 1;
	i[27] = 4; i[28] = 1; i[29] = 0;

	// Right Face
	i[30] = 3; i[31] = 2; i[32] = 6;
	i[33] = 3; i[34] = 6; i[35] = 5;

	for (int j = 0; j < 36; j++)
		boundingBoxIndex.push_back(i[j]);
}
XMMATRIX Rotationx;
XMMATRIX Rotationy;

void CreateSphere(int LatLines, int LongLines)
{
	NumSphereVertices = ((LatLines - 2) * LongLines) + 2;
	NumSphereFaces = ((LatLines - 3) * (LongLines) * 2) + (LongLines * 2);

	float sphereYaw = 0.0f;
	float spherePitch = 0.0f;

	std::vector<VertexType> vertices(NumSphereVertices);

	XMVECTOR currVertPos = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	vertices[0].pos.x = 0.0f;
	vertices[0].pos.y = 0.0f;
	vertices[0].pos.z = 1.0f;

	for (DWORD i = 0; i < LatLines - 2; ++i)
	{
		spherePitch = (i + 1) * (3.14 / (LatLines - 1));
		Rotationx = XMMatrixRotationX(spherePitch);
		for (DWORD j = 0; j < LongLines; ++j)
		{
			sphereYaw = j * (6.28 / (LongLines));
			Rotationy = XMMatrixRotationZ(sphereYaw);
			currVertPos = XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), (Rotationx * Rotationy));
			currVertPos = XMVector3Normalize(currVertPos);
			vertices[i * LongLines + j + 1].pos.x = XMVectorGetX(currVertPos);
			vertices[i * LongLines + j + 1].pos.y = XMVectorGetY(currVertPos);
			vertices[i * LongLines + j + 1].pos.z = XMVectorGetZ(currVertPos);
		}
	}

	vertices[NumSphereVertices - 1].pos.x = 0.0f;
	vertices[NumSphereVertices - 1].pos.y = 0.0f;
	vertices[NumSphereVertices - 1].pos.z = -1.0f;


	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * NumSphereVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;

	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = &vertices[0];
	HRESULT hr = dx->getDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &sphereVertBuffer);


	std::vector<DWORD> indices(NumSphereFaces * 3);

	int k = 0;
	for (DWORD l = 0; l < LongLines - 1; ++l)
	{
		indices[k] = 0;
		indices[k + 1] = l + 1;
		indices[k + 2] = l + 2;
		k += 3;
	}

	indices[k] = 0;
	indices[k + 1] = LongLines;
	indices[k + 2] = 1;
	k += 3;

	for (DWORD i = 0; i < LatLines - 3; ++i)
	{
		for (DWORD j = 0; j < LongLines - 1; ++j)
		{
			indices[k] = i * LongLines + j + 1;
			indices[k + 1] = i * LongLines + j + 2;
			indices[k + 2] = (i + 1) * LongLines + j + 1;

			indices[k + 3] = (i + 1) * LongLines + j + 1;
			indices[k + 4] = i * LongLines + j + 2;
			indices[k + 5] = (i + 1) * LongLines + j + 2;

			k += 6; // next quad
		}

		indices[k] = (i * LongLines) + LongLines;
		indices[k + 1] = (i * LongLines) + 1;
		indices[k + 2] = ((i + 1) * LongLines) + LongLines;

		indices[k + 3] = ((i + 1) * LongLines) + LongLines;
		indices[k + 4] = (i * LongLines) + 1;
		indices[k + 5] = ((i + 1) * LongLines) + 1;

		k += 6;
	}

	for (DWORD l = 0; l < LongLines - 1; ++l)
	{
		indices[k] = NumSphereVertices - 1;
		indices[k + 1] = (NumSphereVertices - 1) - (l + 1);
		indices[k + 2] = (NumSphereVertices - 1) - (l + 2);
		k += 3;
	}

	indices[k] = NumSphereVertices - 1;
	indices[k + 1] = (NumSphereVertices - 1) - LongLines;
	indices[k + 2] = NumSphereVertices - 2;

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * NumSphereFaces * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;

	iinitData.pSysMem = &indices[0];
	dx->getDevice()->CreateBuffer(&indexBufferDesc, &iinitData, &sphereIndexBuffer);

}





bool InitializeTriangle() {

	VertexType vertices2[] = {
		VertexType(0.5f, 1.0f, -0.5f, 0.1f, 0.1f, -1.0f, -1.0f, -1.0f),
		VertexType(1.0f, +.0f, -1.0f, 0.1f, 0.1f,1.0f,1.0f,1.0f),
		VertexType(+0.0f, +0.0f, -1.0f,0.1f, 0.1f,1.0f,1.0f,1.0f),

		VertexType(+.0f, .0f, .0f ,0.1f, 0.1f,1.0f,1.0f,1.0f),
		VertexType(1.0f, .0f, .0f ,0.1f, 0.1f,1.0f,1.0f,1.0f),

		VertexType(-1.0f, +1.0f, +1.0f,0.1f, 0.1f,1.0f,1.0f,1.0f),
		VertexType(+1.0f, +1.0f, +1.0f,0.1f, 0.1f,1.0f,1.0f,1.0f),
		VertexType(+1.0f, -1.0f, +1.0f,0.1f, 0.1f,1.0f,1.0f,1.0f)
		
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
	Translation = XMMatrixTranslation(triangleX, -0.7f, 0.5f);
	Scale = XMMatrixTranslation(0.2f, 0.15f, 0.2f);
	triangleWorld = Rotation*Scale*Translation;

	triangleWorld2 = XMMatrixIdentity();
	Translation = XMMatrixTranslation(15.0f, -0.7f, 0.5f);
	Scale = XMMatrixTranslation(0.2f, 0.15f, 0.2f);
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
	cbPerObj.World = XMMatrixTranspose(triangleWorld);
	dx->getDevContext()->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
	dx->getDevContext()->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);

	dx->getDevContext()->DrawIndexed(18, 0, 0); //here


	WVP = triangleWorld2 * camView * camProjection;
	cbPerObj.WVP = XMMatrixTranspose(WVP);
	cbPerObj.World = XMMatrixTranspose(triangleWorld2);
	dx->getDevContext()->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
	dx->getDevContext()->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);

	dx->getDevContext()->DrawIndexed(18, 0, 0); //here
}

bool InitializeCube() {

	VertexType vertices[] = {
		// Front Face
		VertexType(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f,1.0f,1.0f,1.0f),
		VertexType(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f,1.0f,1.0f,1.0f),
		VertexType(1.0f,  1.0f, -1.0f, 1.0f, 0.0f,1.0f,1.0f,1.0f),
		VertexType(1.0f, -1.0f, -1.0f, 1.0f, 1.0f,1.0f,1.0f,1.0f),

		// Back Face
		VertexType(-1.0f, -1.0f, 1.0f, 1.0f, 1.0f,1.0f,1.0f,1.0f),
		VertexType(1.0f, -1.0f, 1.0f, 0.0f, 1.0f,1.0f,1.0f,1.0f),
		VertexType(1.0f,  1.0f, 1.0f, 0.0f, 0.0f,1.0f,1.0f,1.0f),
		VertexType(-1.0f,  1.0f, 1.0f, 1.0f, 0.0f,1.0f,1.0f,1.0f),

		// Top Face
		VertexType(-1.0f, 1.0f, -1.0f, 1.f, 1.f,1.0f,1.0f,1.0f),
		VertexType(-1.0f, 1.0f,  1.0f, 0.0f, 1.f,1.0f,1.0f,1.0f),
		VertexType(1.0f, 1.0f,  1.0f, 0.0f, 0.0f,1.0f,1.0f,1.0f),
		VertexType(1.0f, 1.0f, -1.0f, 1.f, 1.f,1.0f,1.0f,1.0f),

		// Bottom Face
		VertexType(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f,1.0f,1.0f,1.0f),
		VertexType(1.0f, -1.0f, -1.0f, 0.0f, 1.0f,1.0f,1.0f,1.0f),
		VertexType(1.0f, -1.0f,  1.0f, 0.0f,0.0f,1.0f,1.0f,1.0f),
		VertexType(-1.0f, -1.0f,  1.0f, 1.0f, 1.f,1.0f,1.0f,1.0f),

		// Left Face
		VertexType(-1.0f, -1.0f,  1.0f, 0.0f, 1.0f,1.0f,1.0f,1.0f),
		VertexType(-1.0f,  1.0f,  1.0f, 0.0f, 0.0f,1.0f,1.0f,1.0f),
		VertexType(-1.0f,  1.0f, -1.0f, 1.0f, 0.0f,1.0f,1.0f,1.0f),
		VertexType(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f,1.0f,1.0f,1.0f),

		// Right Face
		VertexType(1.0f, -1.0f, -1.0f, 0.0f, 1.0f,1.0f,1.0f,1.0f),
		VertexType(1.0f,  1.0f, -1.0f, 0.0f, 0.0f,1.0f,1.0f,1.0f),
		VertexType(1.0f,  1.0f,  1.0f, 1.0f, 0.0f,1.0f,1.0f,1.0f),
		VertexType(1.0f, -1.0f,  1.0f, 1.0f, 1.0f,1.0f,1.0f,1.0f),

	};
	vertexCount = sizeof(vertices)/sizeof(VertexType);
	
	vector<XMFLOAT3> vertPosArray;



	CreateBoundingVolumes(vertPosArray, cubeBoundingBoxVertPosArray, cubeBoundingBoxVertIndexArray, cubeBoundingSphere, cubeCenterOffset);
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
	camRotation.y = 45.0f;

	cubeX += 7.f * time;
	settingCamera();
	XMVECTOR rotaxis = XMVectorSet(0.0f, .0f, 1.0f, 0.0f);
	///
	camPosition.x = cubeX - 7.f;
	
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


	cubeFloorWorld = XMMatrixIdentity();
	Scale = XMMatrixScaling(20.f, 0.5f, 3.0f);
	Translation = XMMatrixTranslation(cubeX+15.0f, -1.0f, 0.f);
	cubeFloorWorld = Scale*Translation;

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
	cbPerObj.World = XMMatrixTranspose(cubeWorld);
	dx->getDevContext()->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
	dx->getDevContext()->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);

	dx->getDevContext()->RSSetState(CWcullMode);
	dx->getDevContext()->DrawIndexed(indexCount, 0, 0);


	
	dx->getDevContext()->IASetVertexBuffers(0, 1, &CubeVertexBuffer, &stride, &offset);
	dx->getDevContext()->IASetIndexBuffer(CubeIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	dx->getDevContext()->PSSetShaderResources(0, 1, &bgtextureView);
	dx->getDevContext()->PSSetSamplers(0, 1, &BgTexSamplerState);
	WVP = cubeFloorWorld * camView * camProjection;
	cbPerObj.WVP = XMMatrixTranspose(WVP);
	cbPerObj.World = XMMatrixTranspose(cubeFloorWorld);
	dx->getDevContext()->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
	dx->getDevContext()->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);

	dx->getDevContext()->RSSetState(CWcullMode);
	dx->getDevContext()->DrawIndexed(indexCount, 0, 0);

}

bool InitializeCircle() {
	VertexType vertices[] = {
		// Front Face
		VertexType(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f,1.0f,1.0f,1.0f),
		VertexType(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f,1.0f,1.0f,1.0f),
		VertexType(1.0f,  1.0f, -1.0f, 1.0f, 0.0f,1.0f,1.0f,1.0f),
		VertexType(1.0f, -1.0f, -1.0f, 1.0f, 1.0f,1.0f,1.0f,1.0f),

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
	cbPerObj.World = XMMatrixTranspose(circleWorld);
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

#include "DDSTextureLoader.h"


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

	

	CreateDDSTextureFromFile(dx->getDevice(), L"grass.dds", NULL, &bgtextureView);
	
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//Create the Sample State
	dx->getDevice()->CreateSamplerState(&sampDesc, &BgTexSamplerState);
}

bool InitializeShader() {
	
	light.dir = XMFLOAT3(0.25f, 0.5f, -1.0f);
	light.ambient = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	light.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

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
	D3DCompileFromFile(L"eff.fx", NULL, NULL, "VS", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&vertexShaderBuffer, &errorMessage);

	D3DCompileFromFile(L"eff.fx", NULL, NULL, "PS", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errorMessage);

	D3DCompileFromFile(L"eff.fx", NULL, NULL, "D2D_PS", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&D2D_PS_Buffer, &errorMessage);

	D3DCompileFromFile(L"eff.fx", NULL, NULL, "SKYMAP_VS", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&SKYMAP_VS_Buffer, &errorMessage);

	D3DCompileFromFile(L"eff.fx", NULL, NULL, "SKYMAP_PS", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&SKYMAP_PS_Buffer, &errorMessage);



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

	result = dx->getDevice()->CreatePixelShader(D2D_PS_Buffer->GetBufferPointer(), D2D_PS_Buffer->GetBufferSize(), NULL, &D2D_PS);
	if (FAILED(result))
	{
		MessageBox(hwnd, L"Could not create the pixel shader object.", L"Error", MB_OK);
		return false;
	}

	dx->getDevice()->CreateVertexShader(SKYMAP_VS_Buffer->GetBufferPointer(), SKYMAP_VS_Buffer->GetBufferSize(), NULL, &SKYMAP_VS);
	dx->getDevice()->CreatePixelShader(SKYMAP_PS_Buffer->GetBufferPointer(), SKYMAP_PS_Buffer->GetBufferSize(), NULL, &SKYMAP_PS);

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

	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));
	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(cbPerFrame);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;
	dx->getDevice()->CreateBuffer(&cbbd, NULL, &cbPerFrameBuffer);

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
	rastDesc.MultisampleEnable = TRUE;
	

	dx->getDevice()->CreateRasterizerState(&rastDesc, &noCull);

	cmdesc.CullMode = D3D11_CULL_NONE;
	dx->getDevice()->CreateRasterizerState(&cmdesc, &RSCullNone);

	D3D11_DEPTH_STENCIL_DESC dssDesc;
	ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dssDesc.DepthEnable = true;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	dx->getDevice()->CreateDepthStencilState(&dssDesc, &DSLessEqual);

	CreateDDSTextureFromFile(dx->getDevice(), L"skymap.dds", NULL, &smrv);

	InitializeTexture();
	InitD2DScreenTexture();

	/*
	result = D3DCompileFromFile(L"effects.fx", NULL, NULL, "SKYMAP_VS", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&SKYMAP_VS_Buffer, &errorMessage);
	result = D3DCompileFromFile(L"effects.fx", NULL, NULL, "SKYMAP_PS", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&SKYMAP_PS_Buffer, &errorMessage);

	result = dx->getDevice()->CreateVertexShader(SKYMAP_VS_Buffer->GetBufferPointer(), SKYMAP_VS_Buffer->GetBufferSize(), NULL, &SKYMAP_VS);
	result = dx->getDevice()->CreatePixelShader(SKYMAP_PS_Buffer->GetBufferPointer(), SKYMAP_PS_Buffer->GetBufferSize(), NULL, &SKYMAP_PS);


	CreateDDSTextureFromFile(dx->getDevice(), L"skymap.dds", nullptr, &smrv);
	*/

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
	cbPerFrameBuffer->Release();

	sphereIndexBuffer->Release();
	sphereVertBuffer->Release();

	SKYMAP_VS->Release();
	SKYMAP_PS->Release();
	SKYMAP_VS_Buffer->Release();
	SKYMAP_PS_Buffer->Release();

	smrv->Release();

	DSLessEqual->Release();
	RSCullNone->Release();
}


//화면 프레임
void UpdateScene(double time) {
	updateCube(time);

	sphereWorld = XMMatrixIdentity();
	Scale = XMMatrixScaling(5.0f, 5.0f, 5.0f);
	Translation = XMMatrixTranslation(camPosition.x, camPosition.y, camPosition.z);
	sphereWorld = Scale * Translation;
}

void DrawScene() {

	CreateSphere(10, 10);

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

	constbuffPerFrame.light = light;
	dx->getDevContext()->UpdateSubresource(cbPerFrameBuffer, 0, NULL, &constbuffPerFrame, 0, 0);
	dx->getDevContext()->PSSetConstantBuffers(0, 1, &cbPerFrameBuffer);

	drawCube();
	drawTriangle();
	drawCircle();

	UINT stride = sizeof(VertexType);
	UINT offset = 0;
	dx->getDevContext()->IASetIndexBuffer(sphereIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	dx->getDevContext()->IASetVertexBuffers(0, 1, &sphereVertBuffer, &stride, &offset);

	WVP = sphereWorld * camView * camProjection;
	cbPerObj.WVP = XMMatrixTranspose(WVP);
	cbPerObj.World = XMMatrixTranspose(sphereWorld);
	dx->getDevContext()->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
	dx->getDevContext()->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
	dx->getDevContext()->PSSetShaderResources(0, 1, &smrv);
	dx->getDevContext()->PSSetSamplers(0, 1, &CubesTexSamplerState);

	dx->getDevContext()->VSSetShader(SKYMAP_VS, 0, 0);
	dx->getDevContext()->PSSetShader(SKYMAP_PS, 0, 0);
	dx->getDevContext()->OMSetDepthStencilState(DSLessEqual, 0);
	dx->getDevContext()->RSSetState(RSCullNone);
	dx->getDevContext()->DrawIndexed(NumSphereFaces * 3, 0, 0);

	dx->getDevContext()->OMSetDepthStencilState(NULL, 0);


	float time2 = total_game_time * 0.001f;
	wstring ws = to_wstring(time2);
	
	dx->RenderText(ws, 80, 10, 250, 250);
	//Set the blend state for D2D render target texture objects
	dx->getDevContext()->OMSetBlendState(Transparency, NULL, 0xffffffff);
	WVP = XMMatrixIdentity();
	cbPerObj.WVP = XMMatrixTranspose(WVP);
	cbPerObj.World = XMMatrixTranspose(WVP);
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
	cbPerObj.World = XMMatrixTranspose(WVP);
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
	cbPerObj.World = XMMatrixTranspose(WVP);
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