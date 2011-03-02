#include "SuperNova.h"

SuperNova::SuperNova(ID3D10Device* _device, ID3D10Effect* _effect, Camera* _camera, int _techNr, int _passNr):
	ParticleSystem(_device, _effect, _camera, _techNr, _passNr)
{
}

void SuperNova::defineVars()
{
	numVertices = 1000;
	emitterPos.x = 0.0f;
	emitterPos.y = 15.0f;
	emitterPos.z = 20.0f;
	size.x = 0.1f;
	size.y = 0.1f;

	lifeTime = 5.0f;
	texFileName = L"resources/superNova.png";
}

void SuperNova::defineVertices()
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
		speed.push_back(randomVector());
		speed[i].y = 0;
		speed[i] *= 30.f;
		accel = -0.1f;
	}
}

void SuperNova::updateVertices(VertexType* memVertices, float _dt)
{
	
	bool activated = false;
	for(UINT i=0; i<numVertices; i++)
	{
		if(age[i] > lifeTime)
		{
			active[i] = false;
			age[i] = 0.0f;
			speed[i] = randomVector();
			speed[i].y = 0;
			speed[i] *= 30.f;
			vertices[i].position = emitterPos;

			memVertices[i].position = emitterPos;
		}

		if(active[i])
		{
			vertices[i].position += speed[i]*_dt;

			speed[i].x += accel*_dt;
			speed[i].z += accel*_dt;
			age[i] += _dt;

			memVertices[i].fade = D3DXVECTOR4(age[i]/lifeTime, age[i]/lifeTime, age[i]/lifeTime, age[i]/lifeTime);
			memVertices[i].position = vertices[i].position;
			memVertices[i].size = vertices[i].size;
		}
		else
		{
			active[i] = true;
		}
	}
}