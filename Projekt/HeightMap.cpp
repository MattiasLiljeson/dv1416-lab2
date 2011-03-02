#include "HeightMap.h"
HeightMap::HeightMap(ID3D10Device* _device, ID3D10Effect* _effect, int _techNr, int _passNr): D3DObject(_device, _effect, _techNr, _passNr)
{
	//Vars for the HeightMap
	time = 0;
	cellSize = 1.0f;
	nrOfCols = 257;
	nrOfRows = 257;
	nrOfFaces = (nrOfRows-1) * (nrOfCols-1) * 2;
	nrOfVertices = nrOfRows*nrOfCols;
	nrOfIndices = nrOfFaces*3;

	loadHeightMap();
	smoothHeightMap();

	texFileNameBlendmap = L"resources/blendmap3.png";
	texFileNameLayer.push_back (L"resources/wood.png");
	texFileNameLayer.push_back (L"resources/bricks.jpg");
	texFileNameLayer.push_back (L"resources/perlin.jpg");
	texFileNameLayer.push_back (L"resources/tileBrick.jpg");
	texFileNameLayer.push_back (L"resources/stones.jpg");
}

void HeightMap::initTextures()
{
	HR(D3DX10CreateShaderResourceViewFromFile(device,
		texFileNameBlendmap.c_str(),
		NULL, NULL, &texBlendmap, NULL));

	for(int i = 0; i<5; i++)
	{
		texLayer.push_back(NULL);

		HR(D3DX10CreateShaderResourceViewFromFile(device,
			texFileNameLayer[i].c_str(),
			NULL, NULL, &texLayer[i], NULL));
	}
}

void HeightMap::setTextures()
{
	// set which texture to use
	textureVariable = effect->GetVariableByName("texBlendmap")->AsShaderResource();
    textureVariable->SetResource(texBlendmap);

	textureVariable = effect->GetVariableByName("tex0")->AsShaderResource();
	textureVariable->SetResource(texLayer[0]);
	textureVariable = effect->GetVariableByName("tex1")->AsShaderResource();
	textureVariable->SetResource(texLayer[1]);
	textureVariable = effect->GetVariableByName("tex2")->AsShaderResource();
	textureVariable->SetResource(texLayer[2]);
	textureVariable = effect->GetVariableByName("tex3")->AsShaderResource();
	textureVariable->SetResource(texLayer[3]);
	textureVariable = effect->GetVariableByName("tex4")->AsShaderResource();
	textureVariable->SetResource(texLayer[4]);
}

void HeightMap::loadHeightMap()
{
	float scale = 0.1f;
	float offset = 0.0f;

	vector<unsigned char> in(nrOfVertices);
	ifstream inFile;
	inFile.open("resources/korb.raw",  std::ios_base::binary);

	if(inFile)
	{
		inFile.read((char*)&in[0], in.size());
		inFile.close();
	}

	heightMap.resize(nrOfVertices,0);
	for(int i=0; i<nrOfVertices; i++)
		heightMap[i] = in[i]*scale + offset;

}

bool HeightMap::inBounds(int i, int j)
{
	//return (i >= 0 && i<nrOfRows) && (j >= 0 && j<nrOfCols);
	return i >= 0 && i < nrOfRows &&
		   j >= 0 && j < nrOfCols;
}

float HeightMap::average(int posRow, int posCol)
{
	float avg = 0.0f;
	float num = 0.0f;

	for(int i = posRow-1; i <= posRow+1; i++)
	{
		for(int j = posCol-1; j <= posCol+1; j++)
		{
			if(inBounds(i,j))
			{
				avg += heightMap[i*nrOfCols + j];
				num += 1.0f;
			}
		}
	}

	return avg / num;
}

void HeightMap::smoothHeightMap()
{
	vector<float> dest(heightMap.size());

	for(int i=0; i < nrOfRows; i++)
		for(int j=0; j < nrOfCols; j++)
			dest[nrOfCols*i + j] = average(i,j);

	heightMap = dest;
}

float HeightMap::getHeight(float x, float z)
{
	// Transform from terrain local space to "cell" space.
	float c = (x + 0.5f*(nrOfCols-1)*cellSize) / cellSize;
	float d = (z - 0.5f*(nrOfRows-1)*cellSize) / -cellSize;

	// Get the row and column we are in.
	int row = (int)floorf(d);
	int col = (int)floorf(c);

	// Grab the heights of the cell we are in.
	// A 0--0 B
	//   | /|
	//   |/ |
	// C 0--0 D
	float A = heightMap[row*nrOfCols + col];
	float B = heightMap[row*nrOfCols + col + 1];
	float C = heightMap[(row+1)*nrOfCols + col];
	float D = heightMap[(row+1)*nrOfCols + col + 1];

	// Where we are relative to the cell.
	float s = c - (float)col;
	float t = d - (float)row;

	if( s + t <= 1.0f)		// If upper triangle ABC.
	{
		float uy = B - A;
		float vy = C - A;
		return A + s*uy + t*vy;
	}
	else	// lower triangle DCB.
	{
		float uy = C - D;
		float vy = B - D;
		return D + (1.0f-s)*uy + (1.0f-t)*vy;
	}
}

void HeightMap::defineVertexBuffer()
{
	int pos;
	float x, y, z;

	//create geometry
	vector<VertexType> vertices(nrOfVertices);
	float halfWidth = (nrOfCols-1)*cellSize*0.5f;
	float halfDepth = (nrOfRows-1)*cellSize*0.5f;

	float du = 1.0f / (nrOfCols-1);
	float dv = 1.0f / (nrOfRows-1);

	for(int i=0; i<nrOfRows; i++)
	{
		z = halfDepth - i*cellSize;
		for(int j=0; j<nrOfCols; j++)
		{
			x = -halfWidth + j*cellSize;

			//get height
			pos = i*nrOfCols + j;
			y = heightMap[pos];

			vertices[pos].Position = D3DXVECTOR3(x,y,z);
			vertices[pos].Normal = D3DXVECTOR3(0,1,0);
			vertices[pos].texCord.x = (float)(j*du);
			vertices[pos].texCord.y = (float)(i*dv);
		}
	}

	// Estimate normals for interior nodes using central difference.
	float invTwoDX = 1.0f / (2.0f*cellSize);
	float invTwoDZ = 1.0f / (2.0f*cellSize);
	for(int i = 2; i <nrOfRows-1; ++i)
	{
		for(int j = 2; j < nrOfCols-1; ++j)
		{
			float t = heightMap[(i-1)*nrOfCols + j];
			float b = heightMap[(i+1)*nrOfCols + j];
			float l = heightMap[i*nrOfCols + j - 1];
			float r = heightMap[i*nrOfCols + j + 1];

			D3DXVECTOR3 tanZ(0.0f, (t-b)*invTwoDZ, 1.0f);
			D3DXVECTOR3 tanX(1.0f, (r-l)*invTwoDX, 0.0f);

			D3DXVECTOR3 N;
			D3DXVec3Cross(&N, &tanZ, &tanX);
			D3DXVec3Normalize(&N, &N);

			vertices[i*nrOfCols+j].Normal = N;
		}
	}

	D3D10_BUFFER_DESC vertBuffDesc;

    vertBuffDesc.Usage = D3D10_USAGE_IMMUTABLE;
	vertBuffDesc.ByteWidth = sizeof(VertexType) * nrOfVertices;
    vertBuffDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    vertBuffDesc.CPUAccessFlags = 0;
    vertBuffDesc.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA vertBuffSubRes;
	vertBuffSubRes.pSysMem = &vertices[0];

    HR(device->CreateBuffer(&vertBuffDesc, &vertBuffSubRes, &vertexBuffer));
}

void HeightMap::defineIndexBuffer()
{
	int k = 0;

	vector<DWORD> indices(nrOfIndices);
	for(int i = 0; i<nrOfRows-1; i++)
	{
		for(int j = 0; j<nrOfCols-1; j++)
		{
			indices[k]   = i*nrOfCols+j;
			indices[k+1] = i*nrOfCols+j+1;
			indices[k+2] = (i+1)*nrOfCols+j;

			indices[k+3] = (i+1)*nrOfCols+j;
			indices[k+4] = i*nrOfCols+j+1;
			indices[k+5] = (i+1)*nrOfCols+j+1;

			k += 6; // next quad
		}
	}

    // create the index buffer
	D3D10_BUFFER_DESC indBuffDesc;
    indBuffDesc.Usage = D3D10_USAGE_IMMUTABLE;
    indBuffDesc.ByteWidth = sizeof(DWORD) * nrOfFaces * 3;
    indBuffDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
    indBuffDesc.CPUAccessFlags = 0;
    indBuffDesc.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA indBuffSubRes;
    indBuffSubRes.pSysMem = &indices[0];

    HR(device->CreateBuffer(&indBuffDesc, &indBuffSubRes, &indexBuffer));
}

void HeightMap::defineInputElementDesc()
{
	// create the input element descriptions
	nrOfElements = 3;
    layout = new D3D10_INPUT_ELEMENT_DESC[nrOfElements];

	layout[0].SemanticName = "POSITION";
	layout[0].SemanticIndex = 0;
	layout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layout[0].InputSlot = 0;
	layout[0].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
	layout[0].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	layout[0].InstanceDataStepRate = 0;

	//use normal instead of color
	layout[1].SemanticName = "NORMAL";
	layout[1].SemanticIndex = 0;
	layout[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layout[1].InputSlot = 0;
	layout[1].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
	layout[1].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	layout[1].InstanceDataStepRate = 0;

	//texture
	layout[2].SemanticName = "TEXCOORD";
	layout[2].SemanticIndex = 0;
	layout[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	layout[2].InputSlot = 0;
	layout[2].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
	layout[2].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	layout[2].InstanceDataStepRate = 0;

	//layout[3].SemanticName = "TEXCOORD1";
	//layout[3].SemanticIndex = 1;
	//layout[3].Format = DXGI_FORMAT_R32G32_FLOAT;
	//layout[3].InputSlot = 0;
	//layout[3].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
	//layout[3].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	//layout[3].InstanceDataStepRate = 0;
}

void HeightMap::update(float _dt)
{
	//For time-rotating effect in shader
	// increase the time varaible and send to the effect
    time += 0.001f;

	// create a rotation matrix
	//D3DXMatrixRotationY(&matRotate, time);
	//D3DXMatrixTranslation(&matTranslate, 0, 0, -time);
}