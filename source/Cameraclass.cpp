#include "cameraclass.h"

CameraClass::CameraClass()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;
}


CameraClass::CameraClass(const CameraClass& /*other*/)
{

}


CameraClass::~CameraClass()
{

}


void CameraClass::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
}


void CameraClass::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
}


DirectX::XMFLOAT3 CameraClass::GetPosition()
{
	return DirectX::XMFLOAT3(m_positionX, m_positionY, m_positionZ);
}


DirectX::XMFLOAT3 CameraClass::GetRotation()
{
	return DirectX::XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ);
}


void CameraClass::Render()
{
	DirectX::XMFLOAT3 up = {};
	DirectX::XMFLOAT3 position = {};
	DirectX::XMFLOAT3 lookAt = {};
	DirectX::XMVECTOR upVector = {};
	DirectX::XMVECTOR positionVector = {};
	DirectX::XMVECTOR lookAtVector = {};
	float yaw = 0.0f;
	float pitch = 0.0f;
	float roll = 0.0f;
	DirectX::XMMATRIX rotationMatrix;

	//Setup vecotr that points upwards
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	//Load it into XMVECTOR structure
	upVector = DirectX::XMLoadFloat3(&up);

	//Setup position of camera in world
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	//Load into XMVECTOR structure
	positionVector = DirectX::XMLoadFloat3(&position);

	//Setup where camera looks by default
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	//Load into XMVECTOR structure
	lookAtVector = DirectX::XMLoadFloat3(&lookAt);

	//Set yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	//Create rotation matrix from yaw, pitch, and roll
	rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	//Transform lookAt and up vector by rotation matrix so view is correctly rotated at origin
	lookAtVector = DirectX::XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = DirectX::XMVector3TransformCoord(upVector, rotationMatrix);

	//Translate rotated camera position to location of viewer
	lookAtVector = DirectX::XMVectorAdd(positionVector, lookAtVector);

	//Create view matrix from three updated vectors
	m_viewMatrix = DirectX::XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}


void CameraClass::GetViewMatrix(DirectX::XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
}
