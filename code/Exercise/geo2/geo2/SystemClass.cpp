#include "stdafx.h"
#include "inputclass.h"
#include "graphicsclass.h"
#include "TimerClass.h"
#include "PositionClass.h"
#include "systemclass.h"


SystemClass::SystemClass()
{
}


SystemClass::SystemClass(const SystemClass& other)
{
}


SystemClass::~SystemClass()
{
}


bool SystemClass::Initialize()
{
	// 윈도우 창 가로, 세로 넓이 변수 초기화
	int screenWidth = 0;
	int screenHeight = 0;

	// 윈도우 생성 초기화
	InitializeWindows(screenWidth, screenHeight);

	// m_Input 객체 생성. 이 클래스는 추후 사용자의 키보드 입력 처리에 사용됩니다.
	m_Input = new InputClass;
	if (!m_Input)
	{
		return false;
	}

	// m_Input 객체 초기화
	if(!m_Input->Initialize(m_hinstance, m_hwnd, screenWidth, screenHeight))
	{
		MessageBox(m_hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	// m_Graphics 객체 생성.  그래픽 랜더링을 처리하기 위한 객체입니다.
	m_Graphics = new GraphicsClass;
	if (!m_Graphics)
	{
		return false;
	}

	// m_Graphics 객체 초기화.
	if(!m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd))
	{
		return false;
	}
	
	// 타이머 객체를 만듭니다.
	m_Timer = new TimerClass;
	if(!m_Timer)
	{
		return false;
	}

	// 타이머 객체를 초기화합니다.
	if(!m_Timer->Initialize())
	{
		MessageBox(m_hwnd, L"Could not initialize the Timer object.", L"Error", MB_OK);
		return false;
	}

	// 위치 개체를 생성합니다.
	m_Position = new PositionClass;
	if(!m_Position)
	{
		return false;
	}

	return true;
}


void SystemClass::Shutdown()
{
	// 위치 객체 반환
	if(m_Position)
	{
		delete m_Position;
		m_Position = 0;
	}

	// 타이머 객체를 해제합니다.
	if(m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}

	// m_Graphics 객체 반환
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	// m_Input 객체 반환
	if (m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}

	// Window 종료 처리
	ShutdownWindows();
}


void SystemClass::Run()
{
	// 메시지 구조체 생성 및 초기화
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	// 사용자로부터 종료 메시지를 받을때까지 메시지루프를 돕니다
	while (true)
	{
		// 윈도우 메시지를 처리합니다
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// 종료 메시지를 받을 경우 메시지 루프를 탈출합니다
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// 그 외에는 Frame 함수를 처리합니다.
			if (!Frame())
			{
				MessageBox(m_hwnd, L"Frame Processing Failed", L"Error", MB_OK);
				break;
			}
		}
		
		// 사용자가 ESC키를 눌렀는지 확인 후 종료 처리함
		if(m_Input->IsEscapePressed() == true)
		{
			break;
		}
	}
}


bool SystemClass::Frame()
{
	// 시스템 통계를 업데이트 합니다
	m_Timer->Frame();

	// 입력 프레임 처리를 수행합니다
	if(!m_Input->Frame())
	{
		return false;
	}

	// 업데이트 된 위치를 계산하기 위한 프레임 시간을 설정합니다.
	m_Position->SetFrameTime(m_Timer->GetTime());

	// 왼쪽 또는 오른쪽 화살표 키를 눌렀는지 확인하십시오. 그렇지 않으면 카메라를 적절히 회전하십시오.
	bool keyDown = m_Input->IsLeftArrowPressed();
	m_Position->TurnLeft(keyDown);

	keyDown = m_Input->IsRightArrowPressed();
	m_Position->TurnRight(keyDown);

	// 현재 뷰 포인트 회전을 가져옵니다.
	float rotationY = 0.0f;
	m_Position->GetRotation(rotationY);

	// 그래픽 객체에 대한 프레임 처리를 수행합니다.
	if(!m_Graphics->Frame(rotationY))
	{
		return false;
	}

	return m_Graphics->Render();
}


LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}


void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	// 외부 포인터를 이 객체로 지정합니다
	ApplicationHandle = this;

	// 이 프로그램의 인스턴스를 가져옵니다
	m_hinstance = GetModuleHandle(NULL);

	// 프로그램 이름을 지정합니다
	m_applicationName = L"Dx11Demo_16";

	// windows 클래스를 아래와 같이 설정합니다.
	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// windows class를 등록합니다
	RegisterClassEx(&wc);

	// 모니터 화면의 해상도를 읽어옵니다
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	int posX = 0;
	int posY = 0;

	// FULL_SCREEN 변수 값에 따라 화면을 설정합니다.
	if (FULL_SCREEN)
	{
		// 풀스크린 모드로 지정했다면 모니터 화면 해상도를 데스크톱 해상도로 지정하고 색상을 32bit로 지정합니다.
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// 풀스크린으로 디스플레이 설정을 변경합니다.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
	}
	else
	{
		// 윈도우 모드의 경우 800 * 600 크기를 지정합니다.
		screenWidth = 800;
		screenHeight = 600;

		// 윈도우 창을 가로, 세로의 정 가운데 오도록 합니다.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// 윈도우를 생성하고 핸들을 구합니다.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// 윈도우를 화면에 표시하고 포커스를 지정합니다
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);
}


void SystemClass::ShutdownWindows()
{
	// 풀스크린 모드였다면 디스플레이 설정을 초기화합니다.
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// 창을 제거합니다
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// 프로그램 인스턴스를 제거합니다
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// 외부포인터 참조를 초기화합니다
	ApplicationHandle = NULL;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		// 윈도우 종료를 확인합니다
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	// 윈도우가 닫히는지 확인합니다
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	// 그 외의 모든 메시지들은 시스템 클래스의 메시지 처리로 넘깁니다.
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}