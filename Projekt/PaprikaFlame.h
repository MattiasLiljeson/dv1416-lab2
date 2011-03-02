#ifndef PAPRIKAFLAME_H
#define PAPRIKAFLAME_H

#include "ParticleSystem.h"

class PaprikaFlame: public ParticleSystem
{
private:
	//Vars
	D3DXVECTOR3 emitterPos;
	D3DXVECTOR2 size;
	float lifeTime;
	vector<bool> active;
	vector<float> age;
	vector<float> speed;
	vector<float> accel;
	vector<D3DXVECTOR3> dir;

	void defineVars();
	void defineVertices();
	void updateVertices(VertexType* memVertices, float _dt);

public:
	PaprikaFlame(ID3D10Device* _device, ID3D10Effect* _effect, Camera* _camera, int _techNr, int _passNr);

};

#endif //PAPRIKAFLAME_H