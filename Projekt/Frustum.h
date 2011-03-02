#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "D3DObject.h"

class Frustum : public D3DObject
{
private:
	D3DXVECTOR3 vec_corners[8];
	void defineVertexBuffer();
	void defineIndexBuffer();
	void defineInputElementDesc();

public:
	Frustum(D3DXVECTOR3 _vec_corners[8], ID3D10Device* _device, ID3D10Effect* _effect, int _techNr = 0, int _passNr = 0);
	void update(float _dt);
	void setWorld(D3DXMATRIX _world);
};

#endif //FRUSTUM_H