#ifndef D3DOBJECT_H
#define D3DOBJECT_H

#include <string>
#include <windows.h>
#include <windowsx.h>
#include <d3d10.h>
#include <d3dx10.h>
#include "Utils.h"

#include "cBuffers.h"

#include "Surface.h"

#include "VertexSimple.h"
#include "VertexSimple2.h"
#include "VertexTexture.h"
#include "VertexBlendmap.h"

using namespace std;

class D3DObject
{
protected:
	ID3D10Device* device;
	string resourceFolder;

	//tech and FX
	ID3D10Effect* effect;
	ID3D10EffectTechnique* technique;
	ID3D10EffectPass* pass;	
	D3D10_PASS_DESC PassDesc;

	//vertices and indices
	typedef VertexTexture VertexType;	//change this per impl
	ID3D10Buffer* vertexBuffer;
	ID3D10Buffer* indexBuffer;
	int nrOfVertices;
	int nrOfIndices;

	//textures
	ID3D10ShaderResourceView* texture;
	ID3D10EffectShaderResourceVariable* textureVariable;
	string texFileName;

	//surface info
	Surface surface;
	ID3D10EffectVariable* surfaceVariable;

	//input layout
	D3D10_INPUT_ELEMENT_DESC* layout;
	UINT nrOfElements;
	ID3D10InputLayout* inputLayout;	
	
	//Cbuff, not used
	ID3D10Buffer *cBuffer;

	//matrices for transformations
	ID3D10EffectMatrixVariable* world;
	D3DXMATRIX matScale;
	D3DXMATRIX matRotate;
	D3DXMATRIX matTranslate;
	D3DXMATRIX matWorld;

	//Shader settings
	int techNr;
	int passNr;

	//functions used only inside class
	virtual void defineVertexBuffer() = 0;
	virtual void defineIndexBuffer() = 0;
	virtual void defineInputElementDesc() = 0;

	virtual void initTextures();
	virtual void initPipeline();
	virtual void initFXVars();

	virtual void setTextures();
	virtual void setFXVars();

	//virtual void initCBuffers();
	//virtual void initVertexBuffer();
	//virtual void initIndexBuffer();

public:
	D3DObject(ID3D10Device* _device, ID3D10Effect* _effect, int _techNr = 0, int _passNr = 0);
	~D3DObject();
	virtual void init();
	virtual void update(float _dt) = 0;
	virtual void draw();
	virtual D3DXVECTOR3 getPos();
};

#endif //D3DOBJECT_H