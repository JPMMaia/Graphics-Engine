// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

// TODO: reference additional headers your program requires here
#include <d3d12.h>
#include <dxgi1_5.h>
#include <wrl.h>
#include <cstdint>

#include "Common/Helpers.h"
#include "GraphicsEngine/d3dx12.h"
#include "GraphicsEngine/DirectXHelper.h"
