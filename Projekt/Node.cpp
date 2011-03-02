#include "Node.h"
Node::Node(D3DXVECTOR3 _vecBoxMin, D3DXVECTOR3 _vecBoxMax, int _numLevels)
{
	vecBoxMin = _vecBoxMin;
	vecBoxMax = _vecBoxMax;
	
	//subDivide(_numLevels);
}

Node::Node(D3DXVECTOR3 _vecBoxMin, D3DXVECTOR3 _vecBoxMax, int _numLevels,
	ID3D10Device* _device, ID3D10Effect* _effect,  int _techNr, int _passNr)
{
	device = _device;
	effect = _effect;
	techNr = _techNr;
	passNr = _passNr;

	vecBoxMin = _vecBoxMin;
	vecBoxMax = _vecBoxMax;
	
	for(int i=0; i<4; i++)
	{
		children[i] = NULL;
	}

	subDivide(_numLevels);
}

void Node::subDivide(int _numLevels)
{
	if(children[0] == NULL)
	{
		_numLevels--;
		if(_numLevels >= 0)
		{
			// quadtree children layout:
			// 
			//             *---*---*<- vecBoxMax    ^ z
			//             | 0 | 1 |                |
			//             *---*---*                |
			//             | 2 | 3 |                |
			// vecBoxMin ->*---*---*              y o------> x

			D3DXVECTOR3 _min, _max;

			//cell 0
			_min.x = vecBoxMin.x;
			_min.y = vecBoxMin.y;
			_min.z = (vecBoxMin.z + vecBoxMax.z)/2.0f;
			
			_max.x = (vecBoxMin.x + vecBoxMax.x)/2.0f;
			_max.y = vecBoxMax.y;
			_max.z = vecBoxMax.z;

			children[0] = new Node(_min, _max, _numLevels, device, effect, techNr, passNr);

			//cell 1
			_min.x = (vecBoxMin.x + vecBoxMax.x)/2.0f;
			_min.y = vecBoxMin.y;
			_min.z = (vecBoxMin.z + vecBoxMax.z)/2.0f;
			
			_max   = vecBoxMax;

			children[1] = new Node(_min, _max, _numLevels, device, effect, techNr, passNr);

			//cell 2
			_min   = vecBoxMin;

			_max.x = (vecBoxMin.x + vecBoxMax.x)/2.0f;
			_max.y = vecBoxMax.y;
			_max.z = (vecBoxMin.z + vecBoxMax.z)/2.0f;

			children[2] = new Node(_min, _max, _numLevels, device, effect, techNr, passNr);

			//cell 3
			_min.x = (vecBoxMin.x + vecBoxMax.x)/2.0f;
			_min.y = vecBoxMin.y;
			_min.z = vecBoxMin.z;

			_max.x = vecBoxMax.x;
			_max.y = vecBoxMax.y;
			_max.z = (vecBoxMin.z + vecBoxMax.z)/2.0f;

			children[3] = new Node(_min, _max, _numLevels, device, effect, techNr, passNr);

			for(int i=0; i<4; i++)
			{
				children[i]->initDrawStuff();
			}
		}
	}
	else //else throw exception
	{
		int korv = 0; //children already exists
	}
}

void Node::insertObject(D3DObject* _object)
{
	D3DXVECTOR3 pos;
	pos = _object->getPos();

	if(children[0] != NULL)
	{
		if(pos.z < children[0]->vecBoxMax.z)
		{
			if(pos.x < children[0]->vecBoxMax.x)
				children[0]->insertObject(_object);
			else
				children[1]->insertObject(_object);
		}
		else
		{
			if(pos.x < children[0]->vecBoxMax.x)
				children[2]->insertObject(_object);
			else
				children[3]->insertObject(_object);
		}
	}
	else
	{
		objects.push_back(_object);
	}
}

void Node::initDrawStuff()
{
	D3DXCOLOR	randColor;
	D3DXVECTOR3 randOffset;

	randColor.r = fmod(rand(),1.01f);
	randColor.g = fmod(rand(),1.01f);
	randColor.b = fmod(rand(),1.01f);
	randColor.a = 1.0f;

	randOffset.x = fmod(rand(),1.01f);
	randOffset.y = fmod(rand(),1.01f);
	randOffset.z = fmod(rand(),1.01f);

	numVertices = 24;
	VertexSimple vertices[24] =
	{
		{D3DXVECTOR3(vecBoxMin.x, vecBoxMin.y, vecBoxMin.z) + randOffset, randColor},
		{D3DXVECTOR3(vecBoxMax.x, vecBoxMin.y, vecBoxMin.z) + randOffset, randColor},
		{D3DXVECTOR3(vecBoxMin.x, vecBoxMin.y, vecBoxMin.z) + randOffset, randColor},
		{D3DXVECTOR3(vecBoxMin.x, vecBoxMax.y, vecBoxMin.z) + randOffset, randColor},
		{D3DXVECTOR3(vecBoxMin.x, vecBoxMax.y, vecBoxMin.z) + randOffset, randColor},
		{D3DXVECTOR3(vecBoxMax.x, vecBoxMax.y, vecBoxMin.z) + randOffset, randColor},
		{D3DXVECTOR3(vecBoxMax.x, vecBoxMin.y, vecBoxMin.z) + randOffset, randColor},
		{D3DXVECTOR3(vecBoxMax.x, vecBoxMax.y, vecBoxMin.z) + randOffset, randColor},

		//back
		{D3DXVECTOR3(vecBoxMin.x, vecBoxMin.y, vecBoxMax.z) + randOffset, randColor},
		{D3DXVECTOR3(vecBoxMax.x, vecBoxMin.y, vecBoxMax.z) + randOffset, randColor},
		{D3DXVECTOR3(vecBoxMin.x, vecBoxMin.y, vecBoxMax.z) + randOffset, randColor},
		{D3DXVECTOR3(vecBoxMin.x, vecBoxMax.y, vecBoxMax.z) + randOffset, randColor},
		{D3DXVECTOR3(vecBoxMin.x, vecBoxMax.y, vecBoxMax.z) + randOffset, randColor},
		{D3DXVECTOR3(vecBoxMax.x, vecBoxMax.y, vecBoxMax.z) + randOffset, randColor},
		{D3DXVECTOR3(vecBoxMax.x, vecBoxMin.y, vecBoxMax.z) + randOffset, randColor},
		{D3DXVECTOR3(vecBoxMax.x, vecBoxMax.y, vecBoxMax.z) + randOffset, randColor},

		//sides
		{D3DXVECTOR3(vecBoxMin.x, vecBoxMin.y, vecBoxMin.z) + randOffset, randColor},
		{D3DXVECTOR3(vecBoxMin.x, vecBoxMin.y, vecBoxMax.z) + randOffset, randColor},
		{D3DXVECTOR3(vecBoxMax.x, vecBoxMin.y, vecBoxMin.z) + randOffset, randColor},
		{D3DXVECTOR3(vecBoxMax.x, vecBoxMin.y, vecBoxMax.z) + randOffset, randColor},
		{D3DXVECTOR3(vecBoxMin.x, vecBoxMax.y, vecBoxMin.z) + randOffset, randColor},
		{D3DXVECTOR3(vecBoxMin.x, vecBoxMax.y, vecBoxMax.z) + randOffset, randColor},
		{D3DXVECTOR3(vecBoxMax.x, vecBoxMax.y, vecBoxMin.z) + randOffset, randColor},
		{D3DXVECTOR3(vecBoxMax.x, vecBoxMax.y, vecBoxMax.z) + randOffset, randColor},
	};

	// create the vertex buffer and store the pointer into pBuffer, which is created globally
    D3D10_BUFFER_DESC bd;
    bd.Usage = D3D10_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(vertices);
    bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    bd.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA vertBuffSubRes;
	vertBuffSubRes.pSysMem = &vertices[0];

    device->CreateBuffer(&bd, &vertBuffSubRes, &vertexBuffer);

	numIndices = 24;
	DWORD indices[24] =
	{
		//plane
		0,1,
		1,2,
		2,3,
		3,0,

		//plane
		4,5,
		5,6,
		6,7,
		7,4,

		//connecting planes
		0,4,
		1,5,
		2,6,
		3,7
	};

	// create the index buffer
	D3D10_BUFFER_DESC ibd;
    ibd.Usage = D3D10_USAGE_DYNAMIC;
    ibd.ByteWidth = sizeof(indices);
    ibd.BindFlags = D3D10_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    ibd.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA indBuffSubRes;
    indBuffSubRes.pSysMem = &indices[0];

    HR(device->CreateBuffer(&ibd, &indBuffSubRes, &indexBuffer));

	// create the input element descriptions
	numElements = 2;
    D3D10_INPUT_ELEMENT_DESC* layout = new D3D10_INPUT_ELEMENT_DESC[numElements];

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

	technique = effect->GetTechniqueByIndex(techNr);
    pass = technique->GetPassByIndex(passNr);
    pass->GetDesc(&PassDesc);

	//layout - input layout description is set in subclass's constructor
    device->CreateInputLayout(layout, numElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &inputLayout);

	fxVar_world = effect->GetVariableByName("world")->AsMatrix();  // for transformation
	D3DXMatrixIdentity(&mat_world);
	mat_world._42 = 10.0f;
	mat_world._41 = 13.0f;
}

void Node::draw()
{
	fxVar_world->SetMatrix(&mat_world._11);

	device->IASetInputLayout(inputLayout);
    device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);

    UINT stride = sizeof(VertexSimple);
    UINT offset = 0;
    device->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	device->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // apply the appropriate pass
    pass->Apply(passNr);

    // draw to the the back buffer
	device->Draw(numVertices, 0);

	if(children[0] != NULL)
	{
		for(int i=0; i<4; i++)
			children[i]->draw();
	}
}