#ifndef NODE_H
#define NODE_H

#include <d3d10.h>
#include <d3dx10.h>
#include <windows.h>
#include <windowsx.h>

#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "D3DObject.h"
#include "VertexSimple.h"
#include "Utils.h"

using namespace std;

class Node
{
public:
	D3DXVECTOR3 vecBoxMin;
	D3DXVECTOR3 vecBoxMax;
	vector<D3DObject*> objects;
	Node* children[4];

	Node(D3DXVECTOR3 _vecBoxMin, D3DXVECTOR3 _vecBoxMax, int _numLevels);
	Node(D3DXVECTOR3 _vecBoxMin, D3DXVECTOR3 _vecBoxMax, int _numLevels,
		ID3D10Device* _device, ID3D10Effect* _effect, int _techNr = 0, int _passNr = 0);

	void subDivide(int _numLevels);
	void insertObject(D3DObject* _object);

	//debug, draw a cube with lines showing location of node
	ID3D10Device* device;

	//tech and FX
	ID3D10Effect* effect;
	ID3D10EffectTechnique* technique;
	ID3D10EffectPass* pass;	
	D3D10_PASS_DESC PassDesc;
	int techNr;
	int passNr;
	ID3D10EffectMatrixVariable* fxVar_world;
	D3DXMATRIX mat_world;

	UINT numElements;
	ID3D10InputLayout* inputLayout;	

	//vertices and indices
	ID3D10Buffer* vertexBuffer;
	ID3D10Buffer* indexBuffer;
	int numVertices;
	int numIndices;

	void initDrawStuff();
	void draw();
};

#endif