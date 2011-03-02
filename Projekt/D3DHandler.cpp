#include "D3DHandler.h"

LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//BAD HACK. C/P with multiple returns
    switch(message)
    {
        case WM_DESTROY:
            {
                PostQuitMessage(0);
                return 0;
            } break;
    }

    // Handle any messages the switch statement didn't
    return DefWindowProc (hWnd, message, wParam, lParam);
}

D3DHandler::D3DHandler(HINSTANCE _hInstance)
{
	hInstance = _hInstance;
	hWnd = NULL;

	wndWidth = 1920;
	wndHeight = 1080;
}

D3DHandler::~D3DHandler()
{
	delete camera;
	delete world;
	delete input;


	//Some COMs aren't released now, must be fixed
	SAFE_RELEASE(device);    // close and release the 3D device
	SAFE_RELEASE(rtv);
	SAFE_RELEASE(dsv);
	SAFE_RELEASE(swapchain);
	SAFE_RELEASE(effect);
	//SAFE_RELEASE(lightVariable);
	SAFE_RELEASE(rastStateDefault);
	SAFE_RELEASE(rastStateWireframe);
    SAFE_RELEASE(rtv);

	for(UINT i = 0; i<objects.size(); i++)
		delete objects[i];

	for(UINT i = 0; i<groups.size(); i++)
		delete groups[i];

	for(UINT i = 0; i<particleSystems.size(); i++)
		delete particleSystems[i];
}

void D3DHandler::init()
{
	//Debug
	shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
	#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
	#endif

	initWindow();
	initD3D();
	initEffect();
	initStates();

	camera = new Camera(wndWidth, wndHeight, 1.0f, 100.0f, device, effect);
	camera2 = new Camera(wndWidth, wndHeight, 1.0f, 10.0f, device, effect);
	input = new InputHandler(&hInstance, &hWnd);

	initLights();
	initObjects();
	initParticles();

	initFXvars();

	world = new HeightMap(device, effect, 2, 0);
	world->init();

	node = new Node(D3DXVECTOR3(-100,-20,-100),D3DXVECTOR3(100,20,100), 4, device, effect, 3,0);
	node->initDrawStuff();
}

void D3DHandler::initWindow()
{
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = wndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = L"WindowClass";

	RegisterClassEx(&wc);

	//create window, save result
	hWnd = CreateWindowEx(NULL,
							L"WindowClass",
							L"Window",
							WS_OVERLAPPEDWINDOW,
							0,
							0,
							wndWidth,
							wndHeight,
							NULL,
							NULL,
							hInstance,
							NULL);

	ShowWindow(hWnd, SW_SHOW);
}

void D3DHandler::initD3D()
{
	DXGI_SWAP_CHAIN_DESC scd;    //Swap Chain Device
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));    //Clear before use

	scd.BufferCount = 1;    //Two buffers
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hWnd;
    scd.SampleDesc.Count = 1;    //Level of multi sampling, 1 = No
    scd.SampleDesc.Quality = 0;    //Quality of multi-sampling
    scd.Windowed = TRUE;

	//Create a device and swap chain using info from scd
    D3D10CreateDeviceAndSwapChain(NULL,
                                  D3D10_DRIVER_TYPE_HARDWARE,
                                  NULL,
                                  0,
                                  D3D10_SDK_VERSION,
                                  &scd,
                                  &swapchain,
                                  &device);

	// create a texture for the depth buffer
    D3D10_TEXTURE2D_DESC zbd;
    ZeroMemory(&zbd, sizeof(zbd));
    zbd.Width = wndWidth;    // set the width of the depth buffer
    zbd.Height = wndHeight;    // set the height of the depth buffer
    zbd.ArraySize = 1;    // we are only creating one texture
    zbd.SampleDesc.Count = 1;    // no multi-sampling
    zbd.Format = DXGI_FORMAT_D32_FLOAT;    // one 32-bit float per pixel
    zbd.BindFlags = D3D10_BIND_DEPTH_STENCIL;    // texture is to be used as a depth buffer
    ID3D10Texture2D* pDepthBuffer;
    device->CreateTexture2D(&zbd, NULL, &pDepthBuffer);    // create the texture

    // create the depth buffer
    D3D10_DEPTH_STENCIL_VIEW_DESC dsvd;
    ZeroMemory(&dsvd, sizeof(dsvd));
    dsvd.Format = DXGI_FORMAT_D32_FLOAT;    // one 32-bit float per pixel
    dsvd.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;    // depth buffer is a 2D texture
    device->CreateDepthStencilView(pDepthBuffer, &dsvd, &dsv);    // create the depth buffer
    pDepthBuffer->Release();    // release the texture once the depth buffer is made

	//Get the address of the back buffer and use it to create the render target
    ID3D10Texture2D* pBackBuffer;
    swapchain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&pBackBuffer);
    device->CreateRenderTargetView(pBackBuffer, NULL, &rtv);
    pBackBuffer->Release();   //pBackBuffer no longer needed, release to free RAM

	//Set the render target as the back buffer
    device->OMSetRenderTargets(1, &rtv, dsv);

    D3D10_VIEWPORT viewport;

    ZeroMemory(&viewport, sizeof(D3D10_VIEWPORT));

    viewport.TopLeftX = 0;    
    viewport.TopLeftY = 0;    
    viewport.Width = wndWidth;    
    viewport.Height = wndHeight;
	viewport.MinDepth = 0;    // the closest an object can be on the depth buffer is 0.0
    viewport.MaxDepth = 1;    // the farthest an object can be on the depth buffer is 1.0

    device->RSSetViewports(1, &viewport);    //Set the viewport
}

void D3DHandler::initEffect()
{
	//Send blob to gather debug data
	ID3D10Blob* compilationErrors = 0;
	hr = 0;
	hr = D3DX10CreateEffectFromFile(L"effect.fx", 0, 0,
									"fx_4_0", shaderFlags, 0,
									device, 0, 0,
									&effect, &compilationErrors, 0);
	if(FAILED(hr))
	{
		if( compilationErrors )
		{
			MessageBoxA(0, (char*)compilationErrors->GetBufferPointer(), 0, 0);
			SAFE_RELEASE(compilationErrors);
		}
	}
}

void D3DHandler::initFXvars()
{
	//lights
	lightVariable = effect->GetVariableByName("light");
	lightVariable->SetRawValue(&lights[0], 0, sizeof(Light));
}

void D3DHandler::initStates()
{
	D3D10_RASTERIZER_DESC rasterizerDesc;
    rasterizerDesc.FillMode = D3D10_FILL_SOLID;
    rasterizerDesc.CullMode = D3D10_CULL_NONE;
    rasterizerDesc.FrontCounterClockwise = FALSE;
    rasterizerDesc.DepthClipEnable = TRUE;
    rasterizerDesc.ScissorEnable = FALSE;
    rasterizerDesc.AntialiasedLineEnable = FALSE;
    rasterizerDesc.MultisampleEnable = FALSE;
    rasterizerDesc.DepthBias = 0;
    rasterizerDesc.DepthBiasClamp = 0.0f;
    rasterizerDesc.SlopeScaledDepthBias = 0.0f;

    device->CreateRasterizerState(&rasterizerDesc, &rastStateDefault);

    // set the changed values for wireframe mode
    rasterizerDesc.FillMode = D3D10_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D10_CULL_NONE;
    rasterizerDesc.AntialiasedLineEnable = TRUE;

    device->CreateRasterizerState(&rasterizerDesc, &rastStateWireframe);

	if(true)
        device->RSSetState(rastStateWireframe);
    else
        device->RSSetState(rastStateDefault);
}

void D3DHandler::initLights()
{
	Light light;
	light.ambient  = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f);
	light.diffuse  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	light.specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	light.att.x    = 0.0f;
	light.att.y    = 0.0f;
	light.att.z    = 0.01f;
	light.range    = 50.0f;
	light.position.x = 0.0f;
	light.position.y = 10.0f;
	light.position.z = 0.0f;

	lights.push_back(light);
}

void D3DHandler::initObjects()
{
	objects.push_back(new Cube(device, effect, 0, 0));
	objects.push_back(new Hypercraft(device, effect, 0, 0));
	//objects.push_back(new ObjObject(device, effect, 0, 0));

	groups.push_back(new Group(device, effect, "bth.obj", 0,0));

	for(UINT i = 0; i<objects.size(); i++)
		objects[i]->init();

	for(UINT i = 0; i<groups.size(); i++)
		groups[i]->init();

	for(UINT i = 0; i<particleSystems.size(); i++)
		particleSystems[i]->init();
}

void D3DHandler::initParticles()
{
	particleSystems.push_back(new PaprikaFlame(device, effect, camera, 1,0));
	particleSystems.push_back(new WaterFountain(device, effect, camera, 1,0));
	particleSystems.push_back(new SuperNova(device, effect, camera, 1,0));
	
	for(UINT i = 0; i<particleSystems.size(); i++)
		particleSystems[i]->init();
}

int D3DHandler::run()
{
	MSG msg = {0};
	timer.reset();

	while(msg.message != WM_QUIT)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			timer.tick();
			update(timer.getDt());
			draw();
		}
	}
	return msg.wParam;
}

void D3DHandler::update(float _dt)
{
	int W = 0;
	int A = 1;
	int S = 2;
	int D = 3;
	int X = 0;
	int Y = 1;

	//handle input
	input->update();
	if(input->getWasd(W))
		camera->walk(25.0f, _dt);
	if(input->getWasd(A))
		camera->strafe(-25.0f, _dt);
	if(input->getWasd(S))
		camera->walk(-25.0f, _dt);
	if(input->getWasd(D))
		camera->strafe(25.0f, _dt);

	camera->rotateY((float)input->getMouse(X)/1000, 1);
	camera->pitch((float)input->getMouse(Y)/1000, 1);
	
	float y;
	y = world->getHeight(camera->getPos().x, camera->getPos().z);

	//y = 100; //debug

	camera->update(1, y);

	world->update( _dt);
	for(UINT i = 0; i<objects.size(); i++)
		objects[i]->update( _dt);

	for(UINT i = 0; i<groups.size(); i++)
		groups[i]->update( _dt);

	for(UINT i = 0; i<particleSystems.size(); i++)
		particleSystems[i]->update( _dt);
}

void D3DHandler::draw()
{
	// clear the window to a deep blue
    device->ClearRenderTargetView(rtv, D3DXCOLOR(0.0f, 0.2f, 0.4f, 1.0f));
    device->ClearDepthStencilView(dsv, D3D10_CLEAR_DEPTH, 1.0f, 0);

	// reset states
	device->OMSetDepthStencilState(0, 0);
	float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};
	device->OMSetBlendState(0, blendFactors, 0xffffffff);

    // do 3D rendering on the back buffer here
	camera2->draw();
	//camera->draw();

	world->draw();
	for(UINT i = 0; i<objects.size(); i++)
		objects[i]->draw();

	for(UINT i = 0; i<groups.size(); i++)
		groups[i]->draw();

	for(UINT i = 0; i<particleSystems.size(); i++)
		//particleSystems[i]->draw();

	//node->draw();

    // display the rendered frame
    swapchain->Present(0, 0);
}

void D3DHandler::resize()
{
}