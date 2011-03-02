#include "InputHandler.h"

InputHandler::InputHandler(HINSTANCE* _hInstance, HWND* _hWnd)
{
	hInstance = _hInstance;
	hWnd = _hWnd;

	// create the DirectInput interface
    DirectInput8Create(*hInstance,    // the handle to the application
                       DIRECTINPUT_VERSION,    // the compatible version
                       IID_IDirectInput8,    // the DirectInput interface version
                       (void**)&din,    // the pointer to the interface
                       NULL);    // COM stuff, so we'll set it to NULL

    // create the keyboard device
    din->CreateDevice(GUID_SysKeyboard,    // the default keyboard ID being used
                      &dinkeyboard,    // the pointer to the device interface
                      NULL);    // COM stuff, so we'll set it to NULL
	din->CreateDevice(GUID_SysMouse,
                      &dinmouse,
                      NULL);

    // set the data format to keyboard format
    dinkeyboard->SetDataFormat(&c_dfDIKeyboard);
	dinmouse->SetDataFormat(&c_dfDIMouse);

    // set the control we will have over the keyboard
    dinkeyboard->SetCooperativeLevel(*hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	dinmouse->SetCooperativeLevel(*hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);

	reset();
}

InputHandler::~InputHandler()
{
    dinkeyboard->Unacquire();    // make sure the keyboard is unacquired
    dinmouse->Unacquire();    // make sure the mouse in unacquired
	din->Release();    // close DirectInput before exiting
}

void InputHandler::reset()
{
	wasd[W] = 0;
	wasd[A] = 0;
	wasd[S] = 0;
	wasd[D] = 0;

	mouse[0] = 0;
	mouse[1] = 0;
}

void InputHandler::detectInput(void)
{
    // get access if we don't have it already
    dinkeyboard->Acquire();
	dinmouse->Acquire();

    // get the input data
    dinkeyboard->GetDeviceState(256, (LPVOID)keystate);
	dinmouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mousestate);
}

void InputHandler::update()
{
	reset();
	detectInput();
	
	if(keystate[DIK_UP] & 0x80)
        wasd[W] = true;
	if(keystate[DIK_LEFT] & 0x80)
        wasd[A] = true;
	if(keystate[DIK_DOWN] & 0x80)
        wasd[S] = true;
    if(keystate[DIK_RIGHT] & 0x80)
        wasd[D] = true;

	if(keystate[DIK_ESCAPE] & 0x80)
            PostMessage(*hWnd, WM_DESTROY, 0, 0);
}

bool InputHandler::getWasd(int key)
{
	return wasd[key];
}

long InputHandler::getMouse(int axis)
{
	if(axis == X)
		return mousestate.lX;
	else
		return mousestate.lY;
}