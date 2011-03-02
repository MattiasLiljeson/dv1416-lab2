#ifndef OBJOBJECT_H
#define OBJOBJECT_H

#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <windows.h>
#include <windowsx.h>
#include <d3d10.h>
#include <d3dx10.h>
#include "Utils.h"

#include "Light.h"
#include "Surface.h"

#include "D3DObject.h"

class ObjObject : public D3DObject
{
private:
	vector<VertexType> vertices;
	vector<DWORD> indices;
	string objFileName;

	vector<D3DXVECTOR3> vertPos;
	vector<D3DXVECTOR2> vertTexUV;
	vector<D3DXVECTOR3> vertNorm;

	void readObjFile();
	void readMtlFile(stringstream& _stream);
	void readVertices(stringstream& _stream);
	void readTextureUV(stringstream& _stream);
	void readNormals(stringstream& _stream);
	void readFaces(stringstream& _stream);

	void defineVertexBuffer();
	void defineIndexBuffer();
	void defineInputElementDesc();

	//import of .obj-files
public:
	ObjObject(ID3D10Device* _device, ID3D10Effect* _effect, int _techNr = 0, int _passNr = 0);
	ObjObject(ID3D10Device* _device, ID3D10Effect* _effect, 
		vector<VertexTexture> _vertices, vector<DWORD> _indices, string _texFileName
		, int _techNr = 0, int _passNr = 0);
	void update(float _dt);
};

#endif