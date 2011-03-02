//_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

#include <windows.h>
#include <windowsx.h>

#include <string>

#include "D3DHandler.h"


//globals


int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
	D3DHandler d3dHandler(hInstance);
	d3dHandler.init();
	d3dHandler.run();

	return 0;
}