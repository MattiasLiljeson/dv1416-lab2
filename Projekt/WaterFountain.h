#ifndef WATERFOUNTAIN_H
#define WATERFOUNTAIN_H

#include "ParticleSystem.h"

class WaterFountain: public ParticleSystem
{
private:
	//Vars
	D3DXVECTOR3 emitterPos;
	D3DXVECTOR2 size;
	float lifeTime;
	vector<bool> active;
	vector<float> age;
	vector<D3DXVECTOR3> speed;
	D3DXVECTOR3 accel;

	void defineVars();
	void defineVertices();
	void defineBlendState();

	void updateVertices(VertexType* memVertices, float _dt);

public:
	WaterFountain(ID3D10Device* _device, ID3D10Effect* _effect, Camera* _camera, int _techNr, int _passNr);
};

#endif //WATERFOUNTAIN_H