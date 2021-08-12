## 화면 실행에서 종료까지 최소한의 틀

targetVer.h

'''c++
#pragma once
// SDKDDKVer.h를 포함하면 최고 수준의 가용성을 가진 Windows 플랫폼이 정의됩니다.

// 이전 Windows 플랫폼에 대해 응용 프로그램을 빌드하려는 경우에는 SDKDDKVer.h를 포함하기 전에
// WinSDKVer.h를 포함하고 _WIN32_WINNT 매크로를 지원하려는 플랫폼으로 설정하십시오.

#include <SDKDDKVer.h>

'''

pch.h

'''c++

//pre compile header

#pragma once
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include <fstream>
using namespace std;


/////////////
// LINKING //
/////////////
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
using namespace DirectX;


'''


windowClass.h

```c++
#pragma once

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class InputClass;

class windowClass
{
public:
	windowClass();
	windowClass(const windowClass&);
	~windowClass();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows(int& ,int&);
	void ShutdownWindows();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	InputClass* m_Input = nullptr;

};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static windowClass* ApplicationHandle = 0;

```
	
windowClass.cpp
	
'''c++
	
#include "pch.h" //오류가 계속 뜬다면 pch.h를 가장 먼저 인클루드 해야함!~
#include "windowClass.h"
#include "InputClass.h"


windowClass::windowClass()
{
}

windowClass::~windowClass()
{
}

bool windowClass::Initialize()
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
	m_Input->Initialize();

	return true;
}

void windowClass::Shutdown()
{

	if (m_Input) {
		delete m_Input;
		m_Input = nullptr;
	}
	ShutdownWindows();
}

void windowClass::Run()
{	// 메시지 구조체 생성 및 초기화
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
				break;
		}
	}
}


bool windowClass::Frame()
{
	// ESC 키 감지 및 종료 여부를 처리합니다
	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}
	return true;
}

LRESULT CALLBACK windowClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam) {
	
	switch (umsg) {
		//키보드가 눌러졌는가
	case WM_KEYDOWN: //마우스 이벤트와 관련한 매크로들은 WM_~~
	{
		m_Input->KeyDown((unsigned int)wparam);
		return 0;
	}

	//키보드가 떨어졌는가
	case WM_KEYUP: 
	{
		m_Input->KeyUp((unsigned int)wparam);
		return 0;
	}

				 //모든 처리를 개발자가 할 수는 없기때문에 아무런 메세지가 없을 때 등의 경우
				 //윈도우가 제공하는 기본 메세지 처리로 넘김.
	default: 
	{
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
	}
}

void windowClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	ApplicationHandle = this;

	//이 프로그램의 인스턴스 가져오기
	m_hinstance = GetModuleHandle(NULL); //이래서 콜백??

	//프로그램 이름
	m_applicationName = L"DirectX_0812";

	//window class 지정
	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; //window 그리기 특성, 자세한 내용은 MSDM(https://docs.microsoft.com/en-us/windows/win32/winmsg/window-class-styles)
	wc.lpfnWndProc = WndProc; //메세지 처리 방식
	wc.cbClsExtra = 0; //추가 메모리 사용 안함
	wc.cbWndExtra = 0; //추가 메모리 사용 안함
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); //메인 윈도우 배경색 지정
	wc.hInstance = m_hinstance; //실행 인스턴스의 핸들 값 지정
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO); //제목 표시줄에 Logo 아이콘 사용
	wc.hIconSm = wc.hIcon; //
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); //화살표 커서 사용
	wc.lpszClassName = m_applicationName; //'Window Class'의 이름 지정
	wc.lpszMenuName = NULL; //메뉴 사용 안함
	wc.cbSize = sizeof(WNDCLASSEX); //

	//window class 등록
	RegisterClassEx(&wc);

	//** 모니터 화면 중앙에 오게 설정 **//
	//모니터 화면의 해상도 읽어옴.

	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	int posX = 0;
	int posY = 0;

	if (FULL_SCREEN) {
		//풀스크린 -> 모니터 화면 해상도 == 데스크톱 해상도, 색상은 32bit
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
	}
	else {
		//윈도우 모드의 경우
		screenWidth = 800;
		screenHeight = 600;

		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	//윈도우 생성 & 핸들 구하기.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, posX, posY, screenWidth, screenHeight,
		NULL, NULL, m_hinstance, NULL);

	//윈도우 화면 표시 & 포커스 지정
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

}

void windowClass::ShutdownWindows()
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




//Window procedure ; 어떤 메세지가 발생한 경우 '어떻게' 처리할 것인지를 정의한 함수
//메세지를 처리하는 함수!!
LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam) {
	switch (umessage)
	{
	case WM_DESTROY: //윈도우 종료 확인
		PostQuitMessage(0);
		return 0;

	case WM_CLOSE: //윈도우가 닫히는지 확인
		PostQuitMessage(0);
		return 0;

		//그 외의 모든 메세지들은 메시지처리로 넘김.
	default:
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
}
	
	
	
	'''
	
InputClass.h
	
	'''c++
	#pragma once
class InputClass
{
public:
	InputClass();
	InputClass(const InputClass&); //이건 무슨 용도지,,??
	~InputClass();

	void Initialize();
	void KeyDown(unsigned int);
	void KeyUp(unsigned int);

	bool IsKeyDown(unsigned int);

private:
	bool m_keys[256];
};


	'''
	
	InputClass.cpp
	
	'''c++
	#include "InputClass.h"

InputClass::InputClass()
{
}

InputClass::InputClass(const InputClass&)
{
}

InputClass::~InputClass()
{
}

void InputClass::Initialize()
{
	//키 배열 초기화
	for (int i = 0; i < 256; i++) {
		m_keys[i] = false;
	}
}

void InputClass::KeyDown(unsigned int input)
{
	m_keys[input] = true;
}

void InputClass::KeyUp(unsigned int input)
{
	m_keys[input] = false;
}

bool InputClass::IsKeyDown(unsigned int key)
{
	return m_keys[key];
}

	
	'''
