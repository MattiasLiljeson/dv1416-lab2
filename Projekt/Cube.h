#ifndef CUBE_H
#define CUBE_H

#include "D3DObject.h"

class Cube : public D3DObject
{
private:
	float time;
	void defineVertexBuffer();
	void defineIndexBuffer();
	void defineInputElementDesc();

public:
	Cube(ID3D10Device* _device, ID3D10Effect* _effect, int _techNr = 0, int _passNr = 0);
	void update(float _dt);
};

#endif //CUBE_H