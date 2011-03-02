#ifndef PARTICLE_H
#define PARTICLE_H

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

class Particle
{
private:
	typedef VertexParticle VertexType;
	ID3D10Device* device;

	//Blendstate
	ID3D10BlendState* blendState;

	//Vertices
	ID3D10Buffer* vertexBuffer;
	UINT numVertices;
	vector<VertexType> vertices;

	//Vars
	D3DXVECTOR3 emitterPos;
	D3DXVECTOR2 size;
	vector<bool> active;
	vector<float> age;
	vector<float> speed;
	vector<float> accel;
	vector<D3DXVECTOR3> dir;


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
	virtual void defineBlendState();
	virtual void defineVertexBuffer();
	virtual void defineInputElementDesc();

	virtual void initTextures();
	virtual void initPipeline();
	virtual void initInputLayout();

	virtual void setTextures();

	//Random functions used inside the class
	float randomFloat();
	float randomFloat(float _min, float _max);
	D3DXVECTOR3 randomVector();
	D3DXVECTOR3 randomVectorPosY();

public:
	Particle(ID3D10Device* _device, ID3D10Effect* _effect, Camera* _camera, int _techNr, int _passNr);
	~Particle();
	virtual void init();
	virtual void update(float _dt);
	virtual void draw();

};

#endif //PARTICLE_H