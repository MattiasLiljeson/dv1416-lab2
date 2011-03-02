#include "WaterFountain.h"

WaterFountain::WaterFountain(ID3D10Device* _device, ID3D10Effect* _effect, Camera* _camera, int _techNr, int _passNr):
	ParticleSystem(_device, _effect, _camera, _techNr, _passNr)
{
}

void WaterFountain::defineVars()
{
	numVertices = 1000;
	emitterPos.x = 0.0f;
	emitterPos.y = 3.0f;
	emitterPos.z = 15.0f;
	size.x = 0.01f;
	size.y = 0.01f;

	lifeTime = 2.0f;
	texFileName = L"resources/waterDrop3.png";
}

void WaterFountain::defineVertices()
{
	//create vertex to represent the corners of the Cube
	for(UINT i = 0; i < numVertices; i++)
	{
		vertices.push_back(VertexType());
		vertices[i].position = emitterPos;
		vertices[i].size = size;
		vertices[i].fade = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		active.push_back(false);

		age.push_back(0);
		speed.push_back(randomVectorPosY());
		speed[i].y *= 5;
		accel = D3DXVECTOR3(0.0f, -9.82f, 0.0f);
	}
}

void WaterFountain::defineBlendState()
{
	D3D10_BLEND_DESC blendDesc = {0};
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.BlendEnable[0] = true;
	blendDesc.SrcBlend       = D3D10_BLEND_ONE;
	blendDesc.DestBlend      = D3D10_BLEND_ONE;
	blendDesc.BlendOp        = D3D10_BLEND_OP_MAX;
	blendDesc.SrcBlendAlpha  = D3D10_BLEND_ONE;
	blendDesc.DestBlendAlpha = D3D10_BLEND_ZERO;
	blendDesc.BlendOpAlpha   = D3D10_BLEND_OP_ADD;
	blendDesc.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;

	HR(device->CreateBlendState(&blendDesc, &blendState));
}

void WaterFountain::updateVertices(VertexType* memVertices, float _dt)
{
	
	bool activated = false;
	for(UINT i=0; i<numVertices; i++)
	{
		if(age[i] > lifeTime)
		{
			active[i] = false;
			age[i] = 0.0f;
			speed[i] = randomVectorPosY();
			speed[i].y *= 5;
			vertices[i].position = emitterPos;

			memVertices[i].position = emitterPos;
		}

		if(active[i])
		{
			vertices[i].position += speed[i]*_dt;

			speed[i].y += accel.y*_dt;
			age[i] += _dt;

			memVertices[i].fade = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);//D3DXVECTOR4(age[i]/lifeTime, age[i]/lifeTime, age[i]/lifeTime, age[i]/lifeTime);
			memVertices[i].position = vertices[i].position;
			memVertices[i].size = vertices[i].size;
		}
		else
		{
			if(randomFloat() < _dt)
				active[i] = true;
		}
	}
}