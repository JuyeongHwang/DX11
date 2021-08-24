#pragma once

#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <string>
#include <fstream>
using namespace std;

/////////////
// LINKING //
/////////////
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#pragma comment (lib, "dwrite.lib")
#pragma comment (lib, "D3D10_1.lib")
#pragma comment (lib, "DXGI.lib")
#pragma comment (lib, "D2D1.lib")

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")
#include <dinput.h>
//////////////
// INCLUDES //
//////////////
#include <dwrite.h>
#include <d2d1.h>
#include <d3d10_1.h>
#include <sstream>
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
using namespace DirectX;
