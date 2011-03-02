#include "ObjObject.h"
ObjObject::ObjObject(ID3D10Device* _device, ID3D10Effect* _effect, int _techNr, int _passNr)
	: D3DObject(_device, _effect, _techNr, _passNr)
{
	//Vars
	nrOfVertices = -1;
	nrOfIndices = -1;
	objFileName = "bth.obj";

	//just for the lulz
	D3DXMatrixTranslation(&matTranslate, 15.0f, 20.0f, 0.0f);
	D3DXMatrixScaling(&matScale, 1/3.0f, 1/3.0f, 1/3.0f);

	readObjFile();
}

ObjObject::ObjObject(ID3D10Device* _device, ID3D10Effect* _effect, 
		vector<VertexTexture> _vertices, vector<DWORD> _indices, string _texFileName
		, int _techNr, int _passNr)
		: D3DObject(_device, _effect, _techNr, _passNr)
{
	nrOfVertices = _vertices.size();
	nrOfIndices = _indices.size();
	vertices = _vertices;
	indices = _indices;
	texFileName = _texFileName;


	//just for the lulz
	D3DXMatrixTranslation(&matTranslate, 15.0f, 20.0f, 0.0f);
	D3DXMatrixScaling(&matScale, 1/3.0f, 1/3.0f, 1/3.0f);
}

void ObjObject::readObjFile()
{
	//Read obj-file;
	fstream objFile(resourceFolder + objFileName);

	if(objFile)
	{
		string line;
		string prefix;
		while(objFile.eof() == false)
		{
			//prefix = "NULL"; //leave nothing from the previous iteration
			stringstream lineStream;

			getline(objFile, line);
			lineStream << line;
			lineStream >> prefix;

			if(prefix == "mtllib")
				readMtlFile(lineStream);
			else if(prefix == "v")
				readVertices(lineStream);

			else if(prefix == "vt")
				readTextureUV(lineStream);

			else if (prefix == "vn")
				readNormals(lineStream);

			else if(prefix == "f")
				readFaces(lineStream);
		}
	}
}

void ObjObject::readMtlFile(stringstream& _stream)
{
	//Read .mtl-file;
	string mtlFileName;
	_stream >> mtlFileName;

	string line;
	string prefix;
	fstream mtlFile(resourceFolder + mtlFileName);

	while(mtlFile.eof() == false)
	{
		stringstream lineStreamMtl;
		getline(mtlFile, line);
		lineStreamMtl << line;
		lineStreamMtl >> prefix;

		if(prefix == "map_Kd")
		{
			string temp;
			lineStreamMtl >> temp;
			texFileName = resourceFolder + temp;
		}
	}
}

void ObjObject::readVertices(stringstream& _stream)
{
	D3DXVECTOR3 pos;
	_stream >> pos.x >> pos.y >> pos.z;
	vertPos.push_back(pos);
}

void ObjObject::readTextureUV(stringstream& _stream)
{
	D3DXVECTOR2 uv;
	_stream >> uv.x >> uv.y;
	vertTexUV.push_back(uv);
}

void ObjObject::readNormals(stringstream& _stream)
{
	D3DXVECTOR3 normal;
	_stream >> normal.x >> normal.y >> normal.z;
	vertNorm.push_back(normal);
}


void ObjObject::readFaces(stringstream& _stream)
{
	VertexType tempVertex;
	char tmp;

	//init as 1 to ptotect from empty string stream. (0 as effective index)
	int indexPos = 1;
	int texPos = 1;
	int normPos = 1;

	for(int i=0; i<3; i++)
	{
		ZeroMemory(&tempVertex, sizeof(VertexType));
		nrOfVertices++;
		nrOfIndices++;

		_stream >> indexPos >> tmp >> texPos >> tmp >> normPos;

		tempVertex.Position = vertPos[indexPos-1];
		tempVertex.texCord =  vertTexUV[texPos-1];
		tempVertex.Normal = vertNorm[normPos-1];

		vertices.push_back(tempVertex);
		indices.push_back(nrOfIndices);
	}
}
void ObjObject::defineVertexBuffer()
{
	// create vertices to represent the corners of the Cube


	// create the vertex buffer and store the pointer into pBuffer, which is created globally
    D3D10_BUFFER_DESC bd;
    bd.Usage = D3D10_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VertexType)*nrOfVertices;
    bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    bd.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA vertBuffSubRes;
	vertBuffSubRes.pSysMem = &vertices[0];

    HR(device->CreateBuffer(&bd, &vertBuffSubRes, &vertexBuffer));
}

void ObjObject::defineIndexBuffer()
{
	// create the index buffer out of DWORDs

    // create the index buffer
	D3D10_BUFFER_DESC bd;
    bd.Usage = D3D10_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(DWORD)*nrOfIndices;
    bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    bd.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA indBuffSubRes;
    indBuffSubRes.pSysMem = &indices[0];

    HR(device->CreateBuffer(&bd, &indBuffSubRes, &indexBuffer));
}

void ObjObject::defineInputElementDesc()
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

void ObjObject::update(float _dt)
{
	D3DXMATRIX matRotYTemp;
	D3DXMatrixRotationY(&matRotYTemp, _dt);
	matRotate *= matRotYTemp;

	//lightVariable = effect->GetVariableByName("light");
	//surfaceVariable = effect->GetVariableByName("surface");
	//lightVariable->SetRawValue(&light, 0, sizeof(Light));
	//surfaceVariable->SetRawValue(&surface, 0, sizeof(Surface));
}