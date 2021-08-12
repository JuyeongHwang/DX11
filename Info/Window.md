## 화면 실행에서 종료까지 최소한의 틀
### visual studio 2019 


// Windows 헤더 파일:
#include <windows.h>


LPCTSTR WndClassName = L"firstWindow";
HWND hwnd = NULL;

const int Width = 800;
const int Height = 600;

//window 만들기 성공 : true, 아니면 false를 반환할 것임.
bool InitializeWindow(HINSTANCE hInstance, int ShowWnd, int width, int height, bool windowed);

//프로그램이 돌아갈 곳.
int messageloop();

// window callback procedure도 설정해줘야함.
// 윈도우의 메세지를 처리하는 곳입니다
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lParam);



///****< Main Windows Fuction >****///

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

	if (!InitializeWindow(hInstance, nShowCmd, Width, Height, true)) {

		MessageBox(0, L"Window Initialization - Failed", L"Error", MB_OK);

		return 0;
	}


	messageloop();


	return 0;
}


bool InitScene()
{
	return true;
}

/// <summary>
/// 
/// </summary>
/// <param name="hInstance"> This is the handle to our current application</param>
/// <param name="ShowWnd"> How the window should be displayed. Some common commands are SW_SHOWMAXIMIZED, SW_SHOW, SW_SHOWMINIMIZED.</param>
/// <param name="width"> Width of the window in pixels</param>
/// <param name="height"> Height of the window in pixels</param>
/// <param name="windowed"> False if the window is fullscreen and true if the window is windowed </param>
/// <returns></returns>
bool InitializeWindow(HINSTANCE hInstance, int ShowWnd, int width, int height, bool windowed) {


	//1. WNDCLASSEX 만들기
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);

	//CS_HREDRAW : 더블클릭으로 윈도우 조절 가능
	//CS_VREDRAW : 윈도우의 높이가 변하거나 수직으로 움직여도 다시 그려줌
	wc.style = CS_HREDRAW | CS_VREDRAW;

	wc.lpfnWndProc = WndProc;

	//WNDCLASSEX 생성 후에 추가적인 메모리 할당 수
	wc.cbClsExtra = NULL;
	//window instance 후에 추가적인 메모리 할당 수
	wc.cbWndExtra = NULL;

	//GetModuleHandle()
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

	hwnd = CreateWindowEx(
		NULL,
		WndClassName,
		L"Geometry Dash",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		width,
		height,
		NULL,
		NULL,
		hInstance,
		NULL);


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

	/*
LPMSG lpMsg ; msg 포인터
HWND hwnd ; 메세지를 보내는 윈도우의 핸들.
UINT wMsgFilterMin ; 메세지 범위 중(message quere 얘기하는 듯) 첫 메세지의 값이 조사되었는지 확인하고,
wMsgFilterMin과 wMsgFilterMax가 둘다 0이라면, PeekMessage는 모든 메세지를 조사할 것이다.
UINT wMsgFilterMax ; 메세지 큐 중 마지막 값을 확인한다.
UINT wRemoveMsg ; 메세지가 어떻게 관리되는지에 관한 변수. 우리가 PM_REMOVE로 세팅하면 메세지는 읽힌 후, 삭제될 것이다.
*/

//while there is a message...
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
			//run game code
		}
	}

	return (int)msg.wParam;
}

/// <summary>
/// 
/// </summary>
/// <param name="hwnd">
/// the handle to the window that got the message.
/// </param>
/// <param name="msg">
/// the contents of the message.
/// </param>
/// <param name="wParam", "lParam">
/// wParam and lParam are extra information about the message.
/// we will be using wParam to detect keyboard input.
/// </param>
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			if (MessageBox(0, L"Are you sure you want to exit?", L"Really?", MB_YESNO | MB_ICONQUESTION) == IDYES)
				DestroyWindow(hwnd);
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}


