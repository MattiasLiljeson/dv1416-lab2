#include "Camera.h"

Camera::Camera(float _wndWidth, float _wndHeight, int _nearDist, int _farDist, ID3D10Device* _device, ID3D10Effect* _effect)
{
	device = _device;
	effect = _effect;

	vec_position.x = 0.0f;
	vec_position.y = 0.0f;
	vec_position.z = 0.0f;

	vec_up.x = 0.0f;
	vec_up.y = 1.0f;
	vec_up.z = 0.0f;

	vec_lookAt.x = 0.0f;
	vec_lookAt.y = 0.0f;
	vec_lookAt.z = 1.0f;

	vec_right.x = 1.0f;
	vec_right.y = 0.0f;
	vec_right.z = 0.0f;

	fov = (float)D3DXToRadian(45);
	aspectRatio = _wndWidth / _wndHeight;
	nearDist = _nearDist;
	farDist = _farDist;

	initFXVars();
	setProjection();

	frustum = new Frustum(vec_frustumCornersPos, device, effect, 0, 0);
	frustum->init();
}

void Camera::initFXVars()
{
	//fxVar_view			= effect->GetVariableByName("matView")->AsMatrix();
	//fxVar_viewInv		= effect->GetVariableByName("matViewInv")->AsMatrix();
	//fxVar_projection	= effect->GetVariableByName("matProjection")->AsMatrix();
	//fxVar_projectionInv = effect->GetVariableByName("matProjectionInv")->AsMatrix();
	fxVar_viewProj		= effect->GetVariableByName("viewProj")->AsMatrix();
	fxVar_cameraPos		= effect->GetVariableByName("cameraPos");
	//fxVar_farInfo		= effect->GetVariableByName("vecFarInfo");
}

void Camera::update(float _dt)
{
	//Normalize axes
	D3DXVec3Normalize(&vec_lookAt, &vec_lookAt);

	D3DXVec3Cross(&vec_up, &vec_lookAt, &vec_right);
	D3DXVec3Normalize(&vec_up, &vec_up);

	D3DXVec3Cross(&vec_right, &vec_up, &vec_lookAt);
	D3DXVec3Normalize(&vec_right, &vec_right);

	//vecPosition.z -= 0.01*_dt;
	setView();
	setFXVars();

	frustum->setWorld(mat_view);
	fxVar_viewProj->SetMatrix(&mat_viewProj._11);
}

void Camera::draw()
{
	frustum->draw();
}

D3DXMATRIX Camera::getCamera()
{
	return mat_view * mat_projection;
}

D3DXMATRIX Camera::getView()
{
	return mat_view;
}

D3DXVECTOR3 Camera::getPos()
{
	return vec_position;
}

void Camera::setPos(D3DXVECTOR4 _vec_pos)
{
	if(_vec_pos.w > 0.1) // >0.1  instead of ==1 for float precision problem
	{
		//point, replace
		vec_position.x = _vec_pos.x;
		vec_position.y = _vec_pos.y;
		vec_position.z = _vec_pos.z;
	}
	else
	{
		//vector, add
		vec_position.x += _vec_pos.x;
		vec_position.y += _vec_pos.y;
		vec_position.z += _vec_pos.z;
	}
}

void Camera::setLook(D3DXVECTOR4 _vec_look)
{
	D3DXVECTOR3 _vec_temp;

	if(_vec_look.w > 0-1) // >0  instead of ==1 for float precision problem
	{
		//point calculate normal from to - from
		_vec_temp.x = _vec_look.x - vec_position.x;
		_vec_temp.y = _vec_look.y - vec_position.y;
		_vec_temp.z = _vec_look.z - vec_position.z;

		D3DXVec3Normalize(&vec_lookAt, &_vec_temp);
	}
	else
	{
		//vector, replace
		_vec_temp.x = _vec_look.x + vec_lookAt.x;
		_vec_temp.y = _vec_look.y + vec_lookAt.y;
		_vec_temp.z = _vec_look.z + vec_lookAt.z;

		D3DXVec3Normalize(&vec_lookAt, &_vec_temp);
	}
}

void Camera::setHeight(float _height)
{
	vec_position.y = _height;
}

void Camera::setProjection()
{
	//D3DXMatrixPerspectiveFovLH(&matProjection, fov, aspectRatio, nearDist, farDist);

	D3DXMATRIX _mat_perspective;
	ZeroMemory(_mat_perspective, sizeof(D3DXMATRIX));

	_mat_perspective._11 = 1/(aspectRatio*(tan(fov/2)));
	_mat_perspective._22 = 1/(tan(fov/2));
	_mat_perspective._33 = farDist/(farDist - nearDist);
	_mat_perspective._34 = 1.0f;
	_mat_perspective._43 = (-nearDist * farDist)/(farDist - nearDist);

	mat_projection = _mat_perspective;

	farHeight = 2 * tan(fov/2) * farDist;
	farWidth = farHeight * aspectRatio;

	nearHeight = 2 * tan(fov/2) * nearDist;
	nearWidth = nearHeight * aspectRatio;

	// vec_frustumCornersPos follows this layout
	// 0------1
	// |      |
	// 2------3

	vec_frustumCornersPos[0] = D3DXVECTOR3(-farWidth/2,  farHeight/2, farDist);
	vec_frustumCornersPos[1] = D3DXVECTOR3( farWidth/2,  farHeight/2, farDist);
	vec_frustumCornersPos[2] = D3DXVECTOR3(-farWidth/2, -farHeight/2, farDist);
	vec_frustumCornersPos[3] = D3DXVECTOR3( farWidth/2, -farHeight/2, farDist);

	vec_frustumCornersPos[4] = D3DXVECTOR3(-nearWidth/2,  nearHeight/2, nearDist);
	vec_frustumCornersPos[5] = D3DXVECTOR3( nearWidth/2,  nearHeight/2, nearDist);
	vec_frustumCornersPos[6] = D3DXVECTOR3(-nearWidth/2, -nearHeight/2, nearDist);
	vec_frustumCornersPos[7] = D3DXVECTOR3( nearWidth/2, -nearHeight/2, nearDist);

	//D3DXVECTOR3 _vec_farInfo(farDist,farHeight,farWidth);
	//fxVar_farInfo->SetRawValue((void*)&_vec_farInfo, 0, sizeof(D3DXVECTOR3));

	//float _determinant = D3DXMatrixDeterminant(&mat_projection);
	//D3DXMatrixInverse(&mat_projectionInv, &_determinant, &mat_projection);
	//fxVar_projectionInv->SetMatrix(&mat_projectionInv._11);

}

void Camera::setView()
{
	float x = -D3DXVec3Dot(&vec_position, &vec_right);
	float y = -D3DXVec3Dot(&vec_position, &vec_up);
	float z = -D3DXVec3Dot(&vec_position, &vec_lookAt);

	mat_view(0,0) = vec_right.x; 
	mat_view(1,0) = vec_right.y; 
	mat_view(2,0) = vec_right.z; 
	mat_view(3,0) = x;   

	mat_view(0,1) = vec_up.x;
	mat_view(1,1) = vec_up.y;
	mat_view(2,1) = vec_up.z;
	mat_view(3,1) = y;  

	mat_view(0,2) = vec_lookAt.x; 
	mat_view(1,2) = vec_lookAt.y; 
	mat_view(2,2) = vec_lookAt.z; 
	mat_view(3,2) = z;   

	mat_view(0,3) = 0.0f;
	mat_view(1,3) = 0.0f;
	mat_view(2,3) = 0.0f;
	mat_view(3,3) = 1.0f;
}

void Camera::setFXVars()
{
	float _determinant = D3DXMatrixDeterminant(&mat_view);
	D3DXMatrixInverse(&mat_viewInv, &_determinant, &mat_view);

	//fxVar_view->SetMatrix(&mat_view._11);
	//fxVar_viewInv->SetMatrix(&mat_viewInv._11);

	mat_viewProj = mat_view * mat_projection;
	fxVar_viewProj->SetMatrix(&mat_viewProj._11);
	fxVar_cameraPos->SetRawValue((void*)&vec_position, 0, sizeof(D3DXVECTOR3));
}

void Camera::strafe(float _amount, float _dt)
{
	vec_position += _amount*_dt*vec_right;
}

void Camera::walk(float _amount, float _dt)
{
	vec_position += _amount*_dt*vec_lookAt;
}

void Camera::pitch(float _angle, float _dt)
{
	D3DXMATRIX _mat_rotation;
	D3DXMatrixRotationAxis(&_mat_rotation, &vec_right, _angle/_dt);

	D3DXVec3TransformNormal(&vec_up, &vec_up, &_mat_rotation);
	D3DXVec3TransformNormal(&vec_lookAt, &vec_lookAt, &_mat_rotation);
}

void Camera::rotateY(float _angle, float _dt)
{
	D3DXMATRIX _matRotation;
	D3DXMatrixRotationY(&_matRotation, _angle/_dt);

	D3DXVec3TransformNormal(&vec_right, &vec_right, &_matRotation);
	D3DXVec3TransformNormal(&vec_up, &vec_up, &_matRotation);
	D3DXVec3TransformNormal(&vec_lookAt, &vec_lookAt, &_matRotation);
}