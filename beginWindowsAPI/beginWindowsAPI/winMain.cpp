#include "stdafx.h"
#include "SystemClass.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {

	//system ��ü ����
	SystemClass* System = new SystemClass;

	if (!System) { return -1; }

	//��ü �ʱ�ȭ �� ����
	if (System->Initialize()) { System->Run(); }

	//��ü ���� �� �޸� ��ȯ
	System->Shutdown();
	delete System;
	System = nullptr;
}