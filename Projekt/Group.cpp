#include "Group.h"

Group::Group(ID3D10Device* _device, ID3D10Effect* _effect, string _objFileName, int _techNr, int _passNr)
{
	device = _device;
	effect = _effect;
	techNr = _techNr;
	passNr = _passNr;

	nrOfVertices = 0;
	nrOfIndices = 0;

	D3DXMatrixIdentity(&matRotate);
	D3DXMatrixIdentity(&matScale);
	D3DXMatrixIdentity(&matTranslate);
	D3DXMatrixIdentity(&matWorld);

	D3DXMatrixTranslation(&matTranslate, 25.0f, 0.0f, 25.0f);

	resourceFolder = "resources//";
	objFileName = _objFileName;
	readObjFile();
}

Group::~Group()
{
	//for(UINT i=0; i<objects.size(); i++)
	//	delete objects[i];
}

void Group::readObjFile()
{
	//Read obj-file;
	fstream objFile(resourceFolder + objFileName);

	if(objFile)
	{
		string line;
		string prefix;

		int count = 0;
		while(objFile.eof() == false)
		{
			count++;
			if(count = 24824)
				string hejhopp = "korv";

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

			else if(prefix == "g")
			{
				createObject();
				int korv = 0;
			}
		}
		createObject();
	}
}

void Group::readMtlFile(stringstream& _stream)
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

void Group::readVertices(stringstream& _stream)
{
	D3DXVECTOR3 pos;
	_stream >> pos.x >> pos.y >> pos.z;
	vertPos.push_back(pos);
}

void Group::readTextureUV(stringstream& _stream)
{
	D3DXVECTOR2 uv;
	_stream >> uv.x >> uv.y;
	vertTexUV.push_back(uv);
}

void Group::readNormals(stringstream& _stream)
{
	D3DXVECTOR3 normal;
	_stream >> normal.x >> normal.y >> normal.z;
	vertNorm.push_back(normal);
}


void Group::readFaces(stringstream& _stream)
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

		_stream >> indexPos >> tmp >> texPos >> tmp >> normPos;

		tempVertex.Position = vertPos[indexPos-1];
		tempVertex.texCord =  vertTexUV[texPos-1];
		tempVertex.Normal = vertNorm[normPos-1];

		vertices.push_back(tempVertex);
		indices.push_back(nrOfIndices);

		nrOfVertices++;
		nrOfIndices++;
	}
}

void Group::createObject()
{
	if(vertices.size() > 0)
	{
		objects.push_back(new ObjObject(device, effect, vertices, indices, texFileName, techNr, passNr));
	
		vertices.clear();
		indices.clear();
		nrOfVertices = 0;
		nrOfIndices = 0;
	}
}

void Group::init()
{
	//init group transformation matrix effect variable
	technique = effect->GetTechniqueByIndex(techNr);
	world = effect->GetVariableByName("worldGroup")->AsMatrix();  // for transformation

	//init objects
	for(UINT i=0; i<objects.size(); i++)
		objects[i]->init();
}

void Group::update(float _dt)
{
	//update objects
	for(UINT i=0; i<objects.size(); i++)
		objects[i]->update(_dt);
}

void Group::draw()
{
	//update world matrix
	matWorld = matScale * matRotate * matTranslate;
	world->SetMatrix(&matWorld._11);

	//draw objects
	for(UINT i=0; i<objects.size(); i++)
		objects[i]->draw();

	//HACK!!!
	//reset worldGroup matrix to identity in shader so non-grouped objects won't get transformed with the latest rendered group matrix.
	D3DXMATRIX temp;
	D3DXMatrixIdentity(&temp);
	world->SetMatrix(&temp._11);
}