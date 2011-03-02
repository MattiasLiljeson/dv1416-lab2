#include "D3DObject.h"

D3DObject::D3DObject(ID3D10Device* _device, ID3D10Effect* _effect, int _techNr, int _passNr)
{
	device = _device;
	effect = _effect;
	techNr = _techNr;
	passNr = _passNr;

	D3DXMatrixIdentity(&matRotate);
	D3DXMatrixIdentity(&matScale);
	D3DXMatrixIdentity(&matTranslate);
	D3DXMatrixIdentity(&matWorld);

	resourceFolder = "resources//";
	texFileName = resourceFolder + "Wood.png";

	surface.diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	surface.spec = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
}

D3DObject::~D3DObject()
{
	delete layout;
}

void D3DObject::init()
{
	defineVertexBuffer();
	defineIndexBuffer();
	defineInputElementDesc();

	//initVertexBuffer();
	//initIndexBuffer();
	initTextures();

	initPipeline();
	initFXVars();
}

void D3DObject::initTextures()
{
	wstring wstrTemp(texFileName.length(), L' ');
	copy(texFileName.begin(), texFileName.end(), wstrTemp.begin());

	HR(D3DX10CreateShaderResourceViewFromFile(device,
											wstrTemp.c_str(),
											NULL,
											NULL,
											&texture,
											NULL));
}

void D3DObject::initPipeline()
{
    technique = effect->GetTechniqueByIndex(techNr);
    pass = technique->GetPassByIndex(passNr);
    pass->GetDesc(&PassDesc);

	//layout - input layout description is set in subclass's constructor
    device->CreateInputLayout(layout,
                              nrOfElements,
                              PassDesc.pIAInputSignature,
                              PassDesc.IAInputSignatureSize,
                              &inputLayout);
}

void D3DObject::initFXVars()
{
	world = effect->GetVariableByName("world")->AsMatrix();  // for transformation
	surfaceVariable = effect->GetVariableByName("surface");
}

void D3DObject::draw()
{
	matWorld = matScale * matRotate * matTranslate;

	setTextures();
	setFXVars();

    device->IASetInputLayout(inputLayout);
    device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    UINT stride = sizeof(VertexType);
    UINT offset = 0;
    device->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	device->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // apply the appropriate pass
    pass->Apply(passNr);

    // draw to the the back buffer
	device->DrawIndexed(nrOfIndices, 0, 0);
}

void D3DObject::setTextures()
{
	// set which texture to use
	textureVariable = effect->GetVariableByName("texDefault")->AsShaderResource();
    textureVariable->SetResource(texture);
}

void D3DObject::setFXVars()
{
	world->SetMatrix(&matWorld._11);

	surfaceVariable->SetRawValue(&surface, 0, sizeof(Surface));
}

D3DXVECTOR3 D3DObject::getPos()
{
	D3DXVECTOR3 _pos;
	_pos.x = matWorld._41;
	_pos.y = matWorld._42;
	_pos.z = matWorld._43;
	return _pos;
}