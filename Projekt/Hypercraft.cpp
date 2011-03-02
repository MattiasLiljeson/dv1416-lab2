#include "Hypercraft.h"
Hypercraft::Hypercraft(ID3D10Device* _device, ID3D10Effect* _effect, int _techNr, int _passNr): D3DObject(_device, _effect, _techNr, _passNr)
{
	//Vars for the Hypercraft
	time = 0;
	nrOfVertices = 10;
	nrOfIndices = 24;
	D3DXMatrixScaling(&matScale, 0.5, 0.5, 0.5);
	D3DXMatrixTranslation(&matTranslate, 0, 0, 0);
	
	texFileName = "resources/cube.png";
}

void Hypercraft::defineVertexBuffer()
{
	// create three vertices using the VERTEX struct built earlier
	VertexTexture OurVertices[] =
    {
        // fuselage
        {D3DXVECTOR3(3.0f, 0.0f, 0.0f), D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f),  D3DXVECTOR2(0.0f, 0.0f)},
        {D3DXVECTOR3(0.0f, 3.0f, -3.0f), D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f)},
        {D3DXVECTOR3(0.0f, 0.0f, 10.0f), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f)},
        {D3DXVECTOR3(-3.0f, 0.0f, 0.0f), D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f)},

        // left gun
        {D3DXVECTOR3(3.2f, -1.0f, -3.0f), D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f)}, 
        {D3DXVECTOR3(3.2f, -1.0f, 11.0f), D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f)}, 
        {D3DXVECTOR3(2.0f, 1.0f, 2.0f), D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.0f),   D3DXVECTOR2(0.0f, 1.0f)}, 

        // right gun
        {D3DXVECTOR3(-3.2f, -1.0f, -3.0f), D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f)}, 
        {D3DXVECTOR3(-3.2f, -1.0f, 11.0f), D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f)}, 
        {D3DXVECTOR3(-2.0f, 1.0f, 2.0f), D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.0f),   D3DXVECTOR2(0.0f, 1.0f)} 
    };

	// create the vertex buffer and store the pointer into pBuffer, which is created globally
    D3D10_BUFFER_DESC bd;
    bd.Usage = D3D10_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VertexTexture) * nrOfVertices;
    bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    bd.MiscFlags = 0;

    device->CreateBuffer(&bd, NULL, &vertexBuffer);

    void* pVoid;    // the void pointer

    vertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, &pVoid);    // map the vertex buffer
    memcpy(pVoid, OurVertices, sizeof(OurVertices));    // copy the vertices to the buffer
    vertexBuffer->Unmap();
}

void Hypercraft::defineIndexBuffer()
{
	// create the index buffer out of DWORDs
    DWORD OurIndices[] =
    {
        0, 1, 2,    // fuselage
        2, 1, 3,
        3, 1, 0,
        0, 2, 3,
        4, 5, 6,    // wings
        7, 8, 9,
        4, 6, 5,    // wings (back face)
        7, 9, 8,
    };

    // create the index buffer
	D3D10_BUFFER_DESC bd;
    bd.Usage = D3D10_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(DWORD) * nrOfIndices;
    bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    bd.MiscFlags = 0;

    device->CreateBuffer(&bd, NULL, &indexBuffer);

	void* pVoid;    // the void pointer

    indexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, &pVoid);    // map the index buffer
    memcpy(pVoid, OurIndices, sizeof(OurIndices));    // copy the indices to the buffer
    indexBuffer->Unmap();
}

void Hypercraft::defineInputElementDesc()
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

	layout[2].SemanticName = "TEXCOORD";
	layout[2].SemanticIndex = 0;
	layout[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	layout[2].InputSlot = 0;
	layout[2].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
	layout[2].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	layout[2].InstanceDataStepRate = 0;
}

void Hypercraft::update(float _dt)
{
	//For time-rotating effect in shader
	// increase the time varaible and send to the effect
    time += 0.0001f;

	// create a rotation matrix
	D3DXMatrixRotationY(&matRotate, time);
	matRotate._42 = 5.0f;
	matRotate._43 = 20.0f;
	//D3DXMatrixTranslation(&translate, 0, 0, -time);
}