#include "stdafx.h"
#include "d3dclass.h"
#include "cameraclass.h"
#include "textclass.h"
#include "modelclass.h"
#include "lightshaderclass.h"
#include "lightclass.h"
#include "modellistclass.h"
#include "frustumclass.h"
#include "graphicsclass.h"


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
	if(!m_Direct3D)
	{
		return false;
	}

	// Direct3D ��ü �ʱ�ȭ
	if(!m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR))
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// m_Camera ��ü ����
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}

	// ī�޶� ������ ����
	XMMATRIX baseViewMatrix;
	m_Camera->SetPosition(0.0f, 0.0f, -1.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);

	// m_Text ��ü ����
	m_Text = new TextClass;
	if(!m_Text)
	{
		return false;
	}

	// m_Text ��ü �ʱ�ȭ
	if (!m_Text->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix))
	{
		MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
		return false;
	}

	// �� ��ü ����
	m_Model = new ModelClass;
	if(!m_Model)
	{
		return false;
	}

	// �� ��ü �ʱ�ȭ
	if(!m_Model->Initialize(m_Direct3D->GetDevice(), (WCHAR*)L"data/seafloor.dds", (char*)"data/sphere.txt"))
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// ���� ���̴� ��ü ����
	m_LightShader = new LightShaderClass;
	if(!m_LightShader)
	{
		return false;
	}

	// ���� ���̴� ��ü �ʱ�ȭ
	if(!m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// ���� ��ü ����
	m_Light = new LightClass;
	if(!m_Light)
	{
		return false;
	}

	// ���� ��ü �ʱ�ȭ
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);

	// �� ��� ��ü ����
	m_ModelList = new ModelListClass;
	if(!m_ModelList)
	{
		return false;
	}

	// �� ��� ��ü �ʱ�ȭ
	if(!m_ModelList->Initialize(25))
	{
		MessageBox(hwnd, L"Could not initialize the model list object.", L"Error", MB_OK);
		return false;
	}

	// �������� ��ü ����
	m_Frustum = new FrustumClass;
	if(!m_Frustum)
	{
		return false;
	}

	return true;
}


void GraphicsClass::Shutdown()
{
	// �������� ��ü ��ȯ
	if(m_Frustum)
	{
		delete m_Frustum;
		m_Frustum = 0;
	}

	// �� ��� ��ü ��ȯ
	if(m_ModelList)
	{
		m_ModelList->Shutdown();
		delete m_ModelList;
		m_ModelList = 0;
	}

	// ���� ��ü ��ȯ
	if(m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// ���� ���̴� ��ü ��ȯ
	if(m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	// �� ��ü ��ȯ
	if(m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}
	
	// m_Text ��ü ��ȯ
	if (m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}

	// m_Camera ��ü ��ȯ
	if (m_Camera)
	{
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


bool GraphicsClass::Frame(float rotationY)
{
	// ī�޶� ��ġ ����
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

	// ī�޶��� ȸ���� �����մϴ�
	m_Camera->SetRotation(0.0f, rotationY, 0.0f);

	return true;
}


bool GraphicsClass::Render()
{
	float positionX = 0;
	float positionY = 0;
	float positionZ = 0;
	float radius = 1.0f; // ���� �������� 1.0f�� ����
	XMFLOAT4 color;

	// ���� �׸��� ���� ���۸� ����ϴ�
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶��� ��ġ�� ���� �� ����� �����մϴ�
	m_Camera->Render();

	// ī�޶� �� d3d ��ü���� ����, �� �� ���� ����� �����ɴϴ�
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// ����ü�� ����ϴ�.
	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);

	// ������ �� ���� ���� ��´�.
	int modelCount = m_ModelList->GetModelCount();

	//  ������ �� ���� ������ �ʱ�ȭ�մϴ�.
	int renderCount = 0;

	// ��� ���� ���캸�� ī�޶� �信�� �� �� �ִ� ��쿡�� �������մϴ�.
	for(int index=0; index<modelCount; index++)
	{
		//�� �ε������� ���� ���� ��ġ�� ������ �����ɴϴ�.
		m_ModelList->GetData(index, positionX, positionY, positionZ, color);

		// ���� ���� �� frustum�� �ִ��� Ȯ���մϴ�.
		if(m_Frustum->CheckSphere(positionX, positionY, positionZ, radius))
		{
			// ���� ������ �� ��ġ�� �̵��մϴ�.
			worldMatrix = XMMatrixTranslation(positionX, positionY, positionZ);

			// �� ���ؽ��� �ε��� ���۸� �׷��� ������ ���ο� ��ġ�Ͽ� ������� �غ��մϴ�.
			m_Model->Render(m_Direct3D->GetDeviceContext());

			// ����Ʈ ���̴��� ����Ͽ� ���� �������մϴ�.
			m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
								  m_Model->GetTexture(), m_Light->GetDirection(), color);

			// ������ ���� ��Ʈ������ ����.
			m_Direct3D->GetWorldMatrix(worldMatrix);

			//�� ���� ������ �Ǿ��� �������� �������� ���� �ø��ϴ�.
			renderCount++;
		}
	}

	//�� �����ӿ��� ������ ������ �� ���� ���� �����մϴ�.
	if(!m_Text->SetRenderCount(renderCount, m_Direct3D->GetDeviceContext()))
	{
		return false;
	}

	// ��� 2D �������� �����Ϸ��� Z ���۸� ���ϴ�.
	m_Direct3D->TurnZBufferOff();

	// �ؽ�Ʈ�� �������ϱ� ���� ���� ������ �մϴ�
	m_Direct3D->TurnOnAlphaBlending();

	// �ؽ�Ʈ ���ڿ��� ������ �մϴ�
	if(!m_Text->Render(m_Direct3D->GetDeviceContext(), worldMatrix, orthoMatrix))
	{
		return false;
	}

	// �ؽ�Ʈ�� ������ �� �� ���� ������ �����մϴ�
	m_Direct3D->TurnOffAlphaBlending();

	// ��� 2D �������� �Ϸ�Ǿ����Ƿ� Z ���۸� �ٽ� �ѽʽÿ�.
	m_Direct3D->TurnZBufferOn();

	// ������ ������ ȭ�鿡 ����մϴ�
	m_Direct3D->EndScene();

	return true;
}