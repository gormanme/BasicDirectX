#include "frustumclass.h"

FrustumClass::FrustumClass()
{

}


FrustumClass::FrustumClass(const FrustumClass& /*other*/)
{

}


FrustumClass::~FrustumClass()
{

}


void FrustumClass::ConstructFrustum(float screenDepth, DirectX::XMMATRIX projectionMatrix, DirectX::XMMATRIX viewMatrix)
{
	float zMinimum = 0.0f;
	float r = 0.0f;
	DirectX::XMMATRIX matrix = {};

	//Calculate the minimum Z distance in the frustum
	zMinimum = -projectionMatrix._43 / projectionMatrix._33;
	r = screenDepth / (screenDepth - zMinimum);
	projectionMatrix._33 = r;
	projectionMatrix._43 = -r * zMinimum;

	//Create the frustum matrix from the view matrix and updated projection matrix
	matrix = DirectX::XMMatrixMultiply(viewMatrix, projectionMatrix);

	//Calculate near plane of frustum
	m_planes[0] = DirectX::XMVectorSet((matrix._14 + matrix._13), (matrix._24 + matrix._23), (matrix._34 + matrix._33), (matrix._44 + matrix._43));
	m_planes[0] = DirectX::XMPlaneNormalize(m_planes[0]);

	//Calculate far plane of frustum
	m_planes[1] = DirectX::XMVectorSet((matrix._14 - matrix._13), (matrix._24 - matrix._23), (matrix._34 - matrix._33), (matrix._44 - matrix._43));
	m_planes[1] = DirectX::XMPlaneNormalize(m_planes[1]);

	//Calculate left plane of frustum
	m_planes[2] = DirectX::XMVectorSet((matrix._14 + matrix._11), (matrix._24 + matrix._21), (matrix._34 + matrix._31), (matrix._44 + matrix._41));
	m_planes[2] = DirectX::XMPlaneNormalize(m_planes[2]);

	//Calculate right plane of frustum
	m_planes[3] = DirectX::XMVectorSet((matrix._14 - matrix._11), (matrix._24 - matrix._21), (matrix._34 - matrix._31), (matrix._44 - matrix._41));
	m_planes[3] = DirectX::XMPlaneNormalize(m_planes[3]);

	//Calculate top plane of frustum
	m_planes[4] = DirectX::XMVectorSet((matrix._14 - matrix._12), (matrix._24 - matrix._22), (matrix._34 - matrix._32), (matrix._44 - matrix._42));
	m_planes[4] = DirectX::XMPlaneNormalize(m_planes[4]);

	//Calculate bottom plane of frustum
	m_planes[5] = DirectX::XMVectorSet((matrix._14 + matrix._12), (matrix._24 + matrix._22), (matrix._34 + matrix._32), (matrix._44 + matrix._42));
	m_planes[5] = DirectX::XMPlaneNormalize(m_planes[5]);

}


bool FrustumClass::CheckPoint(float x, float y, float z)
{
	DirectX::XMVECTOR vector = DirectX::XMVectorSet(x, y, z, 0.0f);
	DirectX::XMVECTOR result = {};
	//Check if the point is inside all six planes of the view frustum
	for (int i = 0; i < 6; i++)
	{
		result = DirectX::XMPlaneDotCoord(m_planes[i], vector);
		if (DirectX::XMVectorGetX(result) < 0.0f)
		{
			return false;
		}
	}

	return true;
}


bool FrustumClass::CheckCube(float xCenter, float yCenter, float zCenter, float radius)
{
	DirectX::XMVECTOR vector = {};
	DirectX::XMVECTOR dot = {};

	//Check if any one point of the cube is in the view frustum
	for (int i = 0; i < 6; i++)
	{
		vector = DirectX::XMVectorSet((xCenter - radius), (yCenter - radius), (zCenter - radius), 0.0f);
		dot = DirectX::XMPlaneDotCoord(m_planes[i], vector);
		if (DirectX::XMVectorGetX(dot) >= 0.0f)
		{
			continue;
		}

		vector = DirectX::XMVectorSet((xCenter + radius), (yCenter - radius), (zCenter - radius), 0.0f);
		dot = DirectX::XMPlaneDotCoord(m_planes[i], vector);
		if (DirectX::XMVectorGetX(dot) >= 0.0f)
		{
			continue;
		}

		vector = DirectX::XMVectorSet((xCenter - radius), (yCenter + radius), (zCenter - radius), 0.0f);
		dot = DirectX::XMPlaneDotCoord(m_planes[i], vector);
		if (DirectX::XMVectorGetX(dot) >= 0.0f)
		{
			continue;
		}

		vector = DirectX::XMVectorSet((xCenter + radius), (yCenter + radius), (zCenter - radius), 0.0f);
		dot = DirectX::XMPlaneDotCoord(m_planes[i], vector);
		if (DirectX::XMVectorGetX(dot) >= 0.0f)
		{
			continue;
		}

		vector = DirectX::XMVectorSet((xCenter - radius), (yCenter - radius), (zCenter + radius), 0.0f);
		dot = DirectX::XMPlaneDotCoord(m_planes[i], vector);
		if (DirectX::XMVectorGetX(dot) >= 0.0f)
		{
			continue;
		}

		vector = DirectX::XMVectorSet((xCenter + radius), (yCenter - radius), (zCenter + radius), 0.0f);
		dot = DirectX::XMPlaneDotCoord(m_planes[i], vector);
		if (DirectX::XMVectorGetX(dot) >= 0.0f)
		{
			continue;
		}

		vector = DirectX::XMVectorSet((xCenter - radius), (yCenter + radius), (zCenter + radius), 0.0f);
		dot = DirectX::XMPlaneDotCoord(m_planes[i], vector);
		if (DirectX::XMVectorGetX(dot) >= 0.0f)
		{
			continue;
		}

		vector = DirectX::XMVectorSet((xCenter + radius), (yCenter + radius), (zCenter + radius), 0.0f);
		dot = DirectX::XMPlaneDotCoord(m_planes[i], vector);
		if (DirectX::XMVectorGetX(dot) >= 0.0f)
		{
			continue;
		}

		return false;
	}

	return true;
}


bool FrustumClass::CheckSphere(float xCenter, float yCenter, float zCenter, float radius)
{
	DirectX::XMVECTOR dot = {};
	DirectX::XMVECTOR vector = DirectX::XMVectorSet(xCenter, yCenter, zCenter, 0.0f);

	//Check if the radius of the sphere is inside the view frustum
	for (int i = 0; i < 6; i++)
	{
		dot = DirectX::XMPlaneDotCoord(m_planes[i], vector);
		if (DirectX::XMVectorGetX(dot) < -radius)
		{
			return false;
		}

	}

	return true;
}


bool FrustumClass::CheckRectangle(float xCenter, float yCenter, float zCenter, float xSize, float ySize, float zSize)
{
	DirectX::XMVECTOR vector = {};
	DirectX::XMVECTOR dot = {};

	//Check if any one point of the cube is in the view frustum
	for (int i = 0; i < 6; i++)
	{
		vector = DirectX::XMVectorSet((xCenter - xSize), (yCenter - ySize), (zCenter - zSize), 0.0f);
		dot = DirectX::XMPlaneDotCoord(m_planes[i], vector);
		if (DirectX::XMVectorGetX(dot) >= 0.0f)
		{
			continue;
		}

		vector = DirectX::XMVectorSet((xCenter + xSize), (yCenter - ySize), (zCenter - zSize), 0.0f);
		dot = DirectX::XMPlaneDotCoord(m_planes[i], vector);
		if (DirectX::XMVectorGetX(dot) >= 0.0f)
		{
			continue;
		}

		vector = DirectX::XMVectorSet((xCenter - xSize), (yCenter + ySize), (zCenter - zSize), 0.0f);
		dot = DirectX::XMPlaneDotCoord(m_planes[i], vector);
		if (DirectX::XMVectorGetX(dot) >= 0.0f)
		{
			continue;
		}

		vector = DirectX::XMVectorSet((xCenter + xSize), (yCenter + ySize), (zCenter - zSize), 0.0f);
		dot = DirectX::XMPlaneDotCoord(m_planes[i], vector);
		if (DirectX::XMVectorGetX(dot) >= 0.0f)
		{
			continue;
		}

		vector = DirectX::XMVectorSet((xCenter - xSize), (yCenter - ySize), (zCenter + zSize), 0.0f);
		dot = DirectX::XMPlaneDotCoord(m_planes[i], vector);
		if (DirectX::XMVectorGetX(dot) >= 0.0f)
		{
			continue;
		}

		vector = DirectX::XMVectorSet((xCenter + xSize), (yCenter - ySize), (zCenter + zSize), 0.0f);
		dot = DirectX::XMPlaneDotCoord(m_planes[i], vector);
		if (DirectX::XMVectorGetX(dot) >= 0.0f)
		{
			continue;
		}

		vector = DirectX::XMVectorSet((xCenter - xSize), (yCenter + ySize), (zCenter + zSize), 0.0f);
		dot = DirectX::XMPlaneDotCoord(m_planes[i], vector);
		if (DirectX::XMVectorGetX(dot) >= 0.0f)
		{
			continue;
		}

		vector = DirectX::XMVectorSet((xCenter + xSize), (yCenter + ySize), (zCenter + zSize), 0.0f);
		dot = DirectX::XMPlaneDotCoord(m_planes[i], vector);
		if (DirectX::XMVectorGetX(dot) >= 0.0f)
		{
			continue;
		}

		return false;
	}

	return true;
}