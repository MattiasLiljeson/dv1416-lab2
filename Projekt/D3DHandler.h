#ifndef D3DHANDLER_H
#define D3DHANDLER_H

//external
#include <windows.h>
#include <windowsx.h>
#include <d3d10.h>
#include <d3dx10.h>
#include <vector>

// include the Direct3D Library file
#pragma comment (lib, "d3d10.lib")
#pragma comment (lib, "d3dx10.lib")

//stuph
#include "Timer.h"
#include "InputHandler.h"
#include "cBuffers.h"
#include "Camera.h"
#include "Light.h"

//objects
#include "D3DObject.h"
#include "Triangle.h"
#include "Hypercraft.h"
#include "Cube.h"
#include "HeightMap.h"
#include "ObjObject.h"
#include "Group.h"

//particles
#include "PaprikaFlame.h"
#include "WaterFountain.h"
#include "SuperNova.h"

#include"Node.h"

using namespace std;

class D3DHandler
{
private:
	//window settings
	int wndWidth;
	int wndHeight;
	HINSTANCE hInstance;
	HWND hWnd;

	//Direct3D
	ID3D10Device* device;
	ID3D10RenderTargetView* rtv;
	ID3D10DepthStencilView* dsv;    //depth stencil view - z/depth buffer
	IDXGISwapChain* swapchain; 

	//stuph
	Timer timer;
	InputHandler* input;
	Camera* camera;
	Camera* camera2; //for frustum culling debug
	vector<Light> lights;
	Node* node;

	//CBuffer, isn't used
	//ID3D10Buffer *cBuffer;

	//Effect (.fx)
	ID3D10Effect* effect;

	//FX-vars
	ID3D10EffectVariable* lightVariable;
	
	//rasterizer states
	ID3D10RasterizerState *rastStateDefault;		// the default rasterizer state
	ID3D10RasterizerState *rastStateWireframe;		//Debug rasterizer

	//stuff to render
	HeightMap* world;
	vector<D3DObject*> objects;
	vector<Group*> groups;
	vector<ParticleSystem*> particleSystems;

	//Debug
	DWORD shaderFlags;
	HRESULT hr;

	//internal functions
	void initWindow();
	void initD3D();
	void initEffect();
	void initFXvars();
	void initStates();

	void initLights();
	void initObjects();
	void initParticles();

public:
	D3DHandler(HINSTANCE _hInstance);
	~D3DHandler();
	void init();

	int run();
	void update(float _dt);
	void draw();
	void resize();

	//LRESULT CALLBACK msgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	//LRESULT msgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

#endif //D3DHANDLER_H