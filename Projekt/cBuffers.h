#ifndef CBUFFERS_H
#define CBUFFERS_H

#include <windows.h>
#include <windowsx.h>
#include <d3d10.h>
#include <d3dx10.h>


//structs used when updating cbuffers with updateSubresource
struct PerFrame
{
	D3DXMATRIX viewProj;
};

struct PerObject
{
	D3DXMATRIX world;
};

#endif