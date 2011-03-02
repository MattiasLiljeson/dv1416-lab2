#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include <vector>
#include <fstream>
using namespace std;

#include "D3DObject.h"

class HeightMap : public D3DObject
{
private:
	//typedef VertexTexture VertexType;

	//Vars
	float time;
	float cellSize;
	int nrOfCols;
	int nrOfRows;
	int nrOfFaces;

	vector<float> heightMap;

	//load multiple textures
	ID3D10ShaderResourceView* texBlendmap;
	vector<ID3D10ShaderResourceView*> texLayer;
	ID3D10EffectShaderResourceVariable* texVarBlendmap;
	vector<ID3D10EffectShaderResourceVariable*> texVarLayer;
	wstring texFileNameBlendmap;
	vector<wstring> texFileNameLayer;

	void initTextures();
	void setTextures();

	//vertices, indices etc
	void defineVertexBuffer();
	void defineIndexBuffer();
	void defineInputElementDesc();

	//Heightmap specific
	void loadHeightMap();
	void smoothHeightMap();
	float average(int posRow, int posCol);
	bool inBounds(int i, int j);

public:
	HeightMap(ID3D10Device* _device, ID3D10Effect* _effect, int _techNr = 0, int _passNr = 0);
	void update(float _dt);
	float getHeight(float x, float z);
};

#endif //HEIGHTMAP_H