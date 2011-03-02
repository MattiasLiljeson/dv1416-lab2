#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "D3DObject.h"

class Triangle : public D3DObject
{
private:
	float time;
	void initVertexBuffer();
	void initIndexBuffer();
	void initInputElementDesc();

public:
	Triangle(ID3D10Device* _device, ID3D10Effect* _effect, int _techNr = 0, int _passNr = 0);
	void update(float _dt);
};

#endif TRIANGLE_H