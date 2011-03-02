#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <dinput.h>

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

class InputHandler
{
private:
	static const int W = 0;
	static const int A = 1;
	static const int S = 2;
	static const int D = 3;
	static const int X = 0;
	static const int Y = 1;

	HINSTANCE* hInstance;
	HWND* hWnd;
	LPDIRECTINPUT8 din;    // the pointer to our DirectInput interface
	LPDIRECTINPUTDEVICE8 dinkeyboard;    // the pointer to the keyboard device
	LPDIRECTINPUTDEVICE8 dinmouse;    // the pointer to the mouse device
	BYTE keystate[256];    // the storage for the key-information
	DIMOUSESTATE mousestate;    // the storage for the mouse-information

	bool wasd[4];

	long mouse[2];

public:
	InputHandler(HINSTANCE* hInstance, HWND* hWnd); // sets up and initializes DirectInput
	~InputHandler();	//closes DirectInput and releases memory
	void reset();
	void detectInput(void);    // gets the current input state
	void update();
	bool getWasd(int key);
	long getMouse(int axis);
};

#endif //INPUTHANDLER_H