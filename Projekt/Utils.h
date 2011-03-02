#ifndef UTILS_H
#define UTILS_H

#pragma once

#include <d3dx10.h>
#include <dxerr.h>
#include <cassert>

//*****************************************************************************
// Simple d3d error checker for book demos.
//*****************************************************************************

#if defined(DEBUG) | defined(_DEBUG)
	#ifndef HR
	#define HR(x)                                              \
	{                                                          \
		HRESULT hr = (x);                                      \
		if(FAILED(hr))                                         \
		{                                                      \
			DXTrace(__FILE__, (DWORD)__LINE__, hr, L#x, true); \
		}                                                      \
	}
	#endif

#else
	#ifndef HR
	#define HR(x) (x)
	#endif
#endif

#define SAFE_RELEASE(x) if( x ) { x->Release(); (x) = NULL; }

#endif //UTILS_H