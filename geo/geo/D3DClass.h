#pragma once
class D3DClass
{
public:
	D3DClass(){}
	~D3DClass(){}

public:
	bool InitializeDirect3d11App(HINSTANCE hInstance, HWND hwnd, int Width, int Height);
	void ReleaseD3D();
	bool InitD2D_D3D101_DWrite(IDXGIAdapter1* Adapter, int Width, int Height);
	void InitD2DScreenTexture();
	ID3D11Device* getDevice();
	ID3D11DeviceContext* getDContext();


private:
	IDXGISwapChain* SwapChain;
	ID3D11Device* d3d11Device;
	ID3D11DeviceContext* d3d11DevCon;
	ID3D11RenderTargetView* renderTargetView;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11Texture2D* depthStencilBuffer;

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
};

