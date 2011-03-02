#ifndef HYPERCRAFT_H
#define HYPERCRAFT_H

#include "D3DObject.h"

class Hypercraft : public D3DObject
{
private:
	float time;
	void initVertexBuffer();
	void initIndexBuffer();
	void initInputElementDesc();

	virtual void defineVertexBuffer();
	virtual void defineIndexBuffer();
	virtual void defineInputElementDesc();

public:
	Hypercraft(ID3D10Device* _device, ID3D10Effect* _effect, int _techNr = 0, int _passNr = 0);
	void update(float _dt);
};

#endif HYPERCRAFT_H