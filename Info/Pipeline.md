![image](https://user-images.githubusercontent.com/49023736/129656980-0d13e030-8f1a-4363-be58-4be76109e089.png)



## <Programmable Graphics Rendring Pipeline>

렌더링 파이프라인은 direct3d가 가상 카메라가 보는 것을 기반으로 2d 이미지를 만드는 데 사용하는 단계 집합입니다.
Direct3D 10에서는 총 7단계로 구성되어 있으며 Direct3D 11에서는 3단계가 추가되었습니다.

1. Input Assembler (IA) Stage //fixed <br>
2. Vertex Shader (VS) Stage //programmable <br>
3. Hull Shader (HS) Stage //programmable <br>
4. Tesselator Shader (TS) Stage //fixed <br>
5. Domain Shader (DS) Stage //programmable <br>
6. Geometry Shader (GS) Stage //programmable <br>
7. Stream Output (SO) Stage //fixed <br>
8. Rasterizer (RS) Stage //fixed <br>
9. Pixel Shader (PS) Stage //programmable <br>
10. Output Merger (OM) Stage //fixed <br>


### 1. Input Assembler (IA) Stage 

그런 다음 데이터를 사용하여 다른 단계에 입력되고 사용될 삼각형, 사각형, 선 및 점과 같은 지오매트릭 프리미티브를 작성합니다.<br>
IA에 무언가를 보내기 전, 수행해야 할 몇가지 일들이 있습니다.<br>
이는 크게 세가지로 나누어 볼 수 있는데,<br>
1. 버텍스 버퍼와 인덱스 버퍼 만들기<br>
2. 인풋 레이아웃 만들기<br>
3. 프리미티브 타입 결정하기(ex. 삼각형으로 그릴 것인지 사각형으로 그릴 것인지)<br>

입니다. 

버퍼를 만드는 것부터 시작해봅시다. 우리가 사용할 버퍼는 정점버퍼와 인덱스 버퍼 두가지 입니다.


#### <span style="color:blue">입력 레이아웃</span>

선언은 다음과 같습니다.<br>
ID3D11Buffer* vertexBuffer;

버퍼를 만드는 것은 렌더링 파이프라인에 직접적인 연계가 있는 것은 아니기에 device의 메소드인 CreateBuffer를 이용하여 만들어집니다.<br>
이 메소드는 ID3D11_BUFFER_DESC와 ID3D11_SUBRESOURCE_DATA, 그리고 이 정보를 담을 우리의 버퍼, ID3D11Buffer 이 세가지 파라미터를 갖습니다.<br>
IndexBuffer도 마찬가지입니다.<br>
여기서, ID3D11_SUBRESOURCE_DATA에 pSysMem이라는 변수를 채워넣기 위해서, vertex의 경우 구체적인 vertex를 구성하는 정보가 필요하고,
index같은 경우 그 버텍스를 어떤 순서로 그릴 것인지에 대한 정보가 필요합니다.<br>
예시는 아래와 같습니다.

//버텍스 구조

struct Vertex	//Overloaded Vertex Structure
{
	Vertex() {}
	Vertex(float x, float y, float z,
		float u, float v)
		: pos(x, y, z), texCoord(u, v) {}

	XMFLOAT3 pos;
	XMFLOAT2 texCoord;
};

//버텍스 정보

Vertex v[] =
	{
		// Front Face
		Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
		Vertex(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f),
		Vertex(1.0f,  1.0f, -1.0f, 1.0f, 0.0f),
		Vertex(1.0f, -1.0f, -1.0f, 1.0f, 1.0f),
};

//인덱스 정보

DWORD indices[] = {
		// Front Face
		0,  1,  2,
		0,  2,  3,
};

두번째로 만들어야 하는 것은 레이아웃 입니다.

D3D11_INPUT_ELEMENT_DESC의 경우 다음과 같이 사용할 수 있습니다.<br>


D3D11_INPUT_ELEMENT_DESC layout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

이때 첫번째 파라미터는 hlsl과 형태가 같아야함에 주의해야하고, 같은 종류의 정보가 있다면, TEXCOORD_ZERO, TEXCOORD_ONE과 같이 구분해야합니다.
그 뒤 ID3D11Device::CreateInputLayOut()을 통해 ID3D11InputLayout 인터페이스를 생성합니다.<br>

device를 통해 인터페이스들을 만들었다면, 이 인터페이스 오브젝트들을 렌더링 파이프라인에 바인딩해야합니다. <br>
이는 ID3D11DeviceContext가 맡아줄 것입니다. 메소드는 아래와 같습니다.<br>

IASetVertexBuffers()<br>
IASetInputLayout()<br>

그리고 마지막으로 ID3D_PRIMITIVE_TOPOLOGY 인터페이스도 설정해줍니다.<br>
IASetPrimitiveTopology()


이렇게 만들어진 정보를 ID3D11DeviceContext의 Draw()나 DrawIndexed() 함수를 이용하여 그릴 수 있게 됩니다.


  ### 2. Vertex Shader (VS) Stage

  Vertex Shader (VS) Stage

VS는 처음으로 프로그래밍 할 수 있는 단계이다. VS 단계는 IA에서 프리미티브들이 조립된 후 모든 정점이 거치는 단계입니다. 그려진 모든 정점은
VS에 통과합니다. VS를 이용해서 transformation, scaling, lighting, displacement mapping for texture 등을 할 수 있습니다. <br>
프로그램의 정점을 수정할 필요가 없는 경우에도 파이프라인이 작동하려면 항상 정점 셰이더를 구현해야 합니다.<br>
파이프라인의 셰이더는 HLSL 언어로 구현됩니다.

예시는 아래와 같습니다.

cbuffer cbPerObject
{
	float4x4 WVP;
};
struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

VS_OUTPUT VS(float4 inPos : POSITION, float2 inTex : TEXCOORD)
{
	VS_OUTPUT output;

	output.Pos = mul(inPos, WVP);
	output.Tex = inTex;

	return output;
}


우선 cbuffer라고 적혀있는 상수버퍼를 살펴봅시다.
상수버퍼를 사용하기 위해 Map, UnMap을 사용하는 방식이 있고, 간단하게
VSSetConstanctBuffers() 메소드를 이용할 수도 있습니다.

  
  
  https://www.braynzarsoft.net/viewtutorial/q16390-4-begin-drawing
  https://m.blog.naver.com/sorkelf/40162381015
