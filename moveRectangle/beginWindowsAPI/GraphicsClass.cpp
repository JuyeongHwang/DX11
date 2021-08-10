#include "stdafx.h"
#include "d3dclass.h"
#include "graphicsclass.h"
#include "ColorShaderClass.h"
#include "CameraClass.h"
#include "ModelClass.h"
GraphicsClass::GraphicsClass()
{
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	// Direct3D ��ü ����
	m_Direct3D = new D3DClass;
	if (!m_Direct3D)
	{
		return false;
	}

	// Direct3D ��ü �ʱ�ȭ
	if (!m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR))
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}


	//camera..
	m_Camera = new CameraClass;
	if (!m_Camera) {
		return false;
	}

	m_Camera->SetPosition(0.f, 0.f, -5.f);


	//m_Model
	m_Model = new ModelClass;
	if (!m_Model) {
		return false;
	}

	if (!m_Model->Initialize(m_Direct3D->GetDevice())) {
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}


	// 
	//m_ColorShader ��ü ����
	m_ColorShader = new ColorShaderClass;
	if (!m_ColorShader) {
		return false;
	}

	if (!m_ColorShader->Initialize(m_Direct3D->GetDevice(), hwnd)) {
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}


void GraphicsClass::Shutdown()
{
	//m_ColorShader ��ü ��ȯ
	if (m_ColorShader) {
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}

	if (m_Model) {
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	if (m_Camera) {
		delete m_Camera;
		m_Camera = 0;
	}
	// Direct3D ��ü ��ȯ
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}

	
}


bool GraphicsClass::Frame()
{
	// �׷��� ������ ó��
	itrans += 0.01f;
	return Render();
}


bool GraphicsClass::Render()
{
	// ���� �׸��� ���� ���۸� ����ϴ�
	m_Direct3D->BeginScene(0.2f, 1.0f, 1.0f, 1.0f);

	m_Camera->Render();

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	//��ü �̵���Ű��
	XMMATRIX translationMatrix(
		1.0f, 0.0f, 0.0f, itrans,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	m_Direct3D->GetWorldMatrix(worldMatrix);

	//ī�޶� �����̱�
	m_Camera->SetPosition(itrans, 3.f, -5.f);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	
	m_Model->Render(m_Direct3D->GetDeviceContext());

	if(!m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(),
		worldMatrix * XMMatrixTranspose(translationMatrix),	viewMatrix , projectionMatrix )) { return false; }

	// ������ ������ ȭ�鿡 ����մϴ�
	m_Direct3D->EndScene();

	return true;
}