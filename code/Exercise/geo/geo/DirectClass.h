#pragma once
class DirectClass
{

public:
	struct VertexType {
		VertexType() {} //기본 생성자. 없으면 오류난다.
		VertexType(float x, float y, float z,
			float u, float v)
			: pos(x, y, z), texCoord(u, v) {}

		XMFLOAT3 pos;
		XMFLOAT2 texCoord;
	};
public:

	bool InitializeDirect(HWND hwnd, int screenWidth, int screenHeight);
	bool InitD2D_D3D101_DWrite(IDXGIAdapter1* Adapter, int screenWidth, int screenHeight);

	void DXBeginScene();

	void InitD2DScreenTexture(); //graphic부분으로 이전할 애
	void RenderText(std::wstring text, float left, float top, float right, float bottom);//graphic부분으로 이전할 애
	
	void DXEndScene();
	ID3D11Device* getDevice();
	ID3D11DeviceContext* getDevContext();

private:
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_deviceContext= nullptr;
	IDXGISwapChain* m_swapChain = nullptr;
	ID3D11RenderTargetView* m_renderTargetView = nullptr;
	ID3D11DepthStencilView* m_depthStencilView = nullptr;
	ID3D11Texture2D* m_depthStencilBuffer = nullptr;
	//FOR FONT
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
	wstring printText;
};