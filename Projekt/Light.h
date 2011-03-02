#ifndef LIGHT_H
#define LIGHT_H

#include <d3d10.h>
#include <d3dx10.h>

struct Light
{
	D3DXVECTOR3 position;
	float pad1;      // not used
	D3DXVECTOR3 direction;
	float pad2;      // not used
	D3DXCOLOR ambient;
	D3DXCOLOR diffuse;
	D3DXCOLOR specular;
	D3DXVECTOR3 att;
	float spotPow;
	float range;
};

#endif //LIGHT_H
