#include "PaprikaFlame.h"

PaprikaFlame::PaprikaFlame(ID3D10Device* _device, ID3D10Effect* _effect, Camera* _camera, int _techNr, int _passNr):
	ParticleSystem(_device, _effect, _camera, _techNr, _passNr)
{
}

void PaprikaFlame::defineVars()
{
	numVertices = 1000;
	emitterPos.x = 0.0f;
	emitterPos.y = 3.0f;
	emitterPos.z = 10.0f;
	size.x = 0.1f;
	size.y = 0.1f;

	lifeTime = 3.0f;
	texFileName = L"resources/paprika.png";
}

void PaprikaFlame::defineVertices()
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
		speed.push_back(randomFloat());
		accel.push_back(randomFloat());

		dir.push_back(randomVectorPosY());
	}
}

void PaprikaFlame::updateVertices(VertexType* memVertices, float _dt)
{
	
	bool activated = false;
	for(UINT i=0; i<numVertices; i++)
	{
		if(age[i] > lifeTime)
		{
			active[i] = false;
			age[i] = 0.0f;
			speed[i] = 0;
			vertices[i].position = emitterPos;

			memVertices[i].position = emitterPos;
		}

		if(active[i])
		{
			vertices[i].position += D3DXVECTOR3(speed[i]*dir[i].x*_dt, speed[i]*dir[i].y*_dt, speed[i]*dir[i].z*_dt);

			dir[i] += randomVectorPosY();
			D3DXVec3Normalize(&dir[i], &dir[i]);

			speed[i] += accel[i]*_dt;
			age[i] += _dt;

			memVertices[i].fade = D3DXVECTOR4(age[i]/lifeTime, age[i]/lifeTime, age[i]/lifeTime, age[i]/lifeTime);
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