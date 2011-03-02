#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <string>
#include <vector>

#include <windows.h>
#include <windowsx.h>
#include <d3d10.h>
#include <d3dx10.h>

#include "Utils.h"
#include "VertexParticle.h"
#include "Camera.h"

using namespace std;

class ParticleSystem
{
protected:
	typedef VertexParticle VertexType;
	ID3D10Device* device;

	//Blendstate and Depthstate
	ID3D10BlendState* blendState;
	ID3D10DepthStencilState* depthStencilState;

	//Vertices
	ID3D10Buffer* vertexBuffer;
	UINT numVertices;
	vector<VertexType> vertices;

	//Textures
	ID3D10ShaderResourceView* texture;
	ID3D10EffectShaderResourceVariable* textureVariable;
	wstring texFileName;
	
	//FX
	ID3D10Effect* effect;			
	ID3D10EffectTechnique* technique;  
	ID3D10EffectPass* pass;
	D3D10_PASS_DESC PassDesc;	

	//Input layout
	D3D10_INPUT_ELEMENT_DESC* layout;
	UINT nrOfElements;
	ID3D10InputLayout* inputLayout;		

	//Shader settings
	int techNr;
	int passNr;

	//camera specific
	Camera* camera;
	ID3D10EffectVariable* cameraPos;
	ID3D10EffectMatrixVariable* viewProj;

	//functions used only inside the class
	virtual void defineVars() = 0;
	virtual void defineVertices() = 0;
	virtual void updateVertices(VertexType* memVertices,  float _dt) = 0;

	virtual void defineBlendState();
	virtual void defineDepthStencilState();
	virtual void defineInputElementDesc();

	virtual void initVertexBuffer();
	virtual void initTextures();
	virtual void initPipeline();
	virtual void initInputLayout();

	virtual void setTextures();

	//Random functions used inside the class
	float randomFloat();
	float randomFloat(float _min, float _max);
	D3DXVECTOR3 randomVector();
	D3DXVECTOR3 randomVectorPosY();
	D3DXVECTOR3 randomVectorMinMax(float _min, float _max);

public:
	ParticleSystem(ID3D10Device* _device, ID3D10Effect* _effect, Camera* _camera, int _techNr, int _passNr);
	~ParticleSystem();
	virtual void init();
	virtual void update(float _dt);
	virtual void draw();

};

#endif //PARTICLESYSTEM_H