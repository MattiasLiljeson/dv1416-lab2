#ifndef VERTEXBLENDMAP_H
#define VERTEXBLENDMAP_H

struct VertexBlendmap
{
	D3DXVECTOR3 position;
	D3DXVECTOR3 normal;
	float tiledU, tiledV;
	float stretchedU, stretchedV;
};

#endif //VERTEXBLENDMAP_H

//struct VertexBlendmap {D3DXVECTOR3 Position; D3DXVECTOR3 Normal; float U, V;};