#include "stdafx.h"
#include "SystemClass.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {

	//system 객체 생성
	SystemClass* System = new SystemClass;

	if (!System) { return -1; }

	//객체 초기화 및 실행
	if (System->Initialize()) { System->Run(); }

	//객체 종료 및 메모리 반환
	System->Shutdown();
	delete System;
	System = nullptr;
}