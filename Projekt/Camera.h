#ifndef CAMERA_H
#define CAMERA_H

#include <cmath>

#include <d3d10.h>
#include <d3dx10.h>
#include "Utils.h"
#include "Frustum.h"

class Camera
{
private:
	ID3D10Device* device;
	ID3D10Effect* effect;

	//effect variables
	ID3D10EffectMatrixVariable* fxVar_view;
	ID3D10EffectMatrixVariable* fxVar_viewInv;
	ID3D10EffectMatrixVariable* fxVar_projection;
	ID3D10EffectMatrixVariable* fxVar_projectionInv;
	ID3D10EffectMatrixVariable* fxVar_viewProj;
	ID3D10EffectVariable*       fxVar_cameraPos; //raw
	ID3D10EffectVariable*       fxVar_farInfo;   //raw
	ID3D10EffectScalarVariable* fxVar_farDist;
	ID3D10EffectScalarVariable* fxVar_farHeight;
	ID3D10EffectScalarVariable* fxVar_farWidth;

	D3DXMATRIX mat_view;
	D3DXMATRIX mat_viewInv;
	D3DXMATRIX mat_projection;
	D3DXMATRIX mat_projectionInv;
	D3DXMATRIX mat_viewProj;

	D3DXVECTOR3 vec_position;
	D3DXVECTOR3 vec_lookAt;
	D3DXVECTOR3 vec_up;
	D3DXVECTOR3 vec_right;

	float fov;
	float aspectRatio;
	float farDist;
	float nearDist;
	float farHeight;
	float nearHeight;
	float farWidth;
	float nearWidth;

	D3DXVECTOR3 vec_frustumCornersPos[8];
	D3DXPLANE plane_frustumSides[6];
	Frustum* frustum; //3d-object representing the frustum in the world

	virtual void initFXVars();
	virtual void setFXVars();

public:
	Camera(float _wndWidth, float _wndHeight, int _nearDist, int _farDist, ID3D10Device* _device, ID3D10Effect* _effect);
	void update(float _dt);
	void draw();
	void setProjection();
	void setView();
	D3DXMATRIX getView();
	D3DXMATRIX getCamera();
	D3DXVECTOR3 getPos();
	void setPos(D3DXVECTOR4 _vec_pos);
	void setLook(D3DXVECTOR4 _vec_look);
	void setHeight(float _height);
	void strafe(float amount, float _dt);
	void walk(float amount, float _dt);
	void pitch(float angle, float _dt);
	void rotateY(float angle, float _dt);
};

#endif