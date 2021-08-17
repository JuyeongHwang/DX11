
## <The Blending Equation>

Direct3D에서 투명도의 구현을 위해 렌더 타겟에 있는 투명한 프리미티브 뒤에 있는 픽셀을 가지고 와,
현재 투명한 프리미티브 픽셀의 컬러를 가져와 섞습니다.<br>

Direct3D는 두개의 다른 블렌딩 공식을 가지는데 하나는 색상 하나는 알파입니다. 
우리가 원하는대로 이 둘을 다르게 적용할 수 있기 위해 두 공식이 존재합니다. <br>
  
  공식은 아래.<br>
Final Color = Source Pixel X Source Belnd Factor + Destination Pixel X Destination Blend Pixel
Final Alpha = (Source Alpha) * (Source Blend Alpha) + (Destination Alpha) * (Destination Blend Factor)

바이너리를 더하는 operator은 다음 중 하나입니다.<br>

typedef enum D3D11_BLEND_OP {
  D3D11_BLEND_OP_ADD            = 1,
  D3D11_BLEND_OP_SUBTRACT       = 2,
  D3D11_BLEND_OP_REV_SUBTRACT   = 3,
  D3D11_BLEND_OP_MIN            = 4,
  D3D11_BLEND_OP_MAX            = 5 
} D3D11_BLEND_OP;


우리는 "blend factors"를 이용해서 블렌딩 할 때, 다른 효과를 만들 것입니다.<br>
  그리고 우리는 blend factors를 <b>D3D11_BLEND</b>를 통해 세팅할 것입니다.<br>

typedef enum D3D11_BLEND {
  D3D11_BLEND_ZERO               = 1,
  D3D11_BLEND_ONE                = 2,
  D3D11_BLEND_SRC_COLOR          = 3,
  D3D11_BLEND_INV_SRC_COLOR      = 4,
  D3D11_BLEND_SRC_ALPHA          = 5,
  D3D11_BLEND_INV_SRC_ALPHA      = 6,
  D3D11_BLEND_DEST_ALPHA         = 7,
  D3D11_BLEND_INV_DEST_ALPHA     = 8,
  D3D11_BLEND_DEST_COLOR         = 9,
  D3D11_BLEND_INV_DEST_COLOR     = 10,
  D3D11_BLEND_SRC_ALPHA_SAT      = 11,
  D3D11_BLEND_BLEND_FACTOR       = 14,
  D3D11_BLEND_INV_BLEND_FACTOR   = 15,
  D3D11_BLEND_SRC1_COLOR         = 16,
  D3D11_BLEND_INV_SRC1_COLOR     = 17,
  D3D11_BLEND_SRC1_ALPHA         = 18,
  D3D11_BLEND_INV_SRC1_ALPHA     = 19 
} D3D11_BLEND;

그 다음 우리의 픽셀을 어떻게 블렌딩 할것인지에 대한 묘사를 넘겨줘야 합니다.<br>
  
typedef struct D3D11_BLEND_DESC {
  BOOL                           AlphaToCoverageEnable; //Multisampling technique usefull for things like wire fences and stuff, 
						we'll talk about this more in a later lesson, specify true to enable.
  BOOL                           IndependentBlendEnable;//We are able to blend with more than one render target at a time, specifically 8. 
						Setting this to false will only blend to the RenderTarget[0] description in the array below, 
							and ignore the 1-7 render target descriptions.
  D3D11_RENDER_TARGET_BLEND_DESC RenderTarget[8]; //As we mentioned a moment ago, we are able to blend with more than one render target
						, in fact, we can blend with 8 different render targets. 
						This is an array of up to 8 <b>D3D11_RENDER_TARGET_BLEND_DESC</b> structures, 
							where each element is a blending description for that render target.
} D3D11_BLEND_DESC;


typedef struct D3D11_RENDER_TARGET_BLEND_DESC {
  BOOL           BlendEnable;  Specify true to enable blending for this render target.

  D3D11_BLEND    SrcBlend;This is our source blend factor (SBF). We can set this to any of the enumerated D3D11_BLEND types.
  D3D11_BLEND    DestBlend;This is our destination blend factor (DBF). We can set this to any of the enumerated D3D11_BLEND types.
  D3D11_BLEND_OP BlendOp;Here is where we specify the blending operation to use which we discussed earlier. Set this to any of the D3D11_BLEND_OP enumerated types.
  D3D11_BLEND    SrcBlendAlpha;This is our source blend factor for the alpha channel (SBF). We can set this to any of the enumerated D3D11_BLEND types.
  D3D11_BLEND    DestBlendAlpha; This is our destination blend factor for the alpha channel (SBF). We can set this to any of the enumerated D3D11_BLEND types.

  D3D11_BLEND_OP BlendOpAlpha;Here we specify the blending operation to use for the alpha channels. set this to any of the D3D10_BLEND_OP enumerated types.

  UINT8          RenderTargetWriteMask;This is where we specify which channel to blend. We are able to choose R, G, B, A, All of them, or a combination of them by specifying one of the following flags:
} D3D11_RENDER_TARGET_BLEND_DESC;


## <Transparent Object's Depth Order.>

 투명한 물체 뒤에 있는 것을 볼 때, 또는 여러 투명 물체가 씬에 있을 때 다뤄야 하는 문제점에 대해 설명합니다.<br>

투명물체를 렌더링 할 때는 무조건 이미 렌더 타겟에 있어야 하고 그것과 투명 물체를 블렌딩합니다.<br>
이것이 ALL opaque(투명x) 물체를 씬에 먼저 렌더링 해야하는 이유입니다. <br>
투명 물체를 렌더링할때 문제로 들어가봅시다. 여기 두 박스가 있습니다. <br>
두 번째 상자의 주의를 도는 첫 번째 상자는, 두 번째 상자 뒤에서 시작하여 먼저 렌더링되도록 코딩이 되어져 있습니다.<br>
그러면 두번째 박스가 렌더링 되어질 때, 이미 렌더 타겟에 있는 첫 번째 박스와 블렌딩 할 수 있다. <br>
하지만, 첫번째 박스가 두번째 박스 주위를 돌다 두번째 박스 앞으로 온다면, 두번째 박스와 블렌딩 할 수 없을 것입니다.<br>
왜냐면 두번째 박스는 이미 렌더타겟에 없기 때문이죠.<br>

이 문제를 풀기 위해서 우리는 두 박스들과 카메라까지의 거리를 알아야 합니다. 어느 쪽이든 카메라에서 더 멀리 있는 것을 먼저 렌더링합니다.<br>
만약 많은 투명 물체를 가진다면, 벡터를 만들고 카메라에서 가장 먼 곳부터 가장 가까운 곳까지 모든 프레임을 구성하고, 벡터의 처음부터 끝까지 투명한 물체를 그리는 것이 최선일 것입니다.<br>

  
  
  
  https://www.braynzarsoft.net/viewtutorial/q16390-12-blending
