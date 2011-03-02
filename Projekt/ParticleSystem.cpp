#include "Utils.h"
#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(ID3D10Device* _device, ID3D10Effect* _effect, Camera* _camera, int _techNr, int _passNr)
{
	device = _device;
	effect = _effect;
	camera = _camera;
	techNr = _techNr;
	passNr = _passNr;
}

ParticleSystem::~ParticleSystem()
{
	delete layout;
}

void ParticleSystem::init()
{
	defineVars();
	defineVertices();

	defineBlendState();
	defineDepthStencilState();
	defineInputElementDesc();

	initVertexBuffer();
	initTextures();
	initPipeline();
	initInputLayout();
}

void ParticleSystem::defineVars()
{
}

void ParticleSystem::defineBlendState()
{
	D3D10_BLEND_DESC blendDesc = {0};
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.BlendEnable[0] = true;
	blendDesc.SrcBlend       = D3D10_BLEND_ONE;
	blendDesc.DestBlend      = D3D10_BLEND_ONE;
	blendDesc.BlendOp        = D3D10_BLEND_OP_ADD;
	blendDesc.SrcBlendAlpha  = D3D10_BLEND_ONE;
	blendDesc.DestBlendAlpha = D3D10_BLEND_ZERO;
	blendDesc.BlendOpAlpha   = D3D10_BLEND_OP_ADD;
	blendDesc.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;

	HR(device->CreateBlendState(&blendDesc, &blendState));
}

void ParticleSystem::defineDepthStencilState()
{
	D3D10_DEPTH_STENCIL_DESC dsDesc;
	dsDesc.DepthEnable      = true;
	dsDesc.DepthWriteMask   = D3D10_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthFunc        = D3D10_COMPARISON_LESS; 
    dsDesc.StencilEnable    = false;
    dsDesc.StencilReadMask  = 0xff;
    dsDesc.StencilWriteMask = 0xff;

    dsDesc.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
	dsDesc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_KEEP;

	dsDesc.BackFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
    dsDesc.BackFace.StencilPassOp = D3D10_STENCIL_OP_INCR;
	dsDesc.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_KEEP;

	HR(device->CreateDepthStencilState(&dsDesc, &depthStencilState));
}

void ParticleSystem::defineInputElementDesc()
{
	// create the input element descriptions
	nrOfElements = 3;
    layout = new D3D10_INPUT_ELEMENT_DESC[nrOfElements];

	layout[0].SemanticName = "POSITION";
	layout[0].SemanticIndex = 0;
	layout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layout[0].InputSlot = 0;
	layout[0].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
	layout[0].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	layout[0].InstanceDataStepRate = 0;

	layout[1].SemanticName = "SIZE";
	layout[1].SemanticIndex = 0;
	layout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	layout[1].InputSlot = 0;
	layout[1].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
	layout[1].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	layout[1].InstanceDataStepRate = 0;

	layout[2].SemanticName = "COLOR";
	layout[2].SemanticIndex = 0;
	layout[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	layout[2].InputSlot = 0;
	layout[2].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
	layout[2].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	layout[2].InstanceDataStepRate = 0;
}

void ParticleSystem::initVertexBuffer()
{
    D3D10_BUFFER_DESC bd;
    bd.Usage = D3D10_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VertexType)*numVertices;
    bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    bd.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA vertBuffSubRes;
	vertBuffSubRes.pSysMem = &vertices[0];

    HR(device->CreateBuffer(&bd, &vertBuffSubRes, &vertexBuffer));
}

void ParticleSystem::initTextures()
{
		HR(D3DX10CreateShaderResourceViewFromFile(device,
												texFileName.c_str(),
												NULL,
												NULL,
												&texture,
												NULL));
}

void ParticleSystem::initPipeline()
{
    technique = effect->GetTechniqueByIndex(techNr);
    pass = technique->GetPassByIndex(passNr);
    pass->GetDesc(&PassDesc);
}

void ParticleSystem::initInputLayout()
{
	D3D10_PASS_DESC PassDesc;
    technique->GetPassByIndex(0)->GetDesc(&PassDesc);
    HR(
		device->CreateInputLayout(layout, nrOfElements, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &inputLayout));
}

void ParticleSystem::update(float _dt)
{
	//update vertbuffer
	VertexType* memVertices = 0;
	HR(vertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&memVertices));
	ZeroMemory(memVertices, sizeof(VertexType)*numVertices);
	updateVertices(memVertices, _dt);
	vertexBuffer->Unmap();
}

void ParticleSystem::draw()
{
	//set texture
	setTextures();

	//set states
	float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};
	device->OMSetBlendState(blendState, blendFactor, 0xffffffff);
	device->OMSetDepthStencilState(depthStencilState, 1);

    device->IASetInputLayout(inputLayout);
    device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);

    UINT stride = sizeof(VertexType);
    UINT offset = 0;
    device->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

    // apply the appropriate pass
    pass->Apply(passNr);

    // draw to the the back buffer
	device->Draw(numVertices, 0);

	//reset states
	device->OMSetBlendState(0, NULL, 0xffffffff);
	device->OMSetDepthStencilState(0, 0);
}
void ParticleSystem::setTextures()
{
	// set which texture to use
	textureVariable = effect->GetVariableByName("texDefault")->AsShaderResource();
    textureVariable->SetResource(texture);
}

float ParticleSystem::randomFloat()
{
	//returns a float between 0 and 1
	float temp;
	temp = (float)rand() / (float)RAND_MAX;
	return temp;
}

float ParticleSystem::randomFloat(float _min, float _max)
{
	//returns a float between _min and _max
	float temp;
	temp = _min + ((float)rand() / (float)RAND_MAX)*(_max -_min);
	return temp;
}

D3DXVECTOR3 ParticleSystem::randomVector()
{
	D3DXVECTOR3 vector(randomFloat(-1.0f, 1.0f), randomFloat(-1.0f, 1.0f), randomFloat(-1.0f, 1.0f));
	D3DXVec3Normalize(&vector, &vector);
	return vector;
}

D3DXVECTOR3 ParticleSystem::randomVectorPosY()
{
	D3DXVECTOR3 vector(randomFloat(-1.0f, 1.0f), randomFloat(-1.0f, 1.0f), randomFloat(-1.0f, 1.0f));
	vector.y = vector.y*vector.y;
	D3DXVec3Normalize(&vector, &vector);
	return vector;
}

D3DXVECTOR3 ParticleSystem::randomVectorMinMax(float _min, float _max)
{
	D3DXVECTOR3 vector(randomFloat(_min, _max), randomFloat(_min, _max), randomFloat(_min, _max));
	vector.y = vector.y*vector.y;
	D3DXVec3Normalize(&vector, &vector);
	return vector;
}