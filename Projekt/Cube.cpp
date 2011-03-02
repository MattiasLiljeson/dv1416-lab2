#include "Cube.h"
Cube::Cube(ID3D10Device* _device, ID3D10Effect* _effect, int _techNr, int _passNr)
	: D3DObject(_device, _effect, _techNr, _passNr)
{
	//Vars for the Cube
	time = 0;
	nrOfVertices = 24;
	nrOfIndices = 36;

	//just for the lulz
	matScale._44 = 0.5f;
	matTranslate._42 = 10.f;


	texFileName = "resources/cube.png";
}

void Cube::defineVertexBuffer()
{
	// create vertices to represent the corners of the Cube
    VertexType OurVertices[] =
	{
        {D3DXVECTOR3(-1.0f, -1.0f, 1.0f),  D3DXVECTOR3(0.0f, 0.0f, 1.0f),  D3DXVECTOR2(0.0f, 0.0f)},
        {D3DXVECTOR3(1.0f, -1.0f, 1.0f),   D3DXVECTOR3(0.0f, 0.0f, 1.0f),  D3DXVECTOR2(0.0f, 1.0f)},
        {D3DXVECTOR3(-1.0f, 1.0f, 1.0f),   D3DXVECTOR3(0.0f, 0.0f, 1.0f),  D3DXVECTOR2(1.0f, 0.0f)},
        {D3DXVECTOR3(1.0f, 1.0f, 1.0f),    D3DXVECTOR3(0.0f, 0.0f, 1.0f),  D3DXVECTOR2(1.0f, 1.0f)},
																		   
        {D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR2(0.0f, 0.0f)},
        {D3DXVECTOR3(-1.0f, 1.0f, -1.0f),  D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR2(0.0f, 1.0f)},
        {D3DXVECTOR3(1.0f, -1.0f, -1.0f),  D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR2(1.0f, 0.0f)},
        {D3DXVECTOR3(1.0f, 1.0f, -1.0f),   D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR2(1.0f, 1.0f)},
																		  
        {D3DXVECTOR3(-1.0f, 1.0f, -1.0f),  D3DXVECTOR3(0.0f, 1.0f, 0.0f),  D3DXVECTOR2(0.0f, 0.0f)},
        {D3DXVECTOR3(-1.0f, 1.0f, 1.0f),   D3DXVECTOR3(0.0f, 1.0f, 0.0f),  D3DXVECTOR2(0.0f, 1.0f)},
        {D3DXVECTOR3(1.0f, 1.0f, -1.0f),   D3DXVECTOR3(0.0f, 1.0f, 0.0f),  D3DXVECTOR2(1.0f, 0.0f)},
        {D3DXVECTOR3(1.0f, 1.0f, 1.0f),    D3DXVECTOR3(0.0f, 1.0f, 0.0f),  D3DXVECTOR2(1.0f, 1.0f)},
																		   
        {D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f)},
        {D3DXVECTOR3(1.0f, -1.0f, -1.0f),  D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR2(0.0f, 1.0f)},
        {D3DXVECTOR3(-1.0f, -1.0f, 1.0f),  D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR2(1.0f, 0.0f)},
        {D3DXVECTOR3(1.0f, -1.0f, 1.0f),   D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR2(1.0f, 1.0f)},
																		   
        {D3DXVECTOR3(1.0f, -1.0f, -1.0f),  D3DXVECTOR3(1.0f, 0.0f, 0.0f),  D3DXVECTOR2(0.0f, 0.0f)},
        {D3DXVECTOR3(1.0f, 1.0f, -1.0f),   D3DXVECTOR3(1.0f, 0.0f, 0.0f),  D3DXVECTOR2(0.0f, 1.0f)},
        {D3DXVECTOR3(1.0f, -1.0f, 1.0f),   D3DXVECTOR3(1.0f, 0.0f, 0.0f),  D3DXVECTOR2(1.0f, 0.0f)},
        {D3DXVECTOR3(1.0f, 1.0f, 1.0f),    D3DXVECTOR3(1.0f, 0.0f, 0.0f),  D3DXVECTOR2(1.0f, 1.0f)},
																		  
        {D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f)},
        {D3DXVECTOR3(-1.0f, -1.0f, 1.0f),  D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 1.0f)},
        {D3DXVECTOR3(-1.0f, 1.0f, -1.0f),  D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f, 0.0f)},
        {D3DXVECTOR3(-1.0f, 1.0f, 1.0f),   D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f, 1.0f)}
    };

	// create the vertex buffer and store the pointer into pBuffer, which is created globally
    D3D10_BUFFER_DESC bd;
    bd.Usage = D3D10_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(OurVertices);
    bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    bd.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA vertBuffSubRes;
	vertBuffSubRes.pSysMem = &OurVertices[0];

    HR(device->CreateBuffer(&bd, &vertBuffSubRes, &vertexBuffer));
}

void Cube::defineIndexBuffer()
{
	// create the index buffer out of DWORDs
    DWORD OurIndices[] =
    {
        0, 1, 2,    // side 1
        2, 1, 3,
        4, 5, 6,    // side 2
        6, 5, 7,
        8, 9, 10,    // side 3
        10, 9, 11,
        12, 13, 14,    // side 4
        14, 13, 15,
        16, 17, 18,    // side 5
        18, 17, 19,
        20, 21, 22,    // side 6
        22, 21, 23,
    };

    // create the index buffer
	D3D10_BUFFER_DESC bd;
    bd.Usage = D3D10_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(OurIndices);
    bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    bd.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA indBuffSubRes;
    indBuffSubRes.pSysMem = &OurIndices[0];

    HR(device->CreateBuffer(&bd, &indBuffSubRes, &indexBuffer));
}

void Cube::defineInputElementDesc()
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

	//use normal instead of color
	layout[1].SemanticName = "NORMAL";
	layout[1].SemanticIndex = 0;
	layout[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layout[1].InputSlot = 0;
	layout[1].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
	layout[1].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	layout[1].InstanceDataStepRate = 0;

	//texture
	layout[2].SemanticName = "TEXCOORD";
	layout[2].SemanticIndex = 0;
	layout[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	layout[2].InputSlot = 0;
	layout[2].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
	layout[2].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	layout[2].InstanceDataStepRate = 0;
}

void Cube::update(float _dt)
{
	//For time-rotating effect in shader
	// increase the time varaible and send to the effect
    time += 0.0001f;

	//rotate the cube according to time
	D3DXMatrixRotationY(&matRotate, time);
}