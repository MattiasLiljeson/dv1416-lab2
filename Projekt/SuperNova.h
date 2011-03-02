#ifndef SUPERNOVA_H
#define SUPERNOVA_H

#include "ParticleSystem.h"

class SuperNova: public ParticleSystem
{
private:
	//Vars
	D3DXVECTOR3 emitterPos;
	D3DXVECTOR2 size;
	float lifeTime;
	vector<bool> active;
	vector<float> age;
	vector<D3DXVECTOR3> speed;
	float accel;

	void defineVars();
	void defineVertices();
	void updateVertices(VertexType* memVertices, float _dt);

public:
	SuperNova(ID3D10Device* _device, ID3D10Effect* _effect, Camera* _camera, int _techNr, int _passNr);
};

#endif //SUPERNOVA_H