#ifndef GROUP_H
#define GROUP_H

#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include <windows.h>
#include <windowsx.h>
#include <d3d10.h>
#include <d3dx10.h>
#include "Utils.h"

#include "ObjObject.h"

using namespace std;

class Group
{
private:
	typedef VertexTexture VertexType;	//change this per impl
	vector<ObjObject*> objects;
	string resourceFolder;
	string objFileName;

	//matrices
	ID3D10EffectMatrixVariable* world;
	D3DXMATRIX matScale;
	D3DXMATRIX matRotate;
	D3DXMATRIX matTranslate;
	D3DXMATRIX matWorld;

	//Object pass through vars
	ID3D10Device* device;
	ID3D10EffectTechnique* technique;
	ID3D10Effect* effect;
	int techNr;
	int passNr;

	//File parsing
	int nrOfVertices;
	int nrOfIndices;
	string texFileName;
	vector<VertexType> vertices;
	vector<DWORD> indices;
	vector<D3DXVECTOR3> vertPos;
	vector<D3DXVECTOR2> vertTexUV;
	vector<D3DXVECTOR3> vertNorm;

	void readObjFile();
	void readMtlFile(stringstream& _stream);
	void readVertices(stringstream& _stream);
	void readTextureUV(stringstream& _stream);
	void readNormals(stringstream& _stream);
	void readFaces(stringstream& _stream);
	void createObject();

public:
	Group(ID3D10Device* _device, ID3D10Effect* _effect, string _objFileName, int _techNr = 0, int _passNr = 0);
	~Group();
	void init();
	void update(float _dt);
	void draw();
};

#endif