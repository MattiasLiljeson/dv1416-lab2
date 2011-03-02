#include "Triangle.h"
Triangle::Triangle(ID3D10Device* _device, ID3D10Effect* _effect, int _techNr, int _passNr): D3DObject(_device, _effect, _techNr, _passNr)
{
	//Vars for the triangle
	time = 0;
	nrOfVertices = 3;
	nrOfIndices = 3;
}

void Triangle::initVertexBuffer()
{
	// create three vertices using the VERTEX struct built earlier
    VertexSimple OurVertices[] =
    {
        {D3DXVECTOR3(0.0f, 0.5f, 0.0f), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f)},
        {D3DXVECTOR3(0.45f, -0.5f, 0.0f), D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f)},
        {D3DXVECTOR3(-0.45f, -0.5f, 0.0f), D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f)}
    };

	// create the vertex buffer and store the pointer into pBuffer, which is created globally
    D3D10_BUFFER_DESC bd;
    bd.Usage = D3D10_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VertexSimple) * 3;
    bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    bd.MiscFlags = 0;

    device->CreateBuffer(&bd, NULL, &vertexBuffer);

    void* pVoid;    // the void pointer

    vertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, &pVoid);    // map the vertex buffer
    memcpy(pVoid, OurVertices, sizeof(OurVertices));    // copy the vertices to the buffer
    vertexBuffer->Unmap();
}

void Triangle::initIndexBuffer()
{
	// create the index buffer out of DWORDs
    DWORD OurIndices[] =
    {
        0, 1, 2,
    };

    // create the index buffer
	D3D10_BUFFER_DESC bd;
    bd.Usage = D3D10_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(DWORD) * nrOfVertices;
    bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    bd.MiscFlags = 0;

    device->CreateBuffer(&bd, NULL, &indexBuffer);

	void* pVoid;    // the void pointer

    indexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, &pVoid);    // map the index buffer
    memcpy(pVoid, OurIndices, sizeof(OurIndices));    // copy the indices to the buffer
    indexBuffer->Unmap();
}

void Triangle::initInputElementDesc()
{
	// create the input element descriptions
    layout = new D3D10_INPUT_ELEMENT_DESC[2];
	layout[0].SemanticName = "POSITION";
	layout[0].SemanticIndex = 0;
	layout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layout[0].InputSlot = 0;
	layout[0].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
	layout[0].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	layout[0].InstanceDataStepRate = 0;
	
	layout[1].SemanticName = "COLOR";
	layout[1].SemanticIndex = 0;
	layout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	layout[1].InputSlot = 0;
	layout[1].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
	layout[1].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	layout[1].InstanceDataStepRate = 0;
}

void Triangle::update(float _dt)
{
	//For time-roatating effect in shader
	// increase the time varaible and send to the effect
    time += 0.001f;

	// create a rotation matrix
	D3DXMatrixRotationY(&matRotate, time);
}